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

#include "advanced_keys.h"

#include "deferred_actions.h"
#include "eeconfig.h"
#include "hardware/hardware.h"
#include "keycodes.h"
#include "layout.h"

static advanced_key_state_t ak_states[NUM_ADVANCED_KEYS];

static void advanced_key_tap_hold(const advanced_key_event_t *event) {
  static deferred_action_t tap_release = {0};

  const tap_hold_t *tap_hold =
      &CURRENT_PROFILE.advanced_keys[event->ak_index].tap_hold;
  ak_state_tap_hold_t *state = &ak_states[event->ak_index].tap_hold;

  switch (event->type) {
  case AK_EVENT_TYPE_PRESS:
    state->since = timer_read();
    state->stage = TAP_HOLD_STAGE_TAP;
    break;

  case AK_EVENT_TYPE_RELEASE:
    if (state->stage == TAP_HOLD_STAGE_TAP) {
      tap_release = (deferred_action_t){
          .type = DEFERRED_ACTION_TYPE_RELEASE,
          .key = event->key,
          .keycode = tap_hold->tap_keycode,
      };
      if (deferred_action_push(&tap_release))
        // We only perform the tap action if the release action was
        // successfully.
        layout_ll_press(event->key, tap_hold->tap_keycode);
    } else if (state->stage == TAP_HOLD_STAGE_HOLD)
      layout_ll_release(event->key, tap_hold->hold_keycode);
    state->stage = TAP_HOLD_STAGE_NONE;
    break;

  default:
    break;
  }
}

static void advanced_key_toggle(const advanced_key_event_t *event) {
  const toggle_t *toggle =
      &CURRENT_PROFILE.advanced_keys[event->ak_index].toggle;
  ak_state_toggle_t *state = &ak_states[event->ak_index].toggle;

  switch (event->type) {
  case AK_EVENT_TYPE_PRESS:
    layout_ll_press(event->key, toggle->keycode);
    state->is_toggled = !state->is_toggled;
    if (state->is_toggled) {
      state->since = timer_read();
      state->stage = TOGGLE_STAGE_TOGGLE;
    } else
      // If the key is toggled off, we use the normal key behavior.
      state->stage = TOGGLE_STAGE_NORMAL;
    break;

  case AK_EVENT_TYPE_RELEASE:
    if (!state->is_toggled)
      layout_ll_release(event->key, toggle->keycode);
    state->stage = TOGGLE_STAGE_NONE;
    break;

  default:
    break;
  }
}

void advanced_key_init(void) {}

void advanced_key_clear(void) {
  // Release any keys that are currently pressed
  for (uint32_t i = 0; i < NUM_ADVANCED_KEYS; i++) {
    const advanced_key_t *ak = &CURRENT_PROFILE.advanced_keys[i];
    const advanced_key_state_t *state = &ak_states[i];

    switch (ak->type) {
    case AK_TYPE_TAP_HOLD:
      if (state->tap_hold.stage == TAP_HOLD_STAGE_HOLD)
        layout_ll_release(ak->key, ak->tap_hold.hold_keycode);
      break;

    case AK_TYPE_TOGGLE:
      if (state->toggle.stage != TOGGLE_STAGE_NONE || state->toggle.is_toggled)
        layout_ll_release(ak->key, ak->toggle.keycode);
      break;

    default:
      break;
    }
  }
  // Clear the advanced key states
  memset(ak_states, 0, sizeof(ak_states));
}

void advanced_key_process(const advanced_key_event_t *event) {
  if (event->type == AK_EVENT_TYPE_NONE || event->ak_index >= NUM_ADVANCED_KEYS)
    return;

  switch (CURRENT_PROFILE.advanced_keys[event->ak_index].type) {
  case AK_TYPE_TAP_HOLD:
    advanced_key_tap_hold(event);
    break;

  case AK_TYPE_TOGGLE:
    advanced_key_toggle(event);
    break;

  default:
    break;
  }
}

void advanced_key_tick(bool has_non_tap_hold_press) {
  for (uint32_t i = 0; i < NUM_ADVANCED_KEYS; i++) {
    const advanced_key_t *ak = &CURRENT_PROFILE.advanced_keys[i];
    advanced_key_state_t *state = &ak_states[i];

    switch (ak->type) {
    case AK_TYPE_TAP_HOLD:
      if (state->tap_hold.stage == TAP_HOLD_STAGE_TAP &&
          // If the hold key is a modifier, immediately register it when another
          // non-Tap-Hold key is pressed.
          ((has_non_tap_hold_press &
            IS_MODIFIER_KEYCODE(ak->tap_hold.hold_keycode)) ||
           // Otherwise, the key must be held for the tapping term.
           timer_elapsed(state->tap_hold.since) >= ak->tap_hold.tapping_term)) {
        layout_ll_press(ak->key, ak->tap_hold.hold_keycode);
        state->tap_hold.stage = TAP_HOLD_STAGE_HOLD;
      }
      break;

    case AK_TYPE_TOGGLE:
      if (state->toggle.stage == TOGGLE_STAGE_TOGGLE &&
          timer_elapsed(state->toggle.since) >= ak->toggle.tapping_term) {
        // If the key is held for more than the tapping term, switch to the
        // normal key behavior.
        state->toggle.stage = TOGGLE_STAGE_NORMAL;
        // Always toggle the key off when in normal behavior
        state->toggle.is_toggled = false;
      }
      break;

    default:
      break;
    }
  }
}
