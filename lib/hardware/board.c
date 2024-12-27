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

#include "board.h"

#include <stdio.h>

#include "stm32f4xx_hal.h"

uint32_t board_serial_number(uint16_t *buffer) {
    char serial_str[24 + 1];

    // Use the 96-bit unique ID as the serial number
    sprintf(serial_str, "%08lX%08lX%08lX", HAL_GetUIDw2(), HAL_GetUIDw1(),
            HAL_GetUIDw0());

    // Convert ASCII string into UTF-16-LE
    for (size_t i = 0; i < 24; i++)
        buffer[i] = serial_str[i];

    return 24;
}

void board_reset(void) { NVIC_SystemReset(); }

#if defined(ENABLE_BOOTLOADER)
// This variable is defined in the linker script. Its address is at the end of
// the RAM.
// WARNING: Make sure to reserve 4 bytes at the end of the RAM in the linker
// script or the bootloader flag could be overwritten.
extern const uint32_t _estack;

void board_enter_bootloader(void) {
    uint32_t *bootloader_flag = (uint32_t *)&_estack;

    *bootloader_flag = BOOTLOADER_FLAG;
    // Soft reset will not clear the RAM, so the bootloader flag will be present
    // in the next boot.
    board_reset();
}
#endif
