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

#include "config.h"

// We support STM32F4xx family for now.
#define CFG_TUSB_MCU OPT_MCU_STM32F4

#if defined(USE_USB_OTG_HS_PORT)
// If USB D+/D- is connected to PB14/PB15
#define CFG_TUSB_RHPORT1_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define BOARD_TUD_RHPORT 1
#else
// If USB D+/D- is connected to PA11/PA12
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define BOARD_TUD_RHPORT 0
#endif

// Default maximum speed to the on-chip full-speed PHY
#define BOARD_TUD_MAX_SPEED OPT_MODE_DEFAULT_SPEED
#define CFG_TUD_MAX_SPEED OPT_MODE_DEFAULT_SPEED

#define CFG_TUSB_OS OPT_OS_NONE
#define CFG_TUSB_DEBUG 0

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))

// Enable USB device stack
#define CFG_TUD_ENABLED 1
#define CFG_TUD_ENDPOINT0_SIZE 64

// USB device classes
#define CFG_TUD_HID 1
#define CFG_TUD_VENDOR 1

// HID buffer size must be strictly larger than the report size.
#define CFG_TUD_HID_EP_BUFSIZE 64
