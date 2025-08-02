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

//---------------------------------------------------------------------+
// TinyUSB Configuration
//---------------------------------------------------------------------+

// STM32F446xx
#define CFG_TUSB_MCU OPT_MCU_STM32F4

//--------------------------------------------------------------------+
// Clock Configuration
//--------------------------------------------------------------------+

#define BOARD_HSE_VALUE 16000000

//--------------------------------------------------------------------+
// USB Configuration
//--------------------------------------------------------------------+

#define BOARD_USB_FS

#define USB_MANUFACTURER_NAME "ABS0"
#define USB_PRODUCT_NAME "HE60"
#define USB_VENDOR_ID 0xAB50
#define USB_PRODUCT_ID 0xAB60

//--------------------------------------------------------------------+
// Analog Configuration
//--------------------------------------------------------------------+

#define ADC_NUM_MUX_INPUTS 9
#define ADC_MUX_INPUT_CHANNELS {0, 1, 2, 3, 4, 5, 6, 7, 8}

// SN74LV4051A 8-Channel Analog Multiplexer
#define ADC_NUM_MUX_SELECT_PINS 3
#define ADC_MUX_SELECT_PORTS {GPIOC, GPIOC, GPIOC}
#define ADC_MUX_SELECT_PINS {GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15}
#define ADC_MUX_INPUT_MATRIX                                                    \
  {                                                                             \
      {4, 15, 18, 29, 36, 56, 59, 62, 66}, {3, 14, 17, 28, 35, 43, 58, 63, 67}, \
      {2, 13, 1, 27, 0, 42, 57, 64, 54},   {0, 16, 19, 30, 37, 55, 60, 61, 65}, \
      {5, 9, 20, 23, 32, 38, 31, 47, 50},  {8, 12, 22, 26, 0, 41, 46, 49, 53},  \
      {6, 10, 0, 24, 33, 39, 44, 0, 51},   {7, 11, 21, 25, 34, 40, 45, 48, 52}, \
  }

//--------------------------------------------------------------------+
// Key Matrix Configuration
//--------------------------------------------------------------------+

// OH49E-S output is not directly proportional to the travel distance of
// the keys so we must invert the values.
#define MATRIX_INVERT_ADC_VALUES

//--------------------------------------------------------------------+
// Keyboard Configuration
//--------------------------------------------------------------------+

// Approximated by measuring the actual values of GEON Raw HE switches
#define DEFAULT_CALIBRATION                                                    \
  {                                                                            \
      .initial_rest_value = 2400,                                              \
      .initial_bottom_out_threshold = 650,                                     \
  }

#define NUM_KEYS 67

#define DEFAULT_KEYMAP                                                         \
  {                                                                            \
      {                                                                        \
          KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,          \
          KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  _______,       \
          KC_BSPC, _______, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,          \
          KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,       \
          KC_RBRC, KC_BSLS, KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,          \
          KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,       \
          KC_ENT,  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,          \
          KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, MO(1),         \
          KC_LCTL, KC_LGUI, KC_LALT, _______, _______, KC_SPC,  _______,       \
          _______, KC_RALT, KC_RGUI, KC_RCTL,                                  \
      },                                                                       \
      {                                                                        \
          KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,         \
          KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,       \
          KC_DEL,  _______, KC_PSCR, _______, KC_PGUP, _______, _______,       \
          _______, _______, _______, _______, PF(0),   PF(1),   PF(2),         \
          PF(3),   SP_BOOT, _______, KC_HOME, KC_PGDN, KC_END,  _______,       \
          _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, _______, _______,       \
          PF_SWAP, _______, _______, _______, _______, _______, _______,       \
          _______, _______, KC_MUTE, KC_VOLD, KC_VOLU, KC_UP,   _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, KC_LEFT, KC_DOWN, KC_RGHT,                                  \
      },                                                                       \
      {                                                                        \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______,                                  \
      },                                                                       \
      {                                                                        \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______,                                  \
      },                                                                       \
  }

#define DEFAULT_ACTUATION_MAP                                                  \
  {                                                                            \
      [0 ... NUM_KEYS - 1] =                                                   \
          {                                                                    \
              .actuation_point = 128,                                          \
              .rt_down = 0,                                                    \
              .rt_up = 0,                                                      \
              .continuous = false,                                             \
          },                                                                   \
  }

#define DEFAULT_ADVANCED_KEYS                                                  \
  {                                                                            \
  }

#define DEFAULT_PROFILE                                                        \
  {                                                                            \
      .keymap = DEFAULT_KEYMAP,                                                \
      .actuation_map = DEFAULT_ACTUATION_MAP,                                  \
      .advanced_keys = DEFAULT_ADVANCED_KEYS,                                  \
      .tick_rate = 30,                                                         \
  }
