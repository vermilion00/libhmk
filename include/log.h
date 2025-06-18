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

#if defined(LOG_ENABLED)
//--------------------------------------------------------------------+
// Log Configuration
//--------------------------------------------------------------------+

#if !defined(LOG_BUFFER_SIZE)
// Buffer for log messages. This should be large enough to hold the
// messages that will be sent through WebUSB.
#define LOG_BUFFER_SIZE 1024
#endif

//--------------------------------------------------------------------+
// Log Buffer
//--------------------------------------------------------------------+

// Null-terminated buffer for log messages
extern char log_buffer[LOG_BUFFER_SIZE];
// Size of the log buffer
extern uint32_t log_buffer_size;

//--------------------------------------------------------------------+
// Log API
//--------------------------------------------------------------------+

/**
 * @brief Initialize the log module
 *
 * @return None
 */
void log_init(void);

/**
 * @brief Log a format string to the buffer
 *
 * @param fmt Format string (printf-style)
 * @param ... Arguments for the format string
 *
 * @return None
 */
void log_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Clear the log buffer
 *
 * @return None
 */
void log_clear(void);
#else
#define log_init()
#define log_printf(fmt, ...)
#define log_clear()
#endif
