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

#include "usb_descriptors.h"

#include "hardware/hardware.h"
#include "tusb.h"

// Device descriptor
static const tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VENDOR_ID,
    .idProduct = USB_PRODUCT_ID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01,
};

// HID report descriptor for keyboard interface
static const uint8_t desc_keyboard_report[] = {
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
    HID_USAGE(HID_USAGE_DESKTOP_KEYBOARD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),

    // 8 bits for modifiers
    HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD), HID_USAGE_MIN(224),
    HID_USAGE_MAX(231), HID_LOGICAL_MIN(0), HID_LOGICAL_MAX(1),
    HID_REPORT_COUNT(8), HID_REPORT_SIZE(1),
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

    // 8 bits reserved
    HID_REPORT_COUNT(1), HID_REPORT_SIZE(8), HID_INPUT(HID_CONSTANT),

    // 5-bit LED indicator output
    HID_USAGE_PAGE(HID_USAGE_PAGE_LED), HID_USAGE_MIN(1), HID_USAGE_MAX(5),
    HID_REPORT_COUNT(5), HID_REPORT_SIZE(1),
    HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

    // LED padding
    HID_REPORT_COUNT(1), HID_REPORT_SIZE(3), HID_OUTPUT(HID_CONSTANT),

    // 6-byte padding for compatibility with 6-KRO HID report
    HID_REPORT_COUNT(48), HID_REPORT_SIZE(1), HID_INPUT(HID_CONSTANT),

    // NKRO bitmap
    HID_USAGE_PAGE(HID_USAGE_PAGE_KEYBOARD), HID_USAGE_MIN(0),
    HID_USAGE_MAX(NUM_NKRO_BYTES * 8 - 1), HID_LOGICAL_MIN(0),
    HID_LOGICAL_MAX(1), HID_REPORT_COUNT(NUM_NKRO_BYTES * 8),
    HID_REPORT_SIZE(1), HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
    HID_COLLECTION_END

};

// HID report descriptor for other HID interfaces
static const uint8_t desc_hid_report[] = {
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(REPORT_ID_SYSTEM_CONTROL)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
};

// HID report descriptor for the raw HID interface
static const uint8_t desc_raw_hid_report[] = {
    HID_USAGE_PAGE_N(RAW_HID_USAGE_PAGE, 2), HID_USAGE(RAW_HID_USAGE),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),

    // Data to host
    HID_USAGE(RAW_HID_USAGE + 1), HID_LOGICAL_MIN(0), HID_LOGICAL_MAX_N(255, 2),
    HID_REPORT_COUNT(RAW_HID_EP_SIZE), HID_REPORT_SIZE(8),
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

    // Data from host
    HID_USAGE(RAW_HID_USAGE + 2), HID_LOGICAL_MIN(0), HID_LOGICAL_MAX_N(255, 2),
    HID_REPORT_COUNT(RAW_HID_EP_SIZE), HID_REPORT_SIZE(8),
    HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_NON_VOLATILE),
    HID_COLLECTION_END

};

#if defined(LOG_ENABLED)
// HID report descriptor for the log interface
static const uint8_t desc_log_report[] = {
    HID_USAGE_PAGE_N(LOG_USAGE_PAGE, 2), HID_USAGE(LOG_USAGE),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),

    // Data to host
    HID_USAGE(LOG_USAGE + 1), HID_LOGICAL_MIN(0), HID_LOGICAL_MAX_N(255, 2),
    HID_REPORT_COUNT(LOG_EP_SIZE), HID_REPORT_SIZE(8),
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), HID_COLLECTION_END

};

#define CONFIG_TOTAL_LEN                                                       \
  (TUD_CONFIG_DESC_LEN + 3 * TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
#else
#define CONFIG_TOTAL_LEN                                                       \
  (TUD_CONFIG_DESC_LEN + 2 * TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
#endif

#define EP_IN(x) ((x) | 0x80)
#define EP_OUT(x) (x)

// Configuration descriptor
static const uint8_t desc_configuration[] = {
    // Configuration descriptor header. Request maximum 500mA for the device
    TUD_CONFIG_DESCRIPTOR(1, USB_ITF_COUNT, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Keyboard interface descriptor. Request highest polling interval
    TUD_HID_DESCRIPTOR(USB_ITF_KEYBOARD, 0, HID_ITF_PROTOCOL_KEYBOARD,
                       sizeof(desc_keyboard_report), EP_IN(1),
                       CFG_TUD_HID_EP_BUFSIZE, 1),
    // HID interface descriptor. Request highest polling interval
    TUD_HID_DESCRIPTOR(USB_ITF_HID, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_hid_report), EP_IN(2),
                       CFG_TUD_HID_EP_BUFSIZE, 1),
    // Raw HID interface descriptor. Request highest polling interval
    TUD_HID_INOUT_DESCRIPTOR(USB_ITF_RAW_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(desc_raw_hid_report), EP_OUT(3), EP_IN(3),
                             RAW_HID_EP_SIZE, 1),
#if defined(LOG_ENABLED)
    // Log interface descriptor. Request highest polling interval
    TUD_HID_DESCRIPTOR(USB_ITF_LOG, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_log_report), EP_IN(4), LOG_EP_SIZE, 1),
#endif
};

_Static_assert(M_ARRAY_SIZE(desc_configuration) == CONFIG_TOTAL_LEN,
               "Invalid configuration descriptor size");

#if defined(BOARD_USB_HS)
// Device qualifier descriptor for USB HS
static const tusb_desc_device_qualifier_t desc_device_qualifier = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = 0x0200,

    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved = 0x00,
};

// Other speed configuration descriptor for USB HS. Same as the main
// configuration descriptor
static uint8_t desc_other_speed_config[CONFIG_TOTAL_LEN];
#endif

// String descriptor
const char *desc_strings[] = {
    // Supported languages (English)
    [STR_ID_LANGID] = (const char[]){U16_TO_U8S_LE(0x0409)},
    // Manufacturer
    [STR_ID_MANUFACTURER] = USB_MANUFACTURER_NAME,
    // Product
    [STR_ID_PRODUCT] = USB_PRODUCT_NAME,
    // Serial number (will be filled by the callback)
    [STR_ID_SERIAL] = NULL,
};

_Static_assert(M_ARRAY_SIZE(desc_strings) == STR_ID_COUNT,
               "Invalid string descriptor size");

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

const uint8_t *tud_descriptor_device_cb(void) {
  return (const uint8_t *)&desc_device;
}

const uint8_t *tud_hid_descriptor_report_cb(uint8_t instance) {
  switch (instance) {
  case USB_ITF_KEYBOARD:
    return desc_keyboard_report;

  case USB_ITF_HID:
    return desc_hid_report;

  case USB_ITF_RAW_HID:
    return desc_raw_hid_report;

#if defined(LOG_ENABLED)
  case USB_ITF_LOG:
    return desc_log_report;
#endif

  default:
    // Invalid interface, should be unreachable
    return NULL;
  }
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
  // We only have one configuration so we don't need to check the index
  return desc_configuration;
}

#if defined(BOARD_USB_HS)
const uint8_t *tud_descriptor_device_qualifier_cb(void) {
  return (const uint8_t *)&desc_device_qualifier;
}

const uint8_t *tud_descriptor_other_speed_configuration_cb(uint8_t index) {
  memcpy(desc_other_speed_config, desc_configuration, CONFIG_TOTAL_LEN);
  desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

  return desc_other_speed_config;
}
#endif

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  static uint16_t desc_str[32 + 1];

  uint32_t chr_count;
  switch (index) {
  case STR_ID_LANGID:
    memcpy(desc_str + 1, desc_strings[STR_ID_LANGID], 2);
    chr_count = 1;
    break;

  case STR_ID_SERIAL:
    chr_count = board_serial(&desc_str[1]);
    break;

  default:
    if (index >= STR_ID_COUNT)
      // Unknown string
      // WARNING: Index 0xEE is a Microsoft OS 1.0 descriptor.
      return NULL;

    const char *str = desc_strings[index];

    // Truncate string if it is too long
    const uint32_t max_len = M_ARRAY_SIZE(desc_str) - 1;
    chr_count = strlen(str);
    if (chr_count > max_len)
      chr_count = max_len;

    // Convert ASCII string into UTF-16-LE
    for (uint32_t i = 0; i < chr_count; i++)
      desc_str[i + 1] = str[i];
    break;
  }

  // First byte is the length of the descriptor, second byte is the type
  desc_str[0] = (2 * chr_count + 2) | ((uint16_t)TUSB_DESC_STRING << 8);

  return desc_str;
}
