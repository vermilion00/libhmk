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

//--------------------------------------------------------------------+
// Common Headers
//--------------------------------------------------------------------+

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board.h"

//--------------------------------------------------------------------+
// Common Macros
//--------------------------------------------------------------------+

#define M_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define M_MIN(a, b) ((a) < (b) ? (a) : (b))
#define M_MAX(a, b) ((a) > (b) ? (a) : (b))
#define M_DIV_CEIL(n, d) (((n) + (d) - 1) / (d))
#define M_BIT(n) (1UL << (n))
#define M_IS_POWER_OF_TWO(n) (((n) != 0) && (((n) & ((n) - 1)) == 0))
