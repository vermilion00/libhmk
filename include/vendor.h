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
// Vendor Configuration
//--------------------------------------------------------------------+

#if defined(VENDOR_WEBUSB_URL) && !defined(VENDOR_WEBUSB_URL_SCHEME)
// URL scheme for WebUSB landing page URL (0: http, 1: https)
#define VENDOR_WEBUSB_URL_SCHEME 1
#endif

#if !defined(VENDOR_BUFFER_SIZE)
// Buffer for receiving vendor data. This should be large enough to hold the
// largest data that can be received from the host.
#define VENDOR_BUFFER_SIZE 4096
#endif

//--------------------------------------------------------------------+
// Vendor Requests
//--------------------------------------------------------------------+

enum {
  // For requesting WebUSB URL
  VENDOR_REQUEST_WEBUSB = 0,
  // For requesting Microsoft OS 2.0 descriptor
  VENDOR_REQUEST_MICROSOFT,
  VENDOR_REQUEST_COUNT,
};

//--------------------------------------------------------------------+
// Class Requests
//--------------------------------------------------------------------+

// WebUSB class request
typedef enum {
  CLASS_REQUEST_FIRMWARE_VERSION = 0,
  CLASS_REQUEST_REBOOT,
  CLASS_REQUEST_BOOTLOADER,
  CLASS_REQUEST_FACTORY_RESET,
  CLASS_REQUEST_RECALIBRATE,
  CLASS_REQUEST_KEY_INFO,
  CLASS_REQUEST_GET_CALIBRATION,
  CLASS_REQUEST_SET_CALIBRATION,
  CLASS_REQUEST_GET_PROFILE,
  CLASS_REQUEST_LOG,
  // Requests below use `wValue` to specify the profile number
  CLASS_REQUEST_GET_KEYMAP = 128,
  CLASS_REQUEST_SET_KEYMAP,
  CLASS_REQUEST_GET_ACTUATION_MAP,
  CLASS_REQUEST_SET_ACTUATION_MAP,
  CLASS_REQUEST_GET_ADVANCED_KEYS,
  CLASS_REQUEST_SET_ADVANCED_KEYS,
} class_request_t;

// `CLASS_REQUEST_DEBUG` payload for each key
typedef struct __attribute__((packed)) {
  uint16_t adc_value;
  uint8_t distance;
} class_res_key_info_t;
