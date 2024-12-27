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

#include "eeprom.h"

#if defined(EEPROM_I2C)

#include <memory.h>

#include "stm32f4xx_hal.h"
#include "timer.h"

#if EEPROM_ADDR_SIZE == 1
typedef uint8_t eeprom_addr_t;
#define EEPROM_ADDR_SIZE_HAL I2C_MEMADD_SIZE_8BIT
#elif EEPROM_ADDR_SIZE == 2
typedef uint16_t eeprom_addr_t;
#define EEPROM_ADDR_SIZE_HAL I2C_MEMADD_SIZE_16BIT
#else
#error "Unsupported EEPROM_ADDR_SIZE"
#endif

extern I2C_HandleTypeDef EEPROM_I2C_HANDLE;

void eeprom_init(void) {
    // Nothing to do here
}

bool eeprom_read(uint32_t addr, uint8_t *data, uint32_t len) {
    if (HAL_I2C_Mem_Read(&EEPROM_I2C_HANDLE, EEPROM_ADDR(addr),
                         (eeprom_addr_t)addr, EEPROM_ADDR_SIZE_HAL, data, len,
                         1000) != HAL_OK)
        return false;

    for (uint32_t i = 0; i < len; i++)
        // Invert the data to simulate STM32 empty flash bits
        data[i] = ~data[i];

    return true;
}

bool eeprom_write(uint32_t addr, uint8_t *data, uint32_t len) {
    static uint8_t write_buf[EEPROM_PAGE_SIZE];

    while (len > 0) {
        uint32_t write_len = EEPROM_PAGE_SIZE - (addr % EEPROM_PAGE_SIZE);

        if (write_len > len)
            write_len = len;

        memcpy(write_buf, data, write_len);
        for (uint32_t i = 0; i < write_len; i++)
            // Invert the data to simulate STM32 empty flash bits
            write_buf[i] = ~write_buf[i];

        if (HAL_I2C_Mem_Write(&EEPROM_I2C_HANDLE, EEPROM_ADDR(addr),
                              (eeprom_addr_t)addr, EEPROM_ADDR_SIZE_HAL,
                              write_buf, write_len, 1000) != HAL_OK)
            return false;

        timer_delay(EEPROM_WRITE_TIME);

        addr += write_len;
        data += write_len;
        len -= write_len;
    }

    return true;
}

bool eeprom_erase(void) {
    static uint8_t erase_buf[EEPROM_PAGE_SIZE];

    memset(erase_buf, 0xFF, sizeof(erase_buf));

    for (uint32_t addr = 0; addr < EEPROM_BYTES; addr += EEPROM_PAGE_SIZE) {
        if (HAL_I2C_Mem_Write(&EEPROM_I2C_HANDLE, EEPROM_ADDR(addr),
                              (eeprom_addr_t)addr, EEPROM_ADDR_SIZE_HAL,
                              erase_buf, sizeof(erase_buf), 1000) != HAL_OK)
            return false;

        timer_delay(EEPROM_WRITE_TIME);
    }

    return true;
}

#endif
