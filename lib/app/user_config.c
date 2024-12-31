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

#include "user_config.h"

#include <memory.h>

#include "crc32.h"
#include "eeprom.h"
#include "switches.h"

extern const user_config_t default_user_config;
// Non-static to allow intrusive access for the vendor class
user_config_t user_config;

/**
 * @brief Calculate the CRC32 of the user configuration
 *
 * @return The CRC32 of the user configuration
 */
static inline uint32_t user_config_crc32(void) {
    return crc32_bitwise(0, (const uint8_t *)&user_config + 4,
                         sizeof(user_config_t) - 4);
}

/**
 * @brief Update the CRC32 of the user configuration and save it
 *
 * @return none
 */
static inline void user_config_save_crc32(void) {
    user_config.crc32 = user_config_crc32();
    eeprom_write(offsetof(user_config_t, crc32), (uint8_t *)&user_config.crc32,
                 sizeof(user_config.crc32));
}

void user_config_init(void) {
    // TODO: Handle version migration

    // Load the user configuration from EEPROM
    eeprom_read(0, (uint8_t *)&user_config, sizeof(user_config_t));

    // Verify the user configuration
    if (user_config.crc32 != user_config_crc32())
        // The user configuration is invalid, reset it to the default
        user_config_reset();
}

void user_config_reset(void) {
    eeprom_erase();
    user_config = default_user_config;
    user_config_save_crc32();
    // Save the rest of the user configuration
    eeprom_write(4, (uint8_t *)&user_config + 4, sizeof(user_config_t) - 4);
}

uint8_t user_config_sw_id(void) { return user_config.sw_id; }

void user_config_set_sw_id(uint8_t sw_id) {
    if (sw_id >= SW_COUNT)
        return;

    if (user_config.sw_id == sw_id)
        return;

    user_config.sw_id = sw_id;
    user_config_save_crc32();
    eeprom_write(SW_ID_OFFSET, &user_config.sw_id, sizeof(uint8_t));
}

uint8_t user_config_tap_hold(void) { return user_config.tap_hold; }

void user_config_set_tap_hold(uint8_t tap_hold) {
    if (tap_hold >= TAP_HOLD_COUNT)
        return;

    if (user_config.tap_hold == tap_hold)
        return;

    user_config.tap_hold = tap_hold;
    user_config_save_crc32();
    eeprom_write(TAP_HOLD_OFFSET, &user_config.tap_hold, sizeof(uint8_t));
}

uint8_t user_config_current_profile(void) {
    return user_config.current_profile;
}

void user_config_set_current_profile(uint8_t profile) {
    if (profile >= NUM_PROFILES)
        return;

    if (user_config.current_profile == profile)
        return;

    user_config.current_profile = profile;
    user_config_save_crc32();
    eeprom_write(CURRENT_PROFILE_OFFSET,
                 (uint8_t *)&user_config.current_profile, sizeof(uint8_t));
}

key_config_t *user_config_key_config(uint8_t profile, uint16_t index) {
    return &user_config.key_config[profile][index];
}

void user_config_set_key_config(uint8_t profile, uint16_t index,
                                const key_config_t *key_config) {
    if (profile >= NUM_PROFILES || index >= NUM_KEYS)
        return;

    if (memcmp(&user_config.key_config[profile][index], key_config,
               sizeof(key_config_t)) == 0)
        return;

    user_config.key_config[profile][index] = *key_config;
    user_config_save_crc32();
    eeprom_write(KEY_CONFIG_OFFSET(profile, index),
                 (uint8_t *)&user_config.key_config[profile][index],
                 sizeof(key_config_t));
}

uint16_t user_config_keymap(uint8_t profile, uint8_t layer, uint16_t index) {
    return user_config.keymap[profile][layer][index];
}

void user_config_set_keymap(uint8_t profile, uint8_t layer, uint16_t index,
                            uint16_t keymap) {
    if (profile >= NUM_PROFILES || layer >= NUM_LAYERS || index >= NUM_KEYS)
        return;

    if (user_config.keymap[profile][layer][index] == keymap)
        return;

    user_config.keymap[profile][layer][index] = keymap;
    user_config_save_crc32();
    eeprom_write(KEYMAP_OFFSET(profile, layer, index),
                 (uint8_t *)&user_config.keymap[profile][layer][index],
                 sizeof(uint16_t));
}

dynamic_keystroke_config_t *
user_config_dynamic_keystroke_config(uint8_t profile, uint8_t index) {
    return &user_config.dynamic_keystroke_config[profile][index];
}

void user_config_set_dynamic_keystroke_config(
    uint8_t profile, uint8_t index, const dynamic_keystroke_config_t *config) {
    if (profile >= NUM_PROFILES || index >= NUM_DYNAMIC_KEYSTROKE_CONFIGS)
        return;

    if (memcmp(&user_config.dynamic_keystroke_config[profile][index], config,
               sizeof(dynamic_keystroke_config_t)) == 0)
        return;

    user_config.dynamic_keystroke_config[profile][index] = *config;
    user_config_save_crc32();
    eeprom_write(
        DYNAMIC_KEYSTROKE_CONFIG_OFFSET(profile, index),
        (uint8_t *)&user_config.dynamic_keystroke_config[profile][index],
        sizeof(dynamic_keystroke_config_t));
}