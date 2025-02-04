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

//--------------------------------------------------------------------+
// Clock Configuration
//--------------------------------------------------------------------+

#define BOARD_HSE_VALUE 16000000

//--------------------------------------------------------------------+
// USB Configuration
//--------------------------------------------------------------------+

#define BOARD_USB_HS

#define USB_MANUFACTURER_NAME "ABS0"
#define USB_PRODUCT_NAME "GAUSS64"
#define USB_VENDOR_ID 0xAB50
#define USB_PRODUCT_ID 0xAB01

//--------------------------------------------------------------------+
// Keyboard Configuration
//--------------------------------------------------------------------+

#define NUM_KEYS 64

#define DEFAULT_KEYMAP                                                         \
  {                                                                            \
      {                                                                        \
          KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,          \
          KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,  KC_MINS, KC_BSPC,       \
          KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,          \
          KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,       \
          KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,          \
          KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_VOLU,       \
          KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,          \
          KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_VOLD,       \
          KC_LCTL, KC_LGUI, KC_LALT, KC_SPC,  KC_RALT, KC_LEFT, KC_DOWN,       \
          KC_RGHT,                                                             \
      },                                                                       \
      {                                                                        \
          KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,         \
          KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,        \
          KC_PSCR, _______, KC_PGUP, _______, _______, _______, _______,       \
          _______, _______, PF(0),   PF(1),   PF(2),   PF(3),   _______,       \
          _______, KC_HOME, KC_PGDN, KC_END,  _______, _______, _______,       \
          KC_MPRV, KC_MPLY, KC_MNXT, _______, _______, PF_SWAP, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, _______, _______, _______, _______, _______, _______,       \
          _______, KY_LOCK, _______, _______, _______, _______, _______,       \
          _______,                                                             \
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
          _______,                                                             \
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
          _______,                                                             \
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
      [0] =                                                                    \
          {                                                                    \
              .layer = 0,                                                      \
              .key = 55,                                                       \
              .type = AK_TYPE_TAP_HOLD,                                        \
              .tap_hold =                                                      \
                  {                                                            \
                      .tap_keycode = KC_VOLD,                                  \
                      .hold_keycode = MO(1),                                   \
                      .tapping_term = 200,                                     \
                      .hold_on_other_key_press = true,                         \
                  },                                                           \
          },                                                                   \
  }

#define DEFAULT_PROFILE                                                        \
  {                                                                            \
      .keymap = DEFAULT_KEYMAP,                                                \
      .actuation_map = DEFAULT_ACTUATION_MAP,                                  \
      .advanced_keys = DEFAULT_ADVANCED_KEYS,                                  \
  }
