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

#include "layout.h"

#include "bitmap.h"
#include "eeconfig.h"
#include "hid.h"
#include "keycodes.h"
#include "matrix.h"

// Layer mask. Each bit represents whether a layer is active or not.
static uint16_t layer_mask;
static uint8_t default_layer;

// Whether the key is disabled by `SP_KEY_LOCK`
static bitmap_t key_disabled[] = MAKE_BITMAP(NUM_KEYS);
// Track whether the key is currently pressed. Used to detect key events.
static bitmap_t key_press_states[] = MAKE_BITMAP(NUM_KEYS);
// Store the keycodes of the currently pressed keys. Layer/profile may change so
// we need to remember the keycodes we pressed to release them correctly.
static uint8_t active_keycodes[NUM_KEYS];
// Only send reports if they changed
bool should_send_reports;

/**
 * @brief Get the current layer
 *
 * The current layer is the highest layer that is currently active in the layer
 * mask. If no layers are active, the default layer is returned.
 *
 * @return Current layer
 */
__attribute__((always_inline)) static inline uint8_t
layout_get_current_layer(void) {
  return layer_mask ? 31 - __builtin_clz(layer_mask) : default_layer;
}

__attribute__((always_inline)) static inline void
layout_layer_on(uint8_t layer) {
  layer_mask |= (1 << layer);
}

__attribute__((always_inline)) static inline void
layout_layer_off(uint8_t layer) {
  layer_mask &= ~(1 << layer);
}

__attribute__((always_inline)) static inline void layout_layer_lock(void) {
  const uint8_t current_layer = layout_get_current_layer();
  default_layer = current_layer == default_layer ? 0 : current_layer;
}

static uint8_t layout_get_keycode(uint8_t current_layer, uint8_t key) {
  // Find the first active layer with a non-transparent keycode
  for (uint32_t i = current_layer + 1; i-- > 0;) {
    if (((layer_mask >> i) & 1) == 0)
      // Layer is not active
      continue;

    const uint8_t keycode = CURRENT_PROFILE.keymap[i][key];
    if (keycode != KC_TRNS)
      return keycode;
  }

  // No keycode found, use the default keycode
  return CURRENT_PROFILE.keymap[default_layer][key];
}

void layout_init(void) {}

void layout_task(void) {
  const uint8_t current_layer = layout_get_current_layer();

  for (uint32_t i = 0; i < NUM_KEYS; i++) {
    const key_state_t *k = &key_matrix[i];
    const bool last_key_press = bitmap_get(key_press_states, i);

    if ((current_layer == 0) & bitmap_get(key_disabled, i))
      // Only keys in layer 0 can be disabled.
      continue;

    if (k->is_pressed & !last_key_press) {
      // Key press event
      const uint8_t keycode = layout_get_keycode(current_layer, i);
      active_keycodes[i] = keycode;

      layout_ll_press(i, keycode);
    } else if (!k->is_pressed & last_key_press) {
      // Key release event
      const uint8_t keycode = active_keycodes[i];
      active_keycodes[i] = KC_NO;

      layout_ll_release(i, keycode);
    } else if (k->is_pressed) {
      // Key hold event
    }

    // Finally, update the key state
    bitmap_set(key_press_states, i, k->is_pressed);
  }

  if (should_send_reports) {
    hid_send_reports();
    should_send_reports = false;
  }
}

void layout_ll_press(uint8_t key, uint8_t keycode) {
  if (keycode == KC_NO)
    return;

  switch (keycode) {
  case HID_KEYCODE_RANGE:
    hid_keycode_add(keycode);
    should_send_reports = true;
    break;

  case MOMENTARY_LAYER_RANGE:
    layout_layer_on(MO_GET_LAYER(keycode));
    break;

  case PROFILE_RANGE:
    // TODO: Implement profile switching
    break;

  case SP_KEY_LOCK:
    bitmap_toggle(key_disabled, key);
    break;

  case SP_LAYER_LOCK:
    layout_layer_lock();
    break;

  case SP_PROFILE_SWAP:
    // TODO: Implement profile swapping
    break;

  case SP_PROFILE_NEXT:
    // TODO: Implement profile switching
    break;

  default:
    break;
  }
}

void layout_ll_release(uint8_t key, uint8_t keycode) {
  if (keycode == KC_NO)
    return;

  switch (keycode) {
  case HID_KEYCODE_RANGE:
    hid_keycode_remove(keycode);
    should_send_reports = true;
    break;

  case MOMENTARY_LAYER_RANGE:
    layout_layer_off(MO_GET_LAYER(keycode));
    break;

  default:
    break;
  }
}
