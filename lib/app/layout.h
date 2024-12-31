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

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

//--------------------------------------------------------------------+
// Layout Configuration
//--------------------------------------------------------------------+

#if !defined(MAX_PENDING_EVENTS)
// The maximum number of pending events in the event queue per matrix scan
#define MAX_PENDING_EVENTS 32
#endif

#if !defined(MAX_TAP_HOLD_EVENTS)
// The maximum number of tap-hold events in the event queue per matrix scan
#define MAX_TAP_HOLD_EVENTS 32
#endif

#if !defined(MAX_POST_HID_REPORT_EVENTS)
// The maximum number of post-HID report events in the event queue per matrix
// scan
#define MAX_POST_HID_REPORT_EVENTS 32
#endif

_Static_assert((MAX_TAP_HOLD_EVENTS & (MAX_TAP_HOLD_EVENTS - 1)) == 0,
               "MAX_TAP_HOLD_EVENTS must be a power of 2");
_Static_assert((MAX_PENDING_EVENTS & (MAX_PENDING_EVENTS - 1)) == 0,
               "MAX_PENDING_EVENTS must be a power of 2");
_Static_assert((MAX_POST_HID_REPORT_EVENTS &
                (MAX_POST_HID_REPORT_EVENTS - 1)) == 0,
               "MAX_POST_HID_REPORT_EVENTS must be a power of 2");

//--------------------------------------------------------------------+
// Layout Types
//--------------------------------------------------------------------+

typedef struct {
    // The index of the switch
    uint16_t index;
    // The switch state
    uint8_t sw_state;
} layout_event_t;

typedef struct {
    // The index of the switch
    uint16_t index;
    // The tap-hold keycode
    uint16_t keycode;
    // The time the switch was pressed
    uint32_t since;
} layout_tap_hold_event_t;

enum {
    POST_HID_REPORT_ACTION_ADD = 0,
    POST_HID_REPORT_ACTION_REMOVE,
    POST_HID_REPORT_ACTION_TAP,
};

typedef struct {
    // The index of the switch
    uint16_t index;
    // HID keycode of the event
    uint8_t keycode;
    // Action to perform
    uint8_t action;
} layout_post_hid_report_event_t;

//--------------------------------------------------------------------+
// Layout APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the layout module
 *
 * @return none
 */
void layout_init(void);

/**
 * @brief Layout task
 *
 * This function is called in the main loop and handles the main logic of the
 * keyboard.
 *
 * @return none
 */
void layout_task(void);

/**
 * @brief Task after sending the HID reports
 *
 * This function is called after sending all HID reports to the host.
 *
 * @return none
 */
void layout_post_hid_report_task(void);