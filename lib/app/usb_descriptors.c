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

#include "board.h"
#include "tusb.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

const tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0210, // Must be at least USB 2.1 to support WebUSB

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

/**
 * @brief USB device descriptor callback
 *
 * @return pointer to descriptor
 */
const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

const uint8_t desc_hid_report[] = {
    HID_REPORT_DESC_HYBRID_KB(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(REPORT_ID_SYSTEM_CONTROL)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL)),
};

/**
 * @brief USB HID report descriptor callback
 *
 * @param instance HID instance
 *
 * @return pointer to descriptor
 */
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    // We only have one HID report descriptor so we ignore the instance.
    return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum {
    ITF_NUM_HID,
    ITF_NUM_VENDOR,
    ITF_NUM_TOTAL,
};

#define CONFIG_TOTAL_LEN                                                       \
    (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_VENDOR_DESC_LEN)

#define EPNUM_HID 0x81
#define EPNUM_VENDOR_IN 0x82
#define EPNUM_VENDOR_OUT 0x02

const uint8_t desc_configuration[] = {
    // Configuration number, interface count, string index, total length,
    // attribute, power (mA)
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

    // Interface number, string index, protocol, report descriptor length, EP In
    // address, EP size, polling interval (1ms for USB FS, 0.125ms for USB
    // HS)
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_hid_report), EPNUM_HID,
                       CFG_TUD_HID_EP_BUFSIZE, 1),

    // Interface number, string index, EP Out & IN address, EP size
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 4, EPNUM_VENDOR_OUT,
                          0x80 | EPNUM_VENDOR_IN,
                          TUD_OPT_HIGH_SPEED ? 512 : 64),
};

/**
 * @brief USB configuration descriptor callback
 *
 * @param index Configuration index
 *
 * @return pointer to descriptor
 */
const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    // We only have one configuration so we ignore the index.
    return desc_configuration;
}

//--------------------------------------------------------------------+
// BOS Descriptor
//--------------------------------------------------------------------+

#define BOS_TOTAL_LEN                                                          \
    (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_SET_HEADER_LEN 0x0A
#define MS_OS_20_SUBSET_HEADER_CONFIGURATION_LEN 0x08
#define MS_OS_20_SUBSET_HEADER_FUNCTION_LEN 0x08
#define MS_OS_20_FEATURE_COMPATBLE_ID_LEN 0x14
#define MS_OS_20_FEATURE_REG_PROPERTY_LEN 0x84
#define MS_OS_20_SUBSET_FUNCTION_LEN                                           \
    (MS_OS_20_SUBSET_HEADER_FUNCTION_LEN + MS_OS_20_FEATURE_COMPATBLE_ID_LEN + \
     MS_OS_20_FEATURE_REG_PROPERTY_LEN)

#define MS_OS_20_SUBSET_CONFIGURATION_LEN                                      \
    (MS_OS_20_SUBSET_HEADER_CONFIGURATION_LEN +                                \
     ITF_NUM_TOTAL * MS_OS_20_SUBSET_FUNCTION_LEN)
#define MS_OS_20_DESC_LEN                                                      \
    (MS_OS_20_SET_HEADER_LEN + MS_OS_20_SUBSET_CONFIGURATION_LEN)

// BOS descriptor is required for WebUSB.
const uint8_t desc_bos[] = {
    // Total length, number of device capabilities
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

// If WebUSB URL is defined, we will display a landing page.
#if defined(USB_WEBUSB_URL)
    // Vendor request, whether to display a landing page
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),
#else
    // Vendor request, whether to display a landing page
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 0),
#endif

    // Microsoft OS 2.0 descriptor length, vendor request
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT),
};

/**
 * @brief USB BOS descriptor callback
 *
 * @return pointer to descriptor
 */
const uint8_t *tud_descriptor_bos_cb(void) { return desc_bos; }

// Microsoft OS 2.0 Descriptors
// https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-os-2-0-descriptors-specification
uint8_t const desc_ms_os_20[] = {
    // Set header length
    U16_TO_U8S_LE(MS_OS_20_SET_HEADER_LEN),
    // Set header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR),
    // Minimum supported Windows version (Windows 8.1)
    U32_TO_U8S_LE(0x06030000),
    // Total length of MS OS 2.0 descriptor
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // Subset header length
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION_LEN),
    // Subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION),
    // Configuration index
    0x00,
    // Reserved
    0x00,
    // Length of this configuration subset including this header
    U16_TO_U8S_LE(MS_OS_20_SUBSET_CONFIGURATION_LEN),

    // Function subset header length
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION_LEN),
    // Function subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    // First interface
    ITF_NUM_HID,
    // Reserved
    0x00,
    // Length of this function subset including this header
    U16_TO_U8S_LE(MS_OS_20_SUBSET_FUNCTION_LEN),

    // Feature descriptor length
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID_LEN),
    // Feature descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID),
    // Compatible ID
    // HID devices do not require driver installation
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Sub-compatible ID
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry property descriptor length
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY_LEN),
    // Registry property descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    // Property data type (REG_MULTI_SZ)
    U16_TO_U8S_LE(0x0007),
    // Property name length
    U16_TO_U8S_LE(0x002A),
    // Property name
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00,
    'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00,
    'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00,
    0x00,
    // Property data length
    U16_TO_U8S_LE(0x0050),
    // Property data (randomly generated GUID)
    '{', 0x00, 'B', 0x00, 'C', 0x00, '1', 0x00, '7', 0x00, 'B', 0x00, '9', 0x00,
    '7', 0x00, 'D', 0x00, '-', 0x00, '8', 0x00, '1', 0x00, '9', 0x00, 'F', 0x00,
    '-', 0x00, '4', 0x00, 'F', 0x00, '6', 0x00, 'F', 0x00, '-', 0x00, '8', 0x00,
    '4', 0x00, '4', 0x00, '4', 0x00, '-', 0x00, '9', 0x00, '0', 0x00, '4', 0x00,
    'B', 0x00, 'D', 0x00, '7', 0x00, 'C', 0x00, '8', 0x00, '6', 0x00, '4', 0x00,
    '7', 0x00, '0', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,

    // Function subset header length
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION_LEN),
    // Function subset header descriptor type
    U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    // Second interface
    ITF_NUM_VENDOR,
    // Reserved
    0x00,
    // Length of this function subset including this header
    U16_TO_U8S_LE(MS_OS_20_SUBSET_FUNCTION_LEN),

    // Feature descriptor length
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID_LEN),
    // Feature descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID),
    // Compatible ID
    // We use WinUSB to install generic USB driver for vendor interface
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    // Sub-compatible ID
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry property descriptor length
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY_LEN),
    // Registry property descriptor type
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    // Property data type (REG_MULTI_SZ)
    U16_TO_U8S_LE(0x0007),
    // Property name length
    U16_TO_U8S_LE(0x002A),
    // Property name
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00,
    'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00,
    'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00,
    0x00,
    // Property data length
    U16_TO_U8S_LE(0x0050),
    // Property data (randomly generated GUID)
    '{', 0x00, '6', 0x00, '8', 0x00, 'D', 0x00, '0', 0x00, '2', 0x00, '1', 0x00,
    '7', 0x00, 'F', 0x00, '-', 0x00, '4', 0x00, '1', 0x00, 'E', 0x00, '7', 0x00,
    '-', 0x00, '4', 0x00, '7', 0x00, 'D', 0x00, 'A', 0x00, '-', 0x00, 'A', 0x00,
    '3', 0x00, '3', 0x00, '2', 0x00, '-', 0x00, 'C', 0x00, 'C', 0x00, '2', 0x00,
    '8', 0x00, '9', 0x00, 'C', 0x00, '0', 0x00, '2', 0x00, 'F', 0x00, 'E', 0x00,
    '3', 0x00, 'E', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00

};

_Static_assert(sizeof(desc_ms_os_20) == MS_OS_20_DESC_LEN,
               "Invalid MS OS 2.0 descriptor size");

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String descriptor index
enum {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};

// Array of pointer to string descriptors
const char *string_desc_arr[] = {
    (const char[]){
        U16_TO_U8S_LE(0x0409)}, // 0: is supported language is English (0x0409)
    USB_MANUFACTURER_NAME,      // 1: Manufacturer
    USB_PRODUCT_NAME,           // 2: Product
    NULL,                       // 3: Serials will use unique ID if possible
    USB_VENDOR_INTF_NAME,       // 4: Vendor interface
};

/**
 * @brief USB string descriptor callback
 *
 * @param index String index
 * @param langid Language ID
 *
 * @return pointer to string descriptor that exists long enough for the transfer
 * to complete
 */
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t desc_str[32 + 1];
    size_t chr_count;

    switch (index) {
    case STRID_LANGID:
        memcpy(&desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
        break;

    case STRID_SERIAL:
        chr_count = board_serial_number(&desc_str[1]);
        break;

    default:
        // WARNING: Index 0xEE is a Microsoft OS 1.0 descriptor.ks
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            // Index out of range
            return NULL;

        const char *str = string_desc_arr[index];

        // Truncate string if it is too long
        const size_t max_count = sizeof(desc_str) / sizeof(desc_str[0])
                                 // -1 for the header
                                 - 1;
        chr_count = strlen(str);
        if (chr_count > max_count)
            chr_count = max_count;

        // Convert ASCII string into UTF-16
        for (size_t i = 0; i < chr_count; i++)
            desc_str[1 + i] = str[i];
        break;
    }

    // First byte is length (including header), second byte is string type
    desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return desc_str;
}
