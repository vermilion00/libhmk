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

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

/* clang-format off
   ___   __   _  _  ____  ____   ___   ___ 
  / __) / _\ / )( \/ ___)/ ___) / __) / _ \
 ( (_ \/    \) \/ (\___ \\___ \(  _ \(__  (
  \___/\_/\_/\____/(____/(____/ \___/  (__/

clang-format on */

//--------------------------------------------------------------------+
// USB Device Configuration
//--------------------------------------------------------------------+

// Uncomment the following line if USB D+/D- is connected to PB14/PB15
#define USE_USB_OTG_HS_PORT

#define USB_MANUFACTURER_NAME "ABS0"
#define USB_PRODUCT_NAME "GAUSS64"

#define USB_VENDOR_ID 0xAB50
#define USB_PRODUCT_ID 0xAB01

// Uncomment the following lines to show a landing page popup on connection. The
// URL scheme (e.g. `https://`) should not be included in the URL and should be
// defined in `USB_WEBUSB_URL_SCHEME`.

// #define USB_WEBUSB_URL "example.com"
// #define USB_WEBUSB_URL_SCHEME 1

//--------------------------------------------------------------------+
// Keyboard Configuration
//--------------------------------------------------------------------+

#define NUM_PROFILES 4
#define NUM_LAYERS 4
#define NUM_KEYS 64
// #define ENABLE_BOOTLOADER
#define ENABLE_WEB_CONFIGURATOR

//--------------------------------------------------------------------+
// Hardware Configuration
//--------------------------------------------------------------------+

#define ADC_RESOLUTION 12
#define EEPROM_WEAR_LEVELING
// #define EEPROM_I2C
// #define EEPROM_24LC512

#endif