/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "layout_internal.h"

#include <memory.h>

#include "board.h"
#include "hid.h"
#include "keycodes.h"
#include "switches.h"
#include "timer.h"
#include "user_config.h"

static uint16_t active_keycodes[NUM_KEYS];
static uint8_t last_sw_states[NUM_KEYS];

void layout_init(void) {
    memset(active_keycodes, 0, sizeof(active_keycodes));
    memset(last_sw_states, 0, sizeof(last_sw_states));

    layout_layer_init();
    layout_event_init();
    layout_tap_hold_event_init();
    layout_post_hid_report_event_init();
}

void layout_task(void) {
    static uint32_t last_tick_event = 0;

    matrix_scan();

    bool matrix_changed = false;
    for (uint32_t i = 0; i < NUM_KEYS; i++) {
        const uint8_t sw_state = get_switch_state(i);

        if (sw_state != last_sw_states[i]) {
            layout_event_push(i, sw_state);
            matrix_changed = true;
        }
    }

    if (matrix_changed || timer_elapsed(last_tick_event) > 0) {
        layout_tick_event();
        last_tick_event = timer_read();

        if (matrix_changed)
            layout_process_events();

        hid_send_reports();
    }
}

void layout_post_hid_report_task(void) {
    layout_process_post_hid_report_events();
}

//--------------------------------------------------------------------+
// Layer APIs
//--------------------------------------------------------------------+

static uint8_t default_layer_num;
static uint16_t layer_mask;

void layout_layer_init(void) {
    default_layer_num = 0;
    layer_mask = 0;
}

uint8_t get_current_layer(void) {
    if (!layer_mask)
        return default_layer_num;

    return 31 - __builtin_clz(layer_mask);
}

void layer_on(uint8_t layer_num) { layer_mask |= 1 << layer_num; }

void layer_off(uint8_t layer_num) { layer_mask &= ~(1 << layer_num); }

void layer_toggle(uint8_t layer_num) { layer_mask ^= 1 << layer_num; }

void layer_goto(uint8_t layer_num) { layer_mask = 1 << layer_num; }

void set_default_layer(uint8_t layer_num) { default_layer_num = layer_num; }

uint16_t get_keycode(uint16_t index) {
    const uint8_t current_profile = user_config_current_profile();

    // Find the highest active layer with a non-transparent keycode
    uint8_t mask = 1 << get_current_layer();
    for (uint8_t i = get_current_layer() + 1; i-- > 0; mask >>= 1) {
        if (!(layer_mask & mask))
            continue;

        const uint16_t keycode = user_config_keymap(current_profile, i, index);
        if (keycode != KC_TRANSPARENT)
            return keycode;
    }

    // Otherwise, use the default layer
    return user_config_keymap(current_profile, default_layer_num, index);
}

//--------------------------------------------------------------------+
// Layout APIs
//--------------------------------------------------------------------+

static uint32_t pending_event_head, num_pending_events;
static layout_event_t pending_events[MAX_PENDING_EVENTS];

void layout_event_init(void) {
    pending_event_head = 0;
    num_pending_events = 0;
    memset(pending_events, 0, sizeof(pending_events));
}

void layout_event_push(uint16_t index, uint8_t sw_state) {
    if (num_pending_events == MAX_PENDING_EVENTS)
        // The event queue is full
        pending_event_head =
            (pending_event_head + 1) & (MAX_PENDING_EVENTS - 1);

    const uint32_t pending_event_tail =
        (pending_event_head + num_pending_events) & (MAX_PENDING_EVENTS - 1);
    pending_events[pending_event_tail].index = index;
    pending_events[pending_event_tail].sw_state = sw_state;
    num_pending_events++;
}

void layout_process_dks(uint16_t index, uint8_t config_num, uint8_t sw_state,
                        uint8_t last_sw_state) {
    if (config_num >= NUM_DYNAMIC_KEYSTROKE_CONFIGS)
        return;

    const dynamic_keystroke_config_t *config =
        user_config_dynamic_keystroke_config(config_num);

    for (uint32_t i = 0; i < 4; i++) {
        const uint8_t keycode = config->keycode[i];
        const dynamic_keystroke_mask_t *mask = &config->mask[i];

        if (keycode == KC_NO || mask == 0)
            continue;

        // The keycode should be removed first if it is active from the previous
        // DKS event and should be relased before this event.
        bool should_remove_keycode = false;
        uint8_t current_config;

        if (IS_SW_PRESSED(sw_state, last_sw_state)) {
            should_remove_keycode = false;
            current_config = mask->config0;
        } else if (IS_SW_PRESSED_BOTTOMED_OUT(sw_state, last_sw_state)) {
            should_remove_keycode = (mask->config0 == 2);
            current_config = mask->config1;
        } else if (IS_SW_RELEASED_BOTTOMED_OUT(sw_state, last_sw_state)) {
            should_remove_keycode = (mask->config0 == 3 || mask->config1 == 2);
            current_config = mask->config2;
        } else if (IS_SW_RELEASED(sw_state, last_sw_state)) {
            should_remove_keycode = true;
            current_config = mask->config3;
        } else {
            // Should be unreachable
            continue;
        }

        if (should_remove_keycode) {
            hid_remove_keycode(keycode);
            if (current_config == 1)
                // Tap action
                layout_post_hid_report_event_push(index, keycode,
                                                  POST_HID_REPORT_ACTION_TAP);
            else if (current_config > 1)
                // Hold action
                layout_post_hid_report_event_push(index, keycode,
                                                  POST_HID_REPORT_ACTION_ADD);
        } else {
            if (current_config == 1)
                // Tap action
                layout_process_tap_action(index, keycode);
            else if (current_config > 1)
                // Hold action
                hid_add_keycode(keycode);
        }
    }
}

void layout_process_magic_keycode(uint16_t keycode) {
    switch (keycode) {
#if defined(ENABLE_BOOTLOADER)
    case SP_MAGIC_BOOTLOADER:
        board_enter_bootloader();
        break;
#endif

    case SP_MAGIC_REBOOT:
        board_reset();
        break;

    case SP_MAGIC_FACTORY_RESET:
        user_config_reset();
        break;

    case SP_MAGIC_RECALIBRATE:
        switch_recalibrate();
        break;

    default:
        break;
    }
}

void layout_process_events(void) {
    for (uint32_t i = 0; i < num_pending_events; i++) {
        const layout_event_t *event = &pending_events[(pending_event_head + i) &
                                                      (MAX_PENDING_EVENTS - 1)];

        if (IS_SW_PRESSED(event->sw_state, last_sw_states[event->index])) {
            uint16_t keycode = get_keycode(event->index);
            active_keycodes[event->index] = keycode;

            if (IS_HID_KEYCODE(keycode)) {
                hid_add_keycode(keycode);
            } else if (IS_MODS_KEYCODE(keycode)) {
                hid_add_keycode(SP_MODS_GET_KEY(keycode));
                hid_add_modifier(SP_MODS_GET_MODS(keycode));
            } else if (IS_MOD_TAP_KEYCODE(keycode)) {
                layout_tap_hold_event_push(event->index, keycode);
            } else if (IS_LAYER_TAP_KEYCODE(keycode)) {
                layout_tap_hold_event_push(event->index, keycode);
            } else if (IS_LAYER_MOD_KEYCODE(keycode)) {
                hid_add_modifier(SP_LAYER_MOD_GET_MODS(keycode));
                layer_on(SP_LAYER_MOD_GET_LAYER(keycode));
            } else if (IS_LAYER_TO_KEYCODE(keycode)) {
                layer_goto(SP_LAYER_TO_GET_LAYER(keycode));
            } else if (IS_LAYER_MO_KEYCODE(keycode)) {
                layer_on(SP_LAYER_MO_GET_LAYER(keycode));
            } else if (IS_LAYER_DEF_KEYCODE(keycode)) {
                set_default_layer(SP_LAYER_DEF_GET_LAYER(keycode));
            } else if (IS_LAYER_TOGGLE_KEYCODE(keycode)) {
                layer_toggle(SP_LAYER_TOGGLE_GET_LAYER(keycode));
            } else if (IS_PROFILE_TO_KEYCODE(keycode)) {
                user_config_set_current_profile(
                    SP_PROFILE_TO_GET_PROFILE(keycode));
            } else if (IS_DKS_KEYCODE(keycode)) {
                layout_process_dks(i, DKS_GET_CONFIG(keycode), event->sw_state,
                                   last_sw_states[event->index]);
            } else if (IS_MAGIC_KEYCODE(keycode)) {
                layout_process_magic_keycode(keycode);
            }
        } else if (IS_SW_RELEASED(event->sw_state,
                                  last_sw_states[event->index])) {
            uint16_t keycode = active_keycodes[event->index];
            active_keycodes[event->index] = KC_NO;

            if (IS_HID_KEYCODE(keycode)) {
                hid_remove_keycode(keycode);
            } else if (IS_MODS_KEYCODE(keycode)) {
                hid_remove_keycode(SP_MODS_GET_KEY(keycode));
                hid_remove_modifier(SP_MODS_GET_MODS(keycode));
            } else if (IS_MOD_TAP_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_LAYER_TAP_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_LAYER_MOD_KEYCODE(keycode)) {
                hid_remove_modifier(SP_LAYER_MOD_GET_MODS(keycode));
                layer_off(SP_LAYER_MOD_GET_LAYER(keycode));
            } else if (IS_LAYER_TO_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_LAYER_MO_KEYCODE(keycode)) {
                layer_off(SP_LAYER_MO_GET_LAYER(keycode));
            } else if (IS_LAYER_DEF_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_LAYER_TOGGLE_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_PROFILE_TO_KEYCODE(keycode)) {
                // Nothing to do
            } else if (IS_DKS_KEYCODE(keycode)) {
                layout_process_dks(i, DKS_GET_CONFIG(keycode), event->sw_state,
                                   last_sw_states[event->index]);
            } else if (IS_MAGIC_KEYCODE(keycode)) {
                // Nothing to do
            }
        } else {
            uint16_t keycode = active_keycodes[event->index];

            if (IS_DKS_KEYCODE(keycode))
                layout_process_dks(i, DKS_GET_CONFIG(keycode), event->sw_state,
                                   last_sw_states[event->index]);
        }

        // Update the last switch state
        last_sw_states[event->index] = event->sw_state;
    }

    // Clear the event queue
    pending_event_head = 0;
    num_pending_events = 0;
}

//--------------------------------------------------------------------+
// Tap-Hold APIs
//--------------------------------------------------------------------+

static uint32_t num_tap_hold_events;
static layout_tap_hold_event_t tap_hold_events[MAX_TAP_HOLD_EVENTS];

void layout_tap_hold_event_init(void) {
    num_tap_hold_events = 0;
    memset(tap_hold_events, 0, sizeof(tap_hold_events));
}

void layout_tap_hold_event_push(uint16_t index, uint16_t keycode) {
    if (num_tap_hold_events == MAX_TAP_HOLD_EVENTS)
        return;

    tap_hold_events[num_tap_hold_events].index = index;
    tap_hold_events[num_tap_hold_events].keycode = keycode;
    tap_hold_events[num_tap_hold_events].since = timer_read();
    num_tap_hold_events++;
}

void layout_process_tap_action(uint16_t index, uint16_t keycode) {
    uint16_t tap_keycode;

    if (IS_HID_KEYCODE(keycode))
        tap_keycode = keycode;
    else if (IS_MOD_TAP_KEYCODE(keycode))
        tap_keycode = SP_MOD_TAP_GET_KEY(keycode);
    else if (IS_LAYER_TAP_KEYCODE(keycode))
        tap_keycode = SP_LAYER_TAP_GET_KEY(keycode);
    else
        // Tap action is not supported
        return;

    hid_add_keycode(tap_keycode);
    // Remove the tap key after the next report
    layout_post_hid_report_event_push(index, tap_keycode,
                                      POST_HID_REPORT_ACTION_REMOVE);
}

void layout_process_hold_action(uint16_t index, uint16_t keycode) {
    if (IS_MOD_TAP_KEYCODE(keycode)) {
        // Same behavior as a modifier-mask empty keycode
        active_keycodes[index] = SP_MOD_TAP_TO_MODS(keycode);
        hid_add_modifier(SP_MOD_TAP_GET_MODS(keycode));
    } else if (IS_LAYER_TAP_KEYCODE(keycode)) {
        // Same behavior as a layer-momentary keycode
        active_keycodes[index] = SP_LAYER_TAP_TO_MO(keycode);
        layer_on(SP_LAYER_TAP_GET_LAYER(keycode));
    }
}

void layout_tick_event(void) {
    // Bitmap to check if a key has a pending release event
    static uint32_t has_release_events[(NUM_KEYS + 31) / 32];

    const uint8_t current_profile = user_config_current_profile();
    const uint8_t tap_hold = user_config_tap_hold();

    if (num_tap_hold_events == 0)
        // No tap-hold events to process
        return;

    // Clear the bitmap
    memset(has_release_events, 0, sizeof(has_release_events));

    bool has_press_events = false;
    for (uint32_t i = 0; i < num_pending_events; i++) {
        const layout_event_t *event = &pending_events[(pending_event_head + i) &
                                                      (MAX_PENDING_EVENTS - 1)];

        if (IS_SW_PRESSED(event->sw_state, last_sw_states[event->index]))
            has_press_events = true;
        else if (IS_SW_RELEASED(event->sw_state, last_sw_states[event->index]))
            has_release_events[event->index >> 5] |= 1
                                                     << (event->index & 0x001F);
    }

    uint32_t free_stack_slot = 0;
    for (uint32_t i = 0; i < num_tap_hold_events; i++) {
        layout_tap_hold_event_t *event = &tap_hold_events[i];
        const uint16_t tapping_term =
            user_config_key_config(current_profile, event->index)->tapping_term;

        bool should_pop = false;
        if (tap_hold == TAP_HOLD_HOLD_ON_OTHER_KEY_PRESS && has_press_events) {
            layout_process_hold_action(event->index, event->keycode);
            should_pop = true;
        } else {
            const uint32_t elapsed = timer_elapsed(event->since);
            const bool has_release_event =
                has_release_events[event->index >> 5] &
                (1 << (event->index & 0x001F));

            if (has_release_event && elapsed < tapping_term) {
                // Perform the tap action
                layout_process_tap_action(event->index, event->keycode);
                should_pop = true;
            } else if (!has_release_event && elapsed >= tapping_term) {
                // Perform the hold action
                layout_process_hold_action(event->index, event->keycode);
                should_pop = true;
            }
        }

        if (!should_pop) {
            if (free_stack_slot != i)
                tap_hold_events[free_stack_slot] = *event;
            free_stack_slot++;
        }
    }

    num_tap_hold_events = free_stack_slot;
}

//--------------------------------------------------------------------+
// Post-HID-Report APIs
//--------------------------------------------------------------------+

static uint32_t post_hid_report_event_head, num_post_hid_report_events;
static layout_post_hid_report_event_t
    post_hid_report_events[MAX_POST_HID_REPORT_EVENTS];

void layout_post_hid_report_event_init(void) {
    post_hid_report_event_head = 0;
    num_post_hid_report_events = 0;
    memset(post_hid_report_events, 0, sizeof(post_hid_report_events));
}

void layout_post_hid_report_event_push(uint16_t index, uint8_t keycode,
                                       uint8_t action) {
    if (num_post_hid_report_events == MAX_POST_HID_REPORT_EVENTS) {
        // If the event queue is full, we should execute the oldest event now.
        layout_process_post_hid_report_event(
            &post_hid_report_events[post_hid_report_event_head]);
        post_hid_report_event_head =
            (post_hid_report_event_head + 1) & (MAX_POST_HID_REPORT_EVENTS - 1);
    }

    const uint32_t post_hid_report_event_tail =
        (post_hid_report_event_head + num_post_hid_report_events) &
        (MAX_POST_HID_REPORT_EVENTS - 1);
    post_hid_report_events[post_hid_report_event_tail].index = index;
    post_hid_report_events[post_hid_report_event_tail].keycode = keycode;
    post_hid_report_events[post_hid_report_event_tail].action = action;
    num_post_hid_report_events++;
}

void layout_process_post_hid_report_event(
    const layout_post_hid_report_event_t *event) {
    switch (event->action) {
    case POST_HID_REPORT_ACTION_ADD:
        hid_add_keycode(event->keycode);
        break;

    case POST_HID_REPORT_ACTION_REMOVE:
        hid_remove_keycode(event->keycode);
        break;

    case POST_HID_REPORT_ACTION_TAP:
        layout_process_tap_action(event->index, event->keycode);
        break;

    default:
        break;
    }
}

void layout_process_post_hid_report_events(void) {
    for (uint32_t i = 0; i < num_post_hid_report_events; i++)
        layout_process_post_hid_report_event(
            &post_hid_report_events[(post_hid_report_event_head + i) &
                                    (MAX_PENDING_EVENTS - 1)]);

    // Clear the event queue
    post_hid_report_event_head = 0;
    num_post_hid_report_events = 0;
}
