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

//---------------------------------------------------------------------+
// Migration Function Types
//---------------------------------------------------------------------+

typedef struct {
  // Persistent configuration version
  uint16_t version;
  // Function to migrate the persistent configuration from the previous version.
  // The function should migrate `src` to `dst` without having to write to the
  // flash. The function should return true if the migration was successful,
  // false otherwise. If the function is NULL, no migration is performed for
  // that version.
  bool (*migrate)(void *dst, const void *src);
} migration_func_t;

//--------------------------------------------------------------------+
// Persistent Configuration Migration API
//--------------------------------------------------------------------+

/**
 * @brief Try to migrate the persistent configuration to the latest format
 *
 * @return true if migration was successful, false otherwise
 */
bool migration_try_migrate(void);
