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

#include "timer.h"

#include "stm32f4xx_hal.h"

void timer_init(void) {
    // Nothing to do here
}

uint32_t timer_read(void) { return HAL_GetTick(); }

uint32_t timer_elapsed(uint32_t last) { return TIMER_DIFF(timer_read(), last); }

void timer_delay(uint32_t ms) {
    uint32_t start = timer_read();

    while (timer_elapsed(start) < ms)
        ;
}