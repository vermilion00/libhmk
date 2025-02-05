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
// Analog API
//--------------------------------------------------------------------+

/**
 * @brief Initialize the analog module
 *
 * This function should initialize the ADC peripheral and any other peripherals
 * needed to scan the keys. In a non-blocking implementation, this function
 * can be used to start the ADC conversion loop.
 *
 * @return None
 */
void analog_init(void);

/**
 * @brief Analog task
 *
 * This function will be called before reading the ADC values. In a blocking
 * implementation, this function can be used to start and wait for the ADC
 * conversion to complete.
 *
 * @return None
 */
void analog_task(void);

/**
 * @brief Read the raw ADC value of the specified key
 *
 * @param key Key index
 *
 * @return Raw ADC value
 */
uint16_t analog_read(uint8_t key);
