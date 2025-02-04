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

#include "common.h"
#include "wear_leveling.h"

//--------------------------------------------------------------------+
// Keyboard Persistent Configuration
//--------------------------------------------------------------------+

// Persistent configuration version
#define EECONFIG_VERSION 0x0100
// Magic number to identify the start of the configuration
#define EECONFIG_MAGIC_START 0x0A42494C
// Magic number to identify the end of the configuration
#define EECONFIG_MAGIC_END 0x0A4B4D48

// Keyboard profile configuration
typedef struct __attribute__((packed)) {
  uint8_t keymap[NUM_LAYERS][NUM_KEYS];
  actuation_t actuation_map[NUM_KEYS];
  advanced_key_t advanced_keys[NUM_ADVANCED_KEYS];
} eeconfig_profile_t;

// Keyboard configuration
typedef struct __attribute__((packed)) {
  uint32_t magic_start;
  uint16_t version;
  uint8_t current_profile;
  uint8_t last_non_default_profile;
  eeconfig_profile_t profiles[NUM_PROFILES];
  uint32_t magic_end;
} eeconfig_t;

_Static_assert(
    sizeof(eeconfig_t) <= WL_VIRTUAL_SIZE,
    "Keyboard configuration size must be at most the virtual storage size.");

extern const eeconfig_t *eeconfig;

#define CURRENT_PROFILE (eeconfig->profiles[eeconfig->current_profile])

//--------------------------------------------------------------------+
// Default Profile Configuration
//--------------------------------------------------------------------+

#if !defined(DEFAULT_PROFILE)
#error "DEFAULT_PROFILE is not defined"
#endif

//--------------------------------------------------------------------+
// Persistent Configuration API
//--------------------------------------------------------------------+

/**
 * @brief Initialize the persistent configuration module
 *
 * @return None
 */
void eeconfig_init(void);

/**
 * @brief Reset the persistent configuration to default values
 *
 * @return true if successful, false otherwise
 */
bool eeconfig_reset(void);
