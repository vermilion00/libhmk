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
// Timer Macros
//--------------------------------------------------------------------+

// Timer difference with overflow handling
#define TIMER_DIFF(a, b) ((uint32_t)((a) - (b)))

//--------------------------------------------------------------------+
// Timer APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the timer
 *
 * @return none
 */
void timer_init(void);

/**
 * @brief Read the current timer value in milliseconds
 *
 * @return Current timer value
 */
uint32_t timer_read(void);

/**
 * @brief Calculate the elapsed time in milliseconds
 *
 * @param last Last timer value
 *
 * @return Elapsed time in milliseconds
 */
uint32_t timer_elapsed(uint32_t last);

/**
 * @brief Blocking delay for a specified number of milliseconds
 *
 * @param ms Delay in milliseconds
 *
 * @return none
 */
void timer_delay(uint32_t ms);
