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

//--------------------------------------------------------------------+
// USB HID APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the HID module
 *
 * @return none
 */
void hid_init(void);

/**
 * @brief Add the keycode to the appropriate HID report
 *
 * @param keycode The keycode to add
 *
 * @return none
 */
void hid_add_keycode(uint16_t keycode);

/**
 * @brief Remove the keycode from the appropriate HID report
 *
 * @param keycode The keycode to remove
 *
 * @return none
 */
void hid_remove_keycode(uint16_t keycode);

/**
 * @brief Add the modifier to the HID report
 *
 * @param modifier The modifier to add
 *
 * @return none
 */
void hid_add_modifier(uint8_t modifier);

/**
 * @brief Remove the modifier from the HID report
 *
 * @param modifier The modifier to remove
 *
 * @return none
 */
void hid_remove_modifier(uint8_t modifier);

/**
 * @brief Start the HID report sending chain
 *
 * @return true if the report was sent successfully, false otherwise
 */
bool hid_send_reports(void);
