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

#include "class/hid/hid.h"
#include "common.h"

//--------------------------------------------------------------------+
// USB Configuration
//--------------------------------------------------------------------+

#if !defined(USB_MANUFACTURER_NAME)
#define USB_MANUFACTURER_NAME "Unknown"
#endif

#if !defined(USB_PRODUCT_NAME)
#define USB_PRODUCT_NAME "Keyboard"
#endif

#if !defined(USB_VENDOR_ITF_NAME)
// Display name when the browser detects the device
#define USB_VENDOR_ITF_NAME USB_PRODUCT_NAME
#endif

#if !defined(USB_VENDOR_ID)
#define USB_VENDOR_ID 0xCAFE
#endif

#if !defined(USB_PRODUCT_ID)
#define USB_PRODUCT_ID 0x1234
#endif

//--------------------------------------------------------------------+
// USB Descriptors
//--------------------------------------------------------------------+

// Length of Microsoft OS 2.0 descriptor in bytes
#define MS_OS_20_DESC_LEN 0x01F2

// Microsoft OS 2.0 descriptor
extern const uint8_t desc_ms_os_20[];

enum {
  STR_ID_LANGID = 0,
  STR_ID_MANUFACTURER,
  STR_ID_PRODUCT,
  STR_ID_SERIAL,
  STR_ID_VENDOR,
  STR_ID_COUNT,
};

enum {
  // Separate interface for keyboard to support boot protocol
  USB_ITF_KEYBOARD = 0,
  USB_ITF_HID,
  USB_ITF_VENDOR,
  USB_ITF_COUNT,
};

enum {
  REPORT_ID_SYSTEM_CONTROL = 1,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_MOUSE,
  REPORT_ID_COUNT,
};

//--------------------------------------------------------------------+
// NKRO HID Report
//--------------------------------------------------------------------+

// 20 * 8 = 160 bits for HID keyboard keycodes up to KC_LANGUAGE_2
#define NUM_NKRO_BYTES 20

// NKRO report with 6-KRO fallback
// https://geekhack.org/index.php?topic=13162
// https://www.devever.net/~hl/usbnkro.
typedef struct __attribute__((packed)) {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keycodes[6];
  uint8_t bitmap[NUM_NKRO_BYTES];
} hid_nkro_kb_report_t;

// HID Report Descriptor
#define HID_REPORT_DESC_NKRO_KB(...)                                           \
  HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                                      \
      HID_USAGE(HID_USAGE_DESKTOP_KEYBOARD),                                   \
      HID_COLLECTION(HID_COLLECTION_APPLICATION),                              \
                                                                               \
      /* Report ID if any */                                                   \
      __VA_ARGS__                                                              \
                                                                               \
      /* 8 bits for modifiers  */                                              \
      HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD),                                 \
      HID_USAGE_MIN(224), HID_USAGE_MAX(231), HID_LOGICAL_MIN(0),              \
      HID_LOGICAL_MAX(1), HID_REPORT_COUNT(8), HID_REPORT_SIZE(1),             \
      HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                       \
                                                                               \
      /* 8 bits reserved */                                                    \
      HID_REPORT_COUNT(1), HID_REPORT_SIZE(8), HID_INPUT(HID_CONSTANT),        \
                                                                               \
      /* 5-bit LED indicator output */                                         \
      HID_USAGE_PAGE(HID_USAGE_PAGE_LED), HID_USAGE_MIN(1), HID_USAGE_MAX(5),  \
      HID_REPORT_COUNT(5), HID_REPORT_SIZE(1),                                 \
      HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                      \
                                                                               \
      /* LED padding */                                                        \
      HID_REPORT_COUNT(1), HID_REPORT_SIZE(3), HID_OUTPUT(HID_CONSTANT),       \
                                                                               \
      /* 6-byte padding for compatibility with 6-KRO HID report */             \
      HID_REPORT_COUNT(48), HID_REPORT_SIZE(1), HID_INPUT(HID_CONSTANT),       \
                                                                               \
      /* NKRO bitmap */                                                        \
      HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD), HID_USAGE_MIN(0),               \
      HID_USAGE_MAX(NUM_NKRO_BYTES * 8 - 1), HID_LOGICAL_MIN(0),               \
      HID_LOGICAL_MAX(1), HID_REPORT_COUNT(NUM_NKRO_BYTES * 8),                \
      HID_REPORT_SIZE(1), HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),   \
      HID_COLLECTION_END
