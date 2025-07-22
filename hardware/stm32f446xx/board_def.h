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
// Bootloader Configuration
//--------------------------------------------------------------------+

#define BOOTLOADER_ADDR 0x1FFF0000
#define BOOTLOADER_MAGIC 0xDEADBEEF

//--------------------------------------------------------------------+
// Flash Configuration
//--------------------------------------------------------------------+

// Flash size in bytes
#define FLASH_SIZE (512 * 1024)
// Number of flash sectors/pages
#define FLASH_NUM_SECTORS 8
// Flash value when erased
#define FLASH_EMPTY_VAL 0xFFFFFFFF

//--------------------------------------------------------------------+
// ADC Configuration
//--------------------------------------------------------------------+

// Number of ADC channels
#define ADC_NUM_CHANNELS 16

#if !defined(ADC_RESOLUTION)
// ADC resolution in bits
#define ADC_RESOLUTION ADC_RESOLUTION_12B
#endif

#if ADC_RESOLUTION == ADC_RESOLUTION_12B
// Maximum ADC value
#define ADC_MAX_VALUE 4095
#elif ADC_RESOLUTION == ADC_RESOLUTION_10B
// Maximum ADC value
#define ADC_MAX_VALUE 1023
#elif ADC_RESOLUTION == ADC_RESOLUTION_8B
// Maximum ADC value
#define ADC_MAX_VALUE 255
#elif ADC_RESOLUTION == ADC_RESOLUTION_6B
// Maximum ADC value
#define ADC_MAX_VALUE 63
#else
#error "Invalid ADC resolution"
#endif

#if !defined(ADC_NUM_SAMPLE_CYCLES)
// Number of sample cycles for each ADC conversion
#define ADC_NUM_SAMPLE_CYCLES ADC_SAMPLETIME_3CYCLES
#endif
