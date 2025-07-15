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

#include "log.h"

#if defined(LOG_ENABLED)
#include "printf/printf.h"
#include "tusb.h"
#include "usb_descriptors.h"

uint32_t log_buffer_size;
char log_buffer[LOG_EP_SIZE];

/**
 * @brief Flush the log buffer if the buffer is not empty
 *
 * @return None
 */
static void log_flush(void) {
  if (log_buffer_size == 0)
    return;

  while (!tud_hid_n_ready(USB_ITF_LOG))
    // Wait for the log interface to be ready
    tud_task();
  tud_hid_n_report(USB_ITF_LOG, 0, log_buffer, LOG_EP_SIZE);

  log_buffer_size = 0;
  memset(log_buffer, 0, LOG_EP_SIZE);
}

void log_init(void) {}

int log_printf(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  const int ret = vprintf_(fmt, args);
  va_end(args);

  return ret;
}

void log_task(void) { log_flush(); }

//--------------------------------------------------------------------+
// Printf Backend
//--------------------------------------------------------------------+

void putchar_(char c) {
  if (log_buffer_size >= LOG_EP_SIZE)
    // Buffer is full, flush it
    log_flush();
  log_buffer[log_buffer_size++] = c;
}
#endif