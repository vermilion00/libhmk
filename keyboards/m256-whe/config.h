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
#define USB_PRODUCT_NAME "M256-WHE"
#define USB_VENDOR_ID 0xAB50
#define USB_PRODUCT_ID 0xAB65

//--------------------------------------------------------------------+
// Analog Configuration
//--------------------------------------------------------------------+

#define ADC_NUM_MUX_INPUTS 9
#define ADC_MUX_INPUT_CHANNELS {3, 4, 2, 5, 1, 6, 0, 7, 8}

// SN74LV4051A 8-Channel Analog Multiplexer
#define ADC_NUM_MUX_SELECT_PINS 3
#define ADC_MUX_SELECT_PORTS {GPIOC, GPIOC, GPIOC}
#define ADC_MUX_SELECT_PINS {GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15}
#define ADC_MUX_INPUT_MATRIX                                                    \
  {                                                                             \
      {3, 15, 20, 32, 33, 45, 63, 0, 0},   {2, 14, 19, 31, 47, 44, 62, 0, 67},  \
      {1, 13, 18, 30, 48, 43, 61, 64, 66}, {4, 16, 21, 17, 34, 46, 0, 65, 68},  \
      {5, 9, 22, 26, 35, 39, 49, 53, 57},  {8, 12, 25, 29, 38, 42, 52, 56, 60}, \
      {6, 10, 23, 27, 36, 40, 50, 54, 58}, {7, 11, 24, 28, 37, 41, 51, 55, 59}, \
  }

//--------------------------------------------------------------------+
// Keyboard Configuration
//--------------------------------------------------------------------+

// Approximated by measuring the actual values of Owlab Ti HE switches on
// a 1.6mm thick PCB
#define DEFAULT_CALIBRATION                                                    \
  {                                                                            \
      .initial_rest_value = 2300,                                              \
      .initial_bottom_out_threshold = 650,                                     \
  }

#define NUM_KEYS 68

#define DEFAULT_KEYMAP                                                         \
  {                                                                            \
      {                                                                        \
          KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,          \
          KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  _______,       \
          KC_BSPC, _______, KC_DEL,  KC_TAB,  KC_Q,    KC_W,    KC_E,          \
          KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,          \
          KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP, KC_CAPS, KC_A,    KC_S,          \
          KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,          \
          KC_SCLN, KC_QUOT, KC_ENT,  KC_PGDN, KC_LSFT, KC_Z,    KC_X,          \
          KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,        \
          KC_SLSH, KC_RSFT, KC_UP,   MO(1),   KC_LCTL, KC_LGUI, KC_LALT,       \
          KC_SPC,  KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT,                         \
      },                                                                       \
      {                                                                        \
          KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,         \
          KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,       \
          _______, _______, _______, KC_PSCR, _______, KC_PGUP, _______,       \
          _______, _______, _______, _______, _______, PF(0),   PF(1),         \
          PF(2),   PF(3),   SP_BOOT, _______, _______, KC_HOME, KC_PGDN,       \
          KC_END,  _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT,       \
          _______, _______, PF_SWAP, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, KC_MUTE, KC_VOLD,       \
          KC_VOLU, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______,                         \
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
          _______, _______, _______, _______, _______,                         \
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
          _______, _______, _______, _______, _______,                         \
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
