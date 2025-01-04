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

#pragma once

#include "layout.h"

//--------------------------------------------------------------------+
// Layer APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the layout layer
 *
 * @return none
 */
void layout_layer_init(void);

/**
 * @brief Get the current layer
 *
 * @return The current layer
 */
uint8_t get_current_layer(void);

/**
 * @brief Activate a layer
 *
 * @param layer_num The layer to activate
 *
 * @return none
 */
void layer_on(uint8_t layer_num);

/**
 * @brief Deactivate a layer
 *
 * @param layer_num The layer to deactivate
 *
 * @return none
 */
void layer_off(uint8_t layer_num);

/**
 * @brief Toggle a layer
 *
 * @param layer_num The layer to toggle
 *
 * @return none
 */
void layer_toggle(uint8_t layer_num);

/**
 * @brief Go to a layer
 *
 * @param layer_num The layer to go to
 *
 * @return none
 */
void layer_goto(uint8_t layer_num);

/**
 * @brief Set the default layer
 *
 * @param layer_num The default layer
 *
 * @return none
 */
void set_default_layer(uint8_t layer_num);

/**
 * @brief Get the keycode at the specified switch index
 *
 * @param index The index of the switch
 *
 * @return The keycode
 */
uint16_t get_keycode(uint16_t index);

//--------------------------------------------------------------------+
// Layout APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the layout event queue
 *
 * @return none
 */
void layout_event_init(void);

/**
 * @brief Push an event to the layout event queue
 *
 * If the event queue is full, the oldest event will be overwritten.
 *
 * @param index The index of the switch
 * @param sw_state The switch state
 *
 * @return none
 */
void layout_event_push(uint16_t index, uint8_t sw_state);

/**
 * @brief Process the dynamic keystroke
 *
 * This function assumes that the dynamic keystroke configuration is valid.
 *
 * @param index The index of the switch
 * @param config_num The dynamic keystroke configuration number
 * @param sw_state The current switch state
 * @param last_sw_state The last switch state
 *
 * @return none
 */
void layout_process_dks(uint16_t index, uint8_t config_num, uint8_t sw_state,
                        uint8_t last_sw_state);

/**
 * @brief Process the magic keycode
 *
 * @param keycode The magic keycode
 *
 * @return none
 */
void layout_process_magic_keycode(uint16_t keycode);

/**
 * @brief Process all the events in the event queue and clear the queue
 *
 * @return none
 */
void layout_process_events(void);

//--------------------------------------------------------------------+
// Tap-Hold APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the tap-hold event stack
 *
 * @return none
 */
void layout_tap_hold_event_init(void);

/**
 * @brief Push a tap-hold event to the event stack
 *
 * If the event stack is full, nothing will be pushed.
 *
 * @param index The index of the switch
 * @param keycode The tap-hold keycode
 *
 * @return none
 */
void layout_tap_hold_event_push(uint16_t index, uint16_t keycode);

/**
 * @brief Process the tap action of a tap-hold or HID keycode
 *
 * @param index The index of the switch
 * @param keycode The tap-hold or HID keycode
 *
 * @return none
 */
void layout_process_tap_action(uint16_t index, uint16_t keycode);

/**
 * @brief Process the hold action of a tap-hold keycode
 *
 * @param index The index of the switch
 * @param keycode The tap-hold keycode
 *
 * @return none
 */
void layout_process_hold_action(uint16_t index, uint16_t keycode);

/**
 * @brief Tick event
 *
 * This function is called before the last switch states are updated and handles
 * time-based events such as tap-hold.
 *
 * @return none
 */
void layout_tick_event(void);

//--------------------------------------------------------------------+
// Post-HID-Report APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the post-HID report event queue
 *
 * @return none
 */
void layout_post_hid_report_event_init(void);

/**
 * @brief Push an event to the post-HID report event queue
 *
 * If the event queue is full, the oldest event will be overwritten.
 *
 * @param index The index of the switch
 * @param keycode The keycode
 * @param action The action to perform
 *
 * @return none
 */
void layout_post_hid_report_event_push(uint16_t index, uint8_t keycode,
                                       uint8_t action);

/**
 * @brief Process single post-HID report event
 *
 * @param event The event to process
 *
 * @return none
 */
void layout_process_post_hid_report_event(
    const layout_post_hid_report_event_t *event);

/**
 * @brief Process all the events in the post-HID report event queue and clear
 * the queue
 *
 * @return none
 */
void layout_process_post_hid_report_events(void);
