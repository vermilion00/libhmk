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

#if defined(DEBUG)

#include <stdint.h>

//--------------------------------------------------------------------+
// Debug Macros
//--------------------------------------------------------------------+

#define DEBUG_COUNTER_DIFF(a, b) ((uint32_t)((a) - (b)))

//--------------------------------------------------------------------+
// Debug APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the debug module
 *
 * @return none
 */
void debug_init(void);

/**
 * @brief Get the current counter value
 *
 * WARNING: The counter value increases at a rate of 1 cycle per clock cycle. It
 * may overflow in a short amount of time.
 *
 * @return Counter value
 */
uint32_t debug_get_counter(void);

#endif
