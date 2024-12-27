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

#include "eeprom.h"

#if defined(EEPROM_WEAR_LEVELING)

// Wear leveling algorithm adapted from QMK Firmware with CRC32 checksum

#include "stm32f4xx_hal.h"

//--------------------------------------------------------------------+
// Wear Leveling Definitions
//--------------------------------------------------------------------+

// STM32F446RE embedded flash memory
#define FLASH_SIZE 524288
#define NUM_FLASH_SECTORS 8

// We assume that the MCU power supply voltage is 3.3V.
#define VOLTAGE_RANGE FLASH_VOLTAGE_RANGE_3
#define FLASH_PROGRAM_TYPE FLASH_TYPEPROGRAM_WORD
#define FLASH_WRITE_SIZE (1 << FLASH_PROGRAM_TYPE)

_Static_assert(EEPROM_BYTES % FLASH_WRITE_SIZE == 0,
               "EEPROM size must be a multiple of 4");

#define EEPROM_BACKING_STORE_SIZE (EEPROM_BYTES << 1)

_Static_assert(EEPROM_BACKING_STORE_SIZE <= FLASH_SIZE,
               "Not enough flash memory for EEPROM");

#define MAX_BYTES_PER_LOG_ENTRY 5

//--------------------------------------------------------------------+
// Wear Leveling Types
//--------------------------------------------------------------------+

// Wear leveling status
enum {
    WEAR_LEVELING_FAILED = 0,
    WEAR_LEVELING_SUCCESS,
    WEAR_LEVELING_CONSOLIDATED,
};

typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t u8[8];
} eeprom_write_log_t;

//--------------------------------------------------------------------+
// Wear Leveling Macros
//--------------------------------------------------------------------+

#define LOG_ENTRY_GET_LENGTH(entry) (((entry).u8[0] >> 2) & 0x07)
#define LOG_ENTRY_GET_ADDRESS(entry) (((entry).u32[0] >> 5) & 0x0007FFFF)
#define LOG_ENTRY_MAKE(addr, len)                                              \
    (eeprom_write_log_t) {                                                     \
        .u32[0] = (((addr) & 0x0007FFFF) << 5) | ((uint32_t)(len) << 2),       \
    }

#endif