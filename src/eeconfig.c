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

#include "eeconfig.h"

#include "advanced_keys.h"
#include "keycodes.h"
#include "layout.h"
#include "migration.h"

const eeconfig_t *eeconfig;

static eeconfig_profile_t default_profile = {
    .keymap = DEFAULT_KEYMAP,
    // `actuation_map` is filled in the initialization function.
    // `advanced_keys` is empty by default.
    // `gamepad_buttons` is empty by default.
    .gamepad_options = DEFAULT_GAMEPAD_OPTIONS,
    .tick_rate = DEFAULT_TICK_RATE,
};

static bool eeconfig_is_latest_version(void) {
  return eeconfig->magic_start == EECONFIG_MAGIC_START &&
         eeconfig->magic_end == EECONFIG_MAGIC_END &&
         eeconfig->version == EECONFIG_VERSION;
}

void eeconfig_init(void) {
  // Fill default profile with default actuation.
  for (uint32_t i = 0; i < NUM_KEYS; i++)
    default_profile.actuation_map[i] = (actuation_t)DEFAULT_ACTUATION;

  eeconfig = (const eeconfig_t *)wl_cache;
  if (!eeconfig_is_latest_version() && !migration_try_migrate())
    eeconfig_reset();
}

// Helper macro for writing rvalue
#define EECONFIG_WRITE_RVAL(field, value)                                      \
  do {                                                                         \
    typeof(((eeconfig_t *)0)->field) _value = value;                           \
    status &= EECONFIG_WRITE(field, &_value);                                  \
  } while (0)

bool eeconfig_reset(void) {
  bool status = true;
  advanced_key_clear();
  EECONFIG_WRITE_RVAL(magic_start, EECONFIG_MAGIC_START);
  EECONFIG_WRITE_RVAL(version, EECONFIG_VERSION);
  EECONFIG_WRITE_RVAL(calibration, DEFAULT_CALIBRATION);
  EECONFIG_WRITE_RVAL(options, DEFAULT_OPTIONS);
  EECONFIG_WRITE_RVAL(current_profile, 0);
  EECONFIG_WRITE_RVAL(last_non_default_profile, M_MIN(1, NUM_PROFILES - 1));
  for (uint32_t i = 0; i < NUM_PROFILES; i++)
    status &= EECONFIG_WRITE(profiles[i], &default_profile);
  EECONFIG_WRITE_RVAL(magic_end, EECONFIG_MAGIC_END);
  layout_load_advanced_keys();

  return status;
}

bool eeconfig_reset_profile(uint8_t profile) {
  if (profile >= NUM_PROFILES)
    return false;

  if (eeconfig->current_profile == profile)
    advanced_key_clear();
  const bool status = EECONFIG_WRITE(profiles[profile], &default_profile);
  if (eeconfig->current_profile == profile)
    layout_load_advanced_keys();

  return status;
}
