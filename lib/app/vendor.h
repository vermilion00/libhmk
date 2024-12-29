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

#include "config.h"

//--------------------------------------------------------------------+
// USB Vendor Definitions
//--------------------------------------------------------------------+

#if defined(USB_WEBUSB_URL) && !defined(USB_WEBUSB_URL_SCHEME)
// 0: http, 1: https
// We default to https if the scheme is not specified.
#define USB_WEBUSB_URL_SCHEME 1
#endif

#if !defined(VENDOR_BUFFER_SIZE)
// The size of the vendor request buffer. Must be large enough to hold the
// largest vendor class request.
#define VENDOR_REQUEST_BUFFER_SIZE 1024
#endif

// Version of the vendor class protocol
#define VENDOR_CLASS_PROTOCOL_VERSION 0x0100

//--------------------------------------------------------------------+
// USB Vendor Class Requests
//--------------------------------------------------------------------+

enum {
    CLASS_REQUEST_PROTOCOL_VERSION = 0,
    CLASS_REQUEST_FIRMWARE_VERSION,
    CLASS_REQUEST_BOOTLOADER,
    CLASS_REQUEST_REBOOT,
    CLASS_REQUEST_FACTORY_RESET,
    CLASS_REQUEST_RECALIBRATE,
    CLASS_REQUEST_SWITCH_DEBUG,
};

enum {
    CLASS_REQUEST_INDEX_GET = 0,
    CLASS_REQUEST_INDEX_SET,
    CLASS_REQUEST_INDEX_RESET,
};

//--------------------------------------------------------------------+
// USB Vendor Class Types
//--------------------------------------------------------------------+

typedef struct __attribute__((packed)) {
    uint16_t protocol_version;
} class_res_protocol_version_t;

typedef struct __attribute__((packed)) {
    uint16_t firmware_version;
} class_res_firmware_version_t;

typedef struct __attribute__((packed)) {
    uint16_t adc_values[NUM_KEYS];
    uint8_t distances[NUM_KEYS];
} class_res_switch_debug_t;
