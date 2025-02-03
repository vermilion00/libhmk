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
