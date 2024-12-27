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

#include "class/hid/hid.h"
#include "config.h"

//--------------------------------------------------------------------+
// USB Device Configuration
//--------------------------------------------------------------------+

#if !defined(USB_MANUFACTURER_NAME)
#define USB_MANUFACTURER_NAME "Unknown"
#endif

#if !defined(USB_PRODUCT_NAME)
#define USB_PRODUCT_NAME "Keyboard"
#endif

#if !defined(USB_VENDOR_INTF_NAME)
// Display name when a WebUSB compatible browser detects the device
#define USB_VENDOR_INTF_NAME USB_PRODUCT_NAME
#endif

#if !defined(USB_VENDOR_ID)
#define USB_VENDOR_ID 0xCAFE
#endif

#if !defined(USB_PRODUCT_ID)
#define USB_PRODUCT_ID 0x1234
#endif

//--------------------------------------------------------------------+
// Vendor Requests
//--------------------------------------------------------------------+

enum {
    VENDOR_REQUEST_WEBUSB,    // For requesting WebUSB landing page URL
    VENDOR_REQUEST_MICROSOFT, // For requesting Microsoft OS 2.0 descriptor
};

// Microsoft OS 2.0 descriptor
extern const uint8_t desc_ms_os_20[];

//--------------------------------------------------------------------+
// HID Report IDs
//--------------------------------------------------------------------+

enum {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_SYSTEM_CONTROL,
    REPORT_ID_CONSUMER_CONTROL,
    REPORT_ID_COUNT,
};

//--------------------------------------------------------------------+
// Custom HID Report Descriptors
//--------------------------------------------------------------------+

// 24 * 8 = 192 bits. Enough bits to all keys up to KC_EXSEL.
#define NUM_NKRO_BYTES 24

// Hybrid keyboard report
// This report operates as an NKRO (N-Key Rollover) report when the host
// supports it and as a 6-KRO (6-Key Rollover) report otherwise (e.g., BIOS).
// This is achieved by appending the NKRO bitmap to the traditional 6-KRO
// report. The report descriptor specifies that the 6-key buffer serves as a
// "padding" field. A host that supports NKRO will ignore the 6-key buffer and
// parse the NKRO bitmap instead, according to the report descriptor. If the
// host does not support NKRO, it will interpret the 6-key buffer as a standard
// 6-KRO report. For more information, visit
// https://www.devever.net/~hl/usbnkro.
typedef struct __attribute__((packed)) {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keycodes[6];
    uint8_t bitmap[NUM_NKRO_BYTES];
} hid_hybrid_kb_report_t;

_Static_assert(1 + sizeof(hid_hybrid_kb_report_t) <= CFG_TUD_HID_EP_BUFSIZE,
               "Buffer size is not enough");

// Hybrid keyboard report descriptor
#define HID_REPORT_DESC_HYBRID_KB(...)                                         \
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                                    \
        HID_USAGE(HID_USAGE_DESKTOP_KEYBOARD),                                 \
        HID_COLLECTION(HID_COLLECTION_APPLICATION),                            \
                                                                               \
        /* Report ID if any */                                                 \
        __VA_ARGS__                                                            \
                                                                               \
        /* 8 bits Modifier Keys (Shift, Control, Alt) */                       \
        HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD),                               \
        HID_USAGE_MIN(224), HID_USAGE_MAX(231), HID_LOGICAL_MIN(0),            \
        HID_LOGICAL_MAX(1), HID_REPORT_COUNT(8), HID_REPORT_SIZE(1),           \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                     \
                                                                               \
        /* 8 bit reserved */                                                   \
        HID_REPORT_COUNT(1), HID_REPORT_SIZE(8), HID_INPUT(HID_CONSTANT),      \
                                                                               \
        /* Output 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | \
            NumLock */                                                         \
        HID_USAGE_PAGE(HID_USAGE_PAGE_LED), HID_USAGE_MIN(1),                  \
        HID_USAGE_MAX(5), HID_REPORT_COUNT(5), HID_REPORT_SIZE(1),             \
        HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                    \
                                                                               \
        /* led padding */                                                      \
        HID_REPORT_COUNT(1), HID_REPORT_SIZE(3), HID_OUTPUT(HID_CONSTANT),     \
                                                                               \
        /* 6-byte padding for compatibility with 6-KRO HID report */           \
        HID_REPORT_COUNT(48), HID_REPORT_SIZE(1), HID_INPUT(HID_CONSTANT),     \
                                                                               \
        /* NKRO Bitmap */                                                      \
        HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD), HID_USAGE_MIN(0),             \
        HID_USAGE_MAX(NUM_NKRO_BYTES * 8 - 1), HID_LOGICAL_MIN(0),             \
        HID_LOGICAL_MAX(1), HID_REPORT_COUNT(NUM_NKRO_BYTES * 8),              \
        HID_REPORT_SIZE(1), HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), \
        HID_COLLECTION_END
