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

extern const user_config_t default_user_config;
static user_config_t user_config;

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
    // Load the user configuration from EEPROM
    eeprom_read(0, (uint8_t *)&user_config, sizeof(user_config_t));

    // Verify the user configuration
    if (user_config.crc32 != user_config_crc32())
        // The user configuration is invalid, reset it to the default
        user_config_reset();
    else if (user_config.version != USER_CONFIG_VERSION)
        // TODO: Handle version migration
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

uint8_t user_config_tap_hold(void) { return user_config.tap_hold; }

uint8_t user_config_current_profile(void) {
    return user_config.current_profile;
}

void user_config_set_current_profile(uint8_t profile) {
    user_config.current_profile = profile;
    user_config_save_crc32();
    eeprom_write(offsetof(user_config_t, current_profile),
                 (uint8_t *)&user_config.current_profile,
                 sizeof(user_config.current_profile));
}

key_config_t *user_config_key_config(uint8_t profile, uint16_t index) {
    return &user_config.key_config[profile][index];
}

uint16_t user_config_keymap(uint8_t profile, uint8_t layer, uint16_t index) {
    return user_config.keymap[profile][layer][index];
}

dynamic_keystroke_config_t *
user_config_dynamic_keystroke_config(uint8_t index) {
    return &user_config.dynamic_keystroke_config[index];
}