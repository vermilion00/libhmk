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

#include "migration.h"

#include "eeconfig.h"
#include "wear_leveling.h"

static const migration_func_t migration_funcs[] = {
    {0x0101, NULL},
};

bool migration_try_migrate(void) {
  if (eeconfig->magic_start != EECONFIG_MAGIC_START)
    // The magic start is always the same for any version.
    return false;

  const uint16_t config_version = eeconfig->version;
  for (uint32_t i = 0; i < M_ARRAY_SIZE(migration_funcs); i++) {
    if (migration_funcs[i].version <= config_version)
      // Older versions
      continue;
    if (migration_funcs[i].migrate != NULL && !migration_funcs[i].migrate())
      // Migration failed
      return false;
  }

  // `eeconfig` is updated, so we reflect the update in the flash.
  wear_leveling_write(0, eeconfig, sizeof(eeconfig_t));

  return true;
}