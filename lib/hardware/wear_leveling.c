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

#include "wear_leveling.h"

#if defined(EEPROM_WEAR_LEVELING)

#include <memory.h>

#include "crc32.h"

// STM32F446RE flash sector sizes
static const uint32_t flash_sector_sizes[] = {
    16 * 1024, 16 * 1024,  16 * 1024,  16 * 1024,
    64 * 1024, 128 * 1024, 128 * 1024, 128 * 1024,
};

_Static_assert(sizeof(flash_sector_sizes) / sizeof(flash_sector_sizes[0]) ==
                   NUM_FLASH_SECTORS,
               "Invalid number of flash sectors");

static uint32_t starting_flash_sector_num;
static uint32_t flash_base_address;

/**
 * @brief Initialize the flash memory
 *
 * @return none
 */
static void flash_init(void) {
    uint32_t reserved_flash_size = 0;
    for (uint32_t i = NUM_FLASH_SECTORS; i-- >= 0;) {
        reserved_flash_size += flash_sector_sizes[i];
        if (reserved_flash_size >= EEPROM_BACKING_STORE_SIZE) {
            starting_flash_sector_num = i;
            break;
        }
    }

    flash_base_address = FLASH_BASE + FLASH_SIZE - reserved_flash_size;
}

/**
 * @brief Read data from the flash memory
 *
 * @param addr Memory address to read from
 * @param data Pointer to the buffer to store the data
 * @param len Number of words to read
 *
 * @return true if successful, false otherwise
 */
static bool flash_read(uint32_t addr, uint32_t *data, uint32_t len) {
    if (addr + len > flash_base_address + EEPROM_BACKING_STORE_SIZE)
        // Out of bounds
        return false;

    for (uint32_t i = 0; i < len; i++)
        // Invert the data since STM32 empty flash bits are 1
        data[i] =
            ~(*(uint32_t *)(flash_base_address + addr + i * FLASH_WRITE_SIZE));

    return true;
}

/**
 * @brief Write data to the flash memory
 *
 * This function may fail if the flash memory is not erased before writing.
 *
 * @param addr Memory address to write to
 * @param data Pointer to the data to write
 * @param len Number of words to write
 *
 * @return true if successful, false otherwise
 */
static bool flash_write(uint32_t addr, uint32_t *data, uint32_t len) {
    if (addr + len > flash_base_address + EEPROM_BACKING_STORE_SIZE)
        // Out of bounds
        return false;

    if (HAL_FLASH_Unlock() != HAL_OK)
        return false;

    for (uint32_t i = 0; i < len; i++) {
        if (HAL_FLASH_Program(FLASH_PROGRAM_TYPE,
                              flash_base_address + addr + i * FLASH_WRITE_SIZE,
                              ~data[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    if (HAL_FLASH_Lock() != HAL_OK)
        return false;

    return true;
}

/**
 * @brief Erase the flash memory used by the EEPROM
 *
 * @return true if successful, false otherwise
 */
static bool flash_erase(void) {
    if (HAL_FLASH_Unlock() != HAL_OK)
        return false;

    for (uint32_t i = starting_flash_sector_num; i < NUM_FLASH_SECTORS; i++) {
        FLASH_Erase_Sector(i, VOLTAGE_RANGE);
        if (FLASH_WaitForLastOperation(50000) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    if (HAL_FLASH_Lock() != HAL_OK)
        return false;

    return true;
}

static __attribute__((aligned(FLASH_WRITE_SIZE))) uint8_t cache[EEPROM_BYTES];
static uint32_t write_address;

/**
 * @brief Clear the EEPROM cache
 *
 * @return none
 */
static void eeprom_clear_cache(void) {
    memset(cache, 0, sizeof(cache));
    // Skip the first 4 bytes for CRC32
    write_address = EEPROM_BYTES + 4;
}

/**
 * @brief Read the consolidated data into the cache
 *
 * This function also verifies the CRC32 checksum. If the function fails,
 * the cache is cleared.
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_read_consolidated(void) {
    uint8_t status = WEAR_LEVELING_SUCCESS;

    if (!flash_read(0, (uint32_t *)cache, EEPROM_BYTES / FLASH_WRITE_SIZE))
        status = WEAR_LEVELING_FAILED;

    if (status != WEAR_LEVELING_FAILED) {
        const uint32_t expected = crc32_bitwise(0, cache, EEPROM_BYTES);
        uint32_t actual;

        if (!flash_read(EEPROM_BYTES, &actual, 1) || expected != actual)
            status = WEAR_LEVELING_FAILED;
    }

    if (status == WEAR_LEVELING_FAILED)
        eeprom_clear_cache();

    return status;
}

/**
 * @brief Write the cache to the flash memory in the consolidated area
 *
 * This function does not clear the write log. It should only be called after
 * the flash memory is erased.
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_write_consolidated(void) {
    uint8_t status = WEAR_LEVELING_CONSOLIDATED;

    if (!flash_write(0, (uint32_t *)cache, EEPROM_BYTES / FLASH_WRITE_SIZE))
        status = WEAR_LEVELING_FAILED;

    if (status != WEAR_LEVELING_FAILED) {
        // Calculate CRC32 checksum and write it to the end of the EEPROM
        uint32_t crc = crc32_bitwise(0, cache, EEPROM_BYTES);
        if (!flash_write(EEPROM_BYTES, &crc, 1))
            status = WEAR_LEVELING_FAILED;
    }

    return status;
}

/**
 * @brief Force consolidation of the cache into the flash memory
 *
 * This function will erase the flash memory including the write log and write
 * the cache to the flash memory.
 *
 * WARNING: Data may be lost if power is lost during this operation.
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_consolidate_force(void) {
    if (!flash_erase())
        return WEAR_LEVELING_FAILED;

    const uint8_t status = eeprom_write_consolidated();
    write_address = EEPROM_BYTES + 4;

    return status;
}

/**
 * @brief Consolidate the cache into the flash memory if the write log is full
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_consolidate_if_needed(void) {
    if (write_address >= EEPROM_BACKING_STORE_SIZE)
        return eeprom_consolidate_force();

    return WEAR_LEVELING_SUCCESS;
}

/**
 * @brief Replay the EEPROM log from the flash memory into the cache
 *
 * @return Waer leveling status
 */
static uint8_t eeprom_replay_log(void) {
    uint8_t status = WEAR_LEVELING_SUCCESS;
    // Skip the first 4 bytes for CRC32
    uint32_t address = EEPROM_BYTES + 4;

    while (address < EEPROM_BACKING_STORE_SIZE) {
        uint32_t value;

        if (!flash_read(address, &value, 1)) {
            status = WEAR_LEVELING_FAILED;
            break;
        }

        if (value == 0)
            // End of log
            break;

        // Increment address after we check for the end of log so that the
        // address points to the end of log
        address += FLASH_WRITE_SIZE;

        eeprom_write_log_t entry;
        entry.u32[0] = value;

        const uint8_t len = LOG_ENTRY_GET_LENGTH(entry);
        const uint32_t addr = LOG_ENTRY_GET_ADDRESS(entry);

        if (addr + len > EEPROM_BYTES) {
            status = WEAR_LEVELING_FAILED;
            break;
        }

        if (len > 1) {
            // Read additional word if necessary
            if (!flash_read(address, &entry.u32[1], 1)) {
                status = WEAR_LEVELING_FAILED;
                break;
            }
            address += FLASH_WRITE_SIZE;
        }

        memcpy(&cache[addr], &entry.u8[3], len);
    }

    write_address = address;
    if (status == WEAR_LEVELING_FAILED)
        // Force consolidation if replay failed
        return eeprom_consolidate_force();
    else
        return eeprom_consolidate_if_needed();
}

/**
 * @brief Append data to the write log
 *
 * @param value Data to append
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_append(uint32_t value) {
    if (!flash_write(write_address, &value, 1))
        return WEAR_LEVELING_FAILED;

    write_address += FLASH_WRITE_SIZE;
    return eeprom_consolidate_if_needed();
}

void eeprom_init(void) {
    eeprom_clear_cache();
    flash_init();

    if (eeprom_read_consolidated())
        eeprom_replay_log();
    else
        // If the consolidated data is corrupted, erase the flash memory
        // and write the cache to the flash memory
        eeprom_consolidate_force();
}

bool eeprom_read(uint32_t addr, uint8_t *data, uint32_t len) {
    if (addr + len > EEPROM_BYTES)
        return false;

    memcpy(data, &cache[addr], len);
    return true;
}

/**
 * @brief Write raw data to the EEPROM
 *
 * This function should only be called after the cache has been updated.
 *
 * @param addr Memory address to write to
 * @param data Pointer to the data to write
 * @param len Number of bytes to write
 *
 * @return Wear leveling status
 */
static uint8_t eeprom_write_raw(uint32_t addr, uint8_t *data, uint32_t len) {
    while (len > 0) {
        const uint32_t write_len =
            len > MAX_BYTES_PER_LOG_ENTRY ? MAX_BYTES_PER_LOG_ENTRY : len;

        eeprom_write_log_t entry = LOG_ENTRY_MAKE(addr, write_len);
        for (uint32_t i = 0; i < write_len; i++)
            entry.u8[3 + i] = data[i];

        uint8_t status = eeprom_append(entry.u32[0]);
        if (status != WEAR_LEVELING_SUCCESS)
            // If we consolidate here, no need to write any more data.
            return status;

        if (write_len > 1) {
            status = eeprom_append(entry.u32[1]);
            if (status != WEAR_LEVELING_SUCCESS)
                // If we consolidate here, no need to write any more data.
                return status;
        }

        addr += write_len;
        data += write_len;
        len -= write_len;
    }

    return WEAR_LEVELING_SUCCESS;
}

bool eeprom_write(uint32_t addr, uint8_t *data, uint32_t len) {
    if (addr + len > EEPROM_BYTES)
        return false;

    if (memcmp(&cache[addr], data, len) == 0)
        // No change
        return true;

    memcpy(&cache[addr], data, len);

    uint8_t status = eeprom_write_raw(addr, data, len);
    if (status == WEAR_LEVELING_SUCCESS)
        return eeprom_consolidate_if_needed() != WEAR_LEVELING_FAILED;

    return true;
}

bool eeprom_erase(void) {
    eeprom_clear_cache();
    return eeprom_consolidate_force() == WEAR_LEVELING_SUCCESS;
}

#endif