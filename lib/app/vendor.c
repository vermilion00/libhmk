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

#include "app.h"
#include "board.h"
#include "switches.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "user_config.h"

//--------------------------------------------------------------------+
// TinyUSB Vendor Callbacks
//--------------------------------------------------------------------+

static uint8_t request_buffer[VENDOR_REQUEST_BUFFER_SIZE];

#if defined(USB_WEBUSB_URL)
// WebUSB landing page URL
const tusb_desc_webusb_url_t desc_url = {
    .bLength = 3 + sizeof(USB_WEBUSB_URL) - 1, // Total length
    .bDescriptorType = TUSB_DESC_STRING,       // WebUSB URL type
    .bScheme = USB_WEBUSB_URL_SCHEME,          // URL scheme
    .url = USB_WEBUSB_URL,                     // WebUSB URL
};
#endif

/**
 * @brief Vendor control transfer callback
 *
 * Invoked when a control transfer occurred on an interface of this class.
 * Handle Microsoft OS 2.0 descriptor request. If `USB_WEBUSB_URL` is defined,
 * handle WebUSB request as well.
 *
 * @param rhport Root hub port number
 * @param stage Transfer stage
 * (`CONTROL_STAGE_SETUP`/`CONTROL_STAGE_DATA`/`CONTROL_STAGE_ACK`)
 * @param request Control request
 *
 * @return true if the request is supported and can be processed.
 */
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                tusb_control_request_t const *request) {
    switch (request->bmRequestType_bit.type) {
    case TUSB_REQ_TYPE_VENDOR:
        switch (request->bRequest) {
#if defined(USB_WEBUSB_URL)
        case VENDOR_REQUEST_WEBUSB:
            if (stage == CONTROL_STAGE_SETUP)
                // Get landing page URL
                return tud_control_xfer(rhport, request, (void *)&desc_url,
                                        desc_url.bLength);
            // Nothing to do for DATA & ACK stages
            return true;
#endif

        case VENDOR_REQUEST_MICROSOFT:
            if (stage == CONTROL_STAGE_SETUP && request->wIndex == 0x07) {
                // Get Microsoft OS 2.0 compatible descriptor
                uint16_t total_len;
                memcpy(&total_len, desc_ms_os_20 + 8, 2);

                return tud_control_xfer(rhport, request, (void *)desc_ms_os_20,
                                        total_len);
            }
            // Nothing to do for DATA & ACK stages
            return true;

        default:
            break;
        }

        break;

    case TUSB_REQ_TYPE_CLASS:
        switch (request->bRequest) {
        case CLASS_REQUEST_PROTOCOL_VERSION:

            if (stage == CONTROL_STAGE_SETUP) {
                if (request->wLength < sizeof(class_res_protocol_version_t))
                    // Invalid response length
                    return false;

                static class_res_protocol_version_t res;
                res.protocol_version = VENDOR_CLASS_PROTOCOL_VERSION;

                return tud_control_xfer(rhport, request, &res, sizeof(res));
            }
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_FIRMWARE_VERSION:
            if (stage == CONTROL_STAGE_SETUP) {
                if (request->wLength < sizeof(class_res_firmware_version_t))
                    // Invalid response length
                    return false;

                static class_res_firmware_version_t res;
                res.firmware_version = FIRMWARE_VERSION;

                return tud_control_xfer(rhport, request, &res, sizeof(res));
            }
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_BOOTLOADER:
#if defined(ENABLE_BOOTLOADER)
            if (stage == CONTROL_STAGE_SETUP) {
                board_enter_bootloader();
                return tud_control_status(rhport, request);
            }
#endif
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_REBOOT:
            if (stage == CONTROL_STAGE_SETUP) {
                board_reset();
                return tud_control_status(rhport, request);
            }
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_FACTORY_RESET:
            if (stage == CONTROL_STAGE_SETUP) {
                user_config_reset();
                return tud_control_status(rhport, request);
            }
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_RECALIBRATE:
            if (stage == CONTROL_STAGE_SETUP) {
                switch_recalibrate();
                return tud_control_status(rhport, request);
            }
            // Nothing to do for DATA & ACK stages
            return true;

        case CLASS_REQUEST_SWITCH_DEBUG:
            if (stage == CONTROL_STAGE_SETUP) {
                if (request->wLength < sizeof(switch_state_t))
                    // Invalid response length
                    return false;

                static class_res_switch_debug_t res;
                for (uint32_t i = 0; i < NUM_KEYS; i++) {
                    res.adc_values[i] = get_switch_adc_value(i);
                    res.distances[i] = get_switch_distance(i);
                }

                return tud_control_xfer(rhport, request, &res, sizeof(res));
            }
            // Nothing to do for DATA & ACK stages
            return true;

        default:
            break;
        }
        break;

    default:
        break;
    }

    // Stall unknown request
    return false;
}
