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

#include "deferred_actions.h"

#include "layout.h"

// Lock for the deferred action stack
static bool stack_lock;

// Deferred action stack
static uint32_t stack_size;
static deferred_action_t stack[MAX_DEFERRED_ACTIONS];

static void deferred_action_execute(const deferred_action_t *action) {
  static deferred_action_t tap_release = {0};

  switch (action->type) {
  case DEFERRED_ACTION_TYPE_PRESS:
    layout_ll_press(action->key, action->keycode);
    break;

  case DEFERRED_ACTION_TYPE_RELEASE:
    layout_ll_release(action->key, action->keycode);
    break;

  case DEFERRED_ACTION_TYPE_TAP:
    tap_release = (deferred_action_t){
        .type = DEFERRED_ACTION_TYPE_RELEASE,
        .key = action->key,
        .keycode = action->keycode,
    };
    if (deferred_action_push(&tap_release))
      // We only perform the tap action if the release action was successfully
      // enqueued. Otherwise, the key will be stuck in the pressed state.
      layout_ll_press(action->key, action->keycode);
    break;

  default:
    break;
  }
}

void deferred_action_init(void) {}

bool deferred_action_push(const deferred_action_t *action) {
  if (stack_lock || stack_size == MAX_DEFERRED_ACTIONS)
    return false;

  stack_lock = true;
  stack[stack_size++] = *action;
  stack_lock = false;

  return true;
}

void deferred_action_process(void) {
  static deferred_action_t buffer[MAX_DEFERRED_ACTIONS];

  if (stack_lock || stack_size == 0)
    return;

  stack_lock = true;
  // Copy all the actions in the stack to a buffer to avoid the stack being
  // locked while executing those actions
  const uint32_t prev_stack_size = stack_size;
  for (uint32_t i = 0; i < stack_size; i++)
    buffer[i] = stack[i];
  // Clear the stack
  stack_size = 0;
  stack_lock = false;

  // Execute all the actions
  for (uint32_t i = 0; i < prev_stack_size; i++)
    deferred_action_execute(&buffer[i]);
}
