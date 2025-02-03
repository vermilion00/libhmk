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

#include "common.h"

//--------------------------------------------------------------------+
// Flash API
//--------------------------------------------------------------------+

/**
 * @brief Initialize the flash driver
 *
 * @return None
 */
void flash_init(void);

/**
 * @brief Get the size of a flash sector
 *
 * @param sector The sector number
 *
 * @return The size of the sector in bytes, or 0 if invalid
 */
uint32_t flash_sector_size(uint32_t sector);

/**
 * @brief Erase a flash sector
 *
 * @param sector The sector number
 *
 * @return true if successful, false otherwise
 */
bool flash_erase(uint32_t sector);

/**
 * @brief Write data to flash
 *
 * @param addr The address to write to
 * @param buffer The data to write
 * @param len The length of the data in words (4 bytes)
 *
 * @return true if successful, false otherwise
 */
bool flash_read(uint32_t addr, void *buf, uint32_t len);

/**
 * @brief Read data from flash
 *
 * @param addr The address to read from
 * @param buffer The buffer to read into
 * @param len The length of the data in words (4 bytes)
 *
 * @return true if successful, false otherwise
 */
bool flash_write(uint32_t addr, const void *buf, uint32_t len);
