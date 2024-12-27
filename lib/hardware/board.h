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

#include <stdint.h>

#include "config.h"

//--------------------------------------------------------------------+
// Board-Specific Definitions
//--------------------------------------------------------------------+

#if defined(ENABLE_BOOTLOADER)
// If the bootloader flag is found at the end of the RAM, the bootloader mode
// will be entered on the next reset.
// WARNING: This flag must be the same in the bootloader and the application
// code. 4 bytes at the end of the RAM must be reserved for this flag.
#define BOOTLOADER_FLAG 0xDEADBEEF
#endif

//--------------------------------------------------------------------+
// Board-Specific APIs
//--------------------------------------------------------------------+

/**
 * @brief Get the serial number of the device
 *
 * The serial number must be in UTF-16-LE format and has a maximum length of 32.
 * Null termination is not required.
 *
 * @param buffer Buffer to store the serial number
 *
 * @return Length of the serial number in 16-bit words
 */
uint32_t board_serial_number(uint16_t *buffer);

/**
 * @brief Software reset the device
 *
 * @return none
 */
void board_reset(void);

#if defined(ENABLE_BOOTLOADER)
/**
 * @brief Enter the bootloader
 *
 * @return none
 */
void board_enter_bootloader(void);
#endif
