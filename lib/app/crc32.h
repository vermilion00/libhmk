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

//--------------------------------------------------------------------+
// CRC32 Definitions
//--------------------------------------------------------------------+

#define CRC32_POLYNOMIAL 0xEDB88320

//--------------------------------------------------------------------+
// CRC32 APIs
//--------------------------------------------------------------------+

/**
 * @brief Calculate the CRC32 of a buffer using bitwise algorithm
 *
 * @param crc The initial CRC32 value
 * @param data The buffer to calculate the CRC32 of
 * @param len The length of the buffer
 *
 * @return The CRC32 of the buffer
 */
uint32_t crc32_bitwise(uint32_t crc, const uint8_t *data, uint32_t len);
