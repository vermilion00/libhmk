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

#include "vendor.h"

#include "tusb.h"
#include "usb_descriptors.h"

#if defined(VENDOR_WEBUSB_URL)
#define WEBUSB_DESC_LEN (3 + sizeof(VENDOR_WEBUSB_URL) - 1)

// WebUSB descriptor with landing page URL
static const tusb_desc_webusb_url_t desc_url = {
    .bLength = WEBUSB_DESC_LEN,
    .bDescriptorType = TUSB_DESC_STRING,
    .bScheme = VENDOR_WEBUSB_URL_SCHEME,
    .url = VENDOR_WEBUSB_URL,
};
#endif

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                tusb_control_request_t const *request) {
  switch (request->bmRequestType_bit.type) {
  case TUSB_REQ_TYPE_VENDOR:
    switch (request->bRequest) {
#if defined(VENDOR_WEBUSB_URL)
    case VENDOR_REQUEST_WEBUSB:
      if (stage == CONTROL_STAGE_SETUP)
        return tud_control_xfer(rhport, request, (void *)&desc_url,
                                WEBUSB_DESC_LEN);
      // Nothing to do for other stages
      return true;
#endif

    case VENDOR_REQUEST_MICROSOFT:
      if (request->wIndex == 0x07 && stage == CONTROL_STAGE_SETUP)
        return tud_control_xfer(rhport, request, (void *)desc_ms_os_20,
                                MS_OS_20_DESC_LEN);
      // Nothing to do for other stages
      return true;

    default:
      break;
    }

  default:
    break;
  }

  // Stall unknown request
  return false;
}
