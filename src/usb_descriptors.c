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
#include "vendor.h"

// Device descriptor
static const tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    // Must be at least USB 2.1 to support WebUSB
    .bcdUSB = 0x0210,

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
static const uint8_t desc_keyboard_report[] = {HID_REPORT_DESC_NKRO_KB()};

// HID report descriptor for the rest of the interfaces
static const uint8_t desc_hid_report[] = {
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(REPORT_ID_SYSTEM_CONTROL)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
};

#define CONFIG_TOTAL_LEN                                                       \
  (TUD_CONFIG_DESC_LEN + 2 * TUD_HID_DESC_LEN + TUD_VENDOR_DESC_LEN)

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
    // Vendor interface descriptor
    TUD_VENDOR_DESCRIPTOR(USB_ITF_VENDOR, STR_ID_VENDOR, EP_OUT(3), EP_IN(3),
                          CFG_TUD_VENDOR_EPSIZE),
};

_Static_assert(M_ARRAY_SIZE(desc_configuration) == CONFIG_TOTAL_LEN,
               "Invalid configuration descriptor size");

#if defined(BOARD_USB_HS)
// Device qualifier descriptor for USB HS
static const tusb_desc_device_qualifier_t desc_device_qualifier = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = 0x0210,

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

#define BOS_TOTAL_LEN                                                          \
  (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

const uint8_t desc_bos[] = {
    // BOS descriptor header
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),
#if defined(VENDOR_WEBUSB_URL)
    // WebUSB BOS descriptor with landing page URL
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),
#else
    // WebUSB BOS descriptor without landing page URL
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 0),
#endif
    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT),
};

_Static_assert(M_ARRAY_SIZE(desc_bos) == BOS_TOTAL_LEN,
               "Invalid BOS descriptor size");

// Microsoft OS 2.0 descriptor
// https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-os-2-0-descriptors-specification
const uint8_t desc_ms_os_20[] = {
    // Set header length
    U16_TO_U8S_LE(0x000A),
    // Set header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR),
    // Minimum supported Windows version (Windows 8.1)
    U32_TO_U8S_LE(0x06030000),
    // Total length of MS OS 2.0 descriptor
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // Subset header length
    U16_TO_U8S_LE(0x0008),
    // Subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION),
    // Configuration index
    0x00,
    // Reserved
    0x00,
    // Total length of subset including header
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x000A),

    // Function subset header length
    U16_TO_U8S_LE(0x0008),
    // Function subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    // First interface
    USB_ITF_KEYBOARD,
    // Reserved
    0x00,
    // Total length of function subset including header
    U16_TO_U8S_LE(0x00A0),

    // Feature descriptor length
    U16_TO_U8S_LE(0x0014),
    // Feature descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID),
    // Compatible ID (no driver needed for HID interface)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Sub-compatible ID
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry property descriptor length
    U16_TO_U8S_LE(0x0084),
    // Registry property descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    // Property data type (REG_MULTI_SZ)
    U16_TO_U8S_LE(0x0007),
    // Property name length
    U16_TO_U8S_LE(0x002A),
    // Property name
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('e'), U16_TO_U8S_LE('v'),
    U16_TO_U8S_LE('i'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('I'), U16_TO_U8S_LE('n'), U16_TO_U8S_LE('t'),
    U16_TO_U8S_LE('e'), U16_TO_U8S_LE('r'), U16_TO_U8S_LE('f'),
    U16_TO_U8S_LE('a'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('G'), U16_TO_U8S_LE('U'), U16_TO_U8S_LE('I'),
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('s'), U16_TO_U8S_LE('\0'),
    // Property data length
    U16_TO_U8S_LE(0x0050),
    // Property data (random GUID)
    U16_TO_U8S_LE('{'), U16_TO_U8S_LE('B'), U16_TO_U8S_LE('C'),
    U16_TO_U8S_LE('1'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('B'),
    U16_TO_U8S_LE('9'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('D'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('8'), U16_TO_U8S_LE('1'),
    U16_TO_U8S_LE('9'), U16_TO_U8S_LE('F'), U16_TO_U8S_LE('-'),
    U16_TO_U8S_LE('4'), U16_TO_U8S_LE('F'), U16_TO_U8S_LE('6'),
    U16_TO_U8S_LE('F'), U16_TO_U8S_LE('-'), U16_TO_U8S_LE('8'),
    U16_TO_U8S_LE('4'), U16_TO_U8S_LE('4'), U16_TO_U8S_LE('4'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('9'), U16_TO_U8S_LE('0'),
    U16_TO_U8S_LE('4'), U16_TO_U8S_LE('B'), U16_TO_U8S_LE('D'),
    U16_TO_U8S_LE('7'), U16_TO_U8S_LE('C'), U16_TO_U8S_LE('8'),
    U16_TO_U8S_LE('6'), U16_TO_U8S_LE('4'), U16_TO_U8S_LE('7'),
    U16_TO_U8S_LE('0'), U16_TO_U8S_LE('}'), U16_TO_U8S_BE('\0'),
    U16_TO_U8S_BE('\0'),

    // Function subset header length
    U16_TO_U8S_LE(0x0008),
    // Function subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    // Second interface
    USB_ITF_HID,
    // Reserved
    0x00,
    // Total length of function subset including header
    U16_TO_U8S_LE(0x00A0),

    // Feature descriptor length
    U16_TO_U8S_LE(0x0014),
    // Feature descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID),
    // Compatible ID (no driver needed for HID interface)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Sub-compatible ID
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry property descriptor length
    U16_TO_U8S_LE(0x0084),
    // Registry property descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    // Property data type (REG_MULTI_SZ)
    U16_TO_U8S_LE(0x0007),
    // Property name length
    U16_TO_U8S_LE(0x002A),
    // Property name
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('e'), U16_TO_U8S_LE('v'),
    U16_TO_U8S_LE('i'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('I'), U16_TO_U8S_LE('n'), U16_TO_U8S_LE('t'),
    U16_TO_U8S_LE('e'), U16_TO_U8S_LE('r'), U16_TO_U8S_LE('f'),
    U16_TO_U8S_LE('a'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('G'), U16_TO_U8S_LE('U'), U16_TO_U8S_LE('I'),
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('s'), U16_TO_U8S_LE('\0'),
    // Property data length
    U16_TO_U8S_LE(0x0050),
    // Property data (random GUID)
    U16_TO_U8S_LE('{'), U16_TO_U8S_LE('A'), U16_TO_U8S_LE('0'),
    U16_TO_U8S_LE('1'), U16_TO_U8S_LE('3'), U16_TO_U8S_LE('A'),
    U16_TO_U8S_LE('5'), U16_TO_U8S_LE('1'), U16_TO_U8S_LE('8'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('6'), U16_TO_U8S_LE('0'),
    U16_TO_U8S_LE('7'), U16_TO_U8S_LE('9'), U16_TO_U8S_LE('-'),
    U16_TO_U8S_LE('4'), U16_TO_U8S_LE('E'), U16_TO_U8S_LE('2'),
    U16_TO_U8S_LE('C'), U16_TO_U8S_LE('-'), U16_TO_U8S_LE('9'),
    U16_TO_U8S_LE('5'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('C'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('C'), U16_TO_U8S_LE('E'),
    U16_TO_U8S_LE('3'), U16_TO_U8S_LE('1'), U16_TO_U8S_LE('D'),
    U16_TO_U8S_LE('6'), U16_TO_U8S_LE('8'), U16_TO_U8S_LE('5'),
    U16_TO_U8S_LE('0'), U16_TO_U8S_LE('8'), U16_TO_U8S_LE('E'),
    U16_TO_U8S_LE('3'), U16_TO_U8S_LE('}'), U16_TO_U8S_BE('\0'),
    U16_TO_U8S_BE('\0'),

    // Function subset header length
    U16_TO_U8S_LE(0x0008),
    // Function subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    // Third interface
    USB_ITF_VENDOR,
    // Reserved
    0x00,
    // Total length of function subset including header
    U16_TO_U8S_LE(0x00A0),

    // Feature descriptor length
    U16_TO_U8S_LE(0x0014),
    // Feature descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID),
    // Compatible ID (WinUSB driver needed for vendor interface)
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    // Sub-compatible ID
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry property descriptor length
    U16_TO_U8S_LE(0x0084),
    // Registry property descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    // Property data type (REG_MULTI_SZ)
    U16_TO_U8S_LE(0x0007),
    // Property name length
    U16_TO_U8S_LE(0x002A),
    // Property name
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('e'), U16_TO_U8S_LE('v'),
    U16_TO_U8S_LE('i'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('I'), U16_TO_U8S_LE('n'), U16_TO_U8S_LE('t'),
    U16_TO_U8S_LE('e'), U16_TO_U8S_LE('r'), U16_TO_U8S_LE('f'),
    U16_TO_U8S_LE('a'), U16_TO_U8S_LE('c'), U16_TO_U8S_LE('e'),
    U16_TO_U8S_LE('G'), U16_TO_U8S_LE('U'), U16_TO_U8S_LE('I'),
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('s'), U16_TO_U8S_LE('\0'),
    // Property data length
    U16_TO_U8S_LE(0x0050),
    // Property data (random GUID)
    U16_TO_U8S_LE('{'), U16_TO_U8S_LE('6'), U16_TO_U8S_LE('8'),
    U16_TO_U8S_LE('D'), U16_TO_U8S_LE('0'), U16_TO_U8S_LE('2'),
    U16_TO_U8S_LE('1'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('F'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('4'), U16_TO_U8S_LE('1'),
    U16_TO_U8S_LE('E'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('-'),
    U16_TO_U8S_LE('4'), U16_TO_U8S_LE('7'), U16_TO_U8S_LE('D'),
    U16_TO_U8S_LE('A'), U16_TO_U8S_LE('-'), U16_TO_U8S_LE('A'),
    U16_TO_U8S_LE('3'), U16_TO_U8S_LE('3'), U16_TO_U8S_LE('2'),
    U16_TO_U8S_LE('-'), U16_TO_U8S_LE('C'), U16_TO_U8S_LE('C'),
    U16_TO_U8S_LE('2'), U16_TO_U8S_LE('8'), U16_TO_U8S_LE('9'),
    U16_TO_U8S_LE('C'), U16_TO_U8S_LE('0'), U16_TO_U8S_LE('2'),
    U16_TO_U8S_LE('F'), U16_TO_U8S_LE('E'), U16_TO_U8S_LE('3'),
    U16_TO_U8S_LE('E'), U16_TO_U8S_LE('}'), U16_TO_U8S_BE('\0'),
    U16_TO_U8S_BE('\0')

};

_Static_assert(M_ARRAY_SIZE(desc_ms_os_20) == MS_OS_20_DESC_LEN,
               "Invalid Microsoft OS 2.0 descriptor size");

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
    // Vendor interface
    [STR_ID_VENDOR] = USB_VENDOR_ITF_NAME,
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
  return instance == USB_ITF_KEYBOARD ? desc_keyboard_report : desc_hid_report;
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

const uint8_t *tud_descriptor_bos_cb(void) { return desc_bos; }

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
