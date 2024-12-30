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

#include "config.h"
#include "eeprom.h"

//--------------------------------------------------------------------+
// User Configuration Definitions
//--------------------------------------------------------------------+

#define USER_CONFIG_VERSION 0x0100

#if !defined(NUM_PROFILES)
// The number of profiles
#define NUM_PROFILES 4
#endif

_Static_assert(NUM_PROFILES <= 16, "Invalid number of profiles");

#if !defined(NUM_LAYERS)
// The number of layers
#define NUM_LAYERS 4
#endif

_Static_assert(NUM_LAYERS <= 16, "Invalid number of layers");

#if !defined(NUM_KEYS)
#error "NUM_KEYS must be defined"
#endif

_Static_assert(NUM_KEYS <= UINT16_MAX, "Invalid number of keys");

#if !defined(NUM_DYNAMIC_KEYSTROKE_CONFIGS)
// The number of dynamic keystroke configurations per profile
#define NUM_DYNAMIC_KEYSTROKE_CONFIGS 16
#endif

_Static_assert(NUM_DYNAMIC_KEYSTROKE_CONFIGS <= 256,
               "Invalid number of dynamic keystroke configurations");

//--------------------------------------------------------------------+
// User Configuration Types
//--------------------------------------------------------------------+

enum {
    KEY_MODE_NORMAL = 0,
    KEY_MODE_RAPID_TRIGGER,
};

typedef struct __attribute__((packed)) {
    // Actuation distance in 0.05mm
    uint8_t actuation_distance;
    // Bottom-out distance in 0.05mm. Only used by dynamic keystroke
    uint8_t bottom_out_distance;
} key_mode_normal_t;

typedef struct __attribute__((packed)) {
    // Actuation distance in 0.05mm. Rapid trigger becomes active after this
    // distance.
    uint8_t actuation_distance;
    // Reset distance in 0.05mm. Rapid trigger becomes inactive after this
    // distance.
    uint8_t reset_distance;
    // Distance in 0.05mm to actuate the key when rapid trigger is active
    uint8_t rt_down_distance;
    // Distance in 0.05mm to release the key when rapid trigger is active
    uint8_t rt_up_distance;
} key_mode_rapid_trigger_t;

typedef struct __attribute__((packed)) {
    // Tapping term in milliseconds
    uint16_t tapping_term;
    uint8_t mode;

    union {
        key_mode_normal_t nm;
        key_mode_rapid_trigger_t rt;
    };
} key_config_t;

// Tap-hold configuration
enum {
    // Hold action is triggered after the tapping term.
    TAP_HOLD_DEFAULT = 0,
    // Hold action is triggered either after the tapping term or when another
    // key is pressed.
    TAP_HOLD_HOLD_ON_OTHER_KEY_PRESS,
};

// Used for representing all possible configurations of a single key dynamic
// keystroke. Each part of the mask roughly corresponds to the length of the
// pill-shaped bar you see in Wootility.
typedef struct __attribute__((packed)) {
    uint8_t config0 : 3;
    uint8_t config1 : 2;
    uint8_t config2 : 2;
    uint8_t config3 : 1;
} dynamic_keystroke_mask_t;

_Static_assert(sizeof(dynamic_keystroke_mask_t) == 1,
               "Invalid size of dynamic keystroke mask");

typedef struct __attribute__((packed)) {
    uint8_t keycode[4];
    dynamic_keystroke_mask_t mask[4];
} dynamic_keystroke_config_t;

typedef struct __attribute__((packed)) {
    // For verifying the configuration
    uint32_t crc32;
    // Configuration version
    uint16_t version;

    // Current switch used
    uint8_t sw_id;
    // Tap-hold configuration
    uint8_t tap_hold;
    // Current profile
    uint8_t current_profile;

    // Key configurations. Each profile is applied throughout all layers.
    key_config_t key_config[NUM_PROFILES][NUM_KEYS];
    uint16_t keymap[NUM_PROFILES][NUM_LAYERS][NUM_KEYS];
    dynamic_keystroke_config_t
        dynamic_keystroke_config[NUM_PROFILES][NUM_DYNAMIC_KEYSTROKE_CONFIGS];
} user_config_t;

_Static_assert(sizeof(user_config_t) <= EEPROM_BYTES,
               "User configuration size exceeds EEPROM size");

//--------------------------------------------------------------------+
// User Configuration APIs
//--------------------------------------------------------------------+

/**
 * @brief Initialize the user configuration module
 *
 * @return none
 */
void user_config_init(void);

/**
 * @brief Reset the user configuration to the default configuration and save it
 *
 * @return none
 */
void user_config_reset(void);

/**
 * @brief Get the switch ID from the user configuration
 *
 * @return The switch ID
 */
uint8_t user_config_sw_id(void);

/**
 * @brief Get the tap-hold configuration from the user configuration
 *
 * @return The tap-hold configuration
 */
uint8_t user_config_tap_hold(void);

/**
 * @brief Get the current profile from the user configuration
 *
 * @return The current profile
 */
uint8_t user_config_current_profile(void);

/**
 * @brief Set the current profile in the user configuration
 *
 * @param profile The profile
 *
 * @return none
 */
void user_config_set_current_profile(uint8_t profile);

/**
 * @brief Get the key configuration from the user configuration
 *
 * @param index The key index
 *
 * @return The key configuration
 */
key_config_t *user_config_key_config(uint8_t profile, uint16_t index);

/**
 * @brief Get the keymap from the user configuration
 *
 * @param profile The profile
 * @param layer The layer
 * @param index The key index
 *
 * @return The keymap
 */
uint16_t user_config_keymap(uint8_t profile, uint8_t layer, uint16_t index);

/**
 * @brief Set the keymap in the user configuration
 *
 * @param profile The profile
 * @param layer The layer
 * @param index The key index
 * @param keycode The keycode
 *
 * @return none
 */
void user_config_set_keymap(uint8_t profile, uint8_t layer, uint16_t index,
                            uint16_t keycode);

/**
 * @brief Get the dynamic keystroke configuration from the user configuration
 *
 * @param profile The profile
 * @param index The dynamic keystroke index
 *
 * @return The dynamic keystroke configuration
 */
dynamic_keystroke_config_t *
user_config_dynamic_keystroke_config(uint8_t profile, uint8_t index);