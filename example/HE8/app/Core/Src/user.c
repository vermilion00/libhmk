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

/* clang-format off
 _  _  ____  ____
/ )( \(  __)/ _  \
) __ ( ) _) ) _  (
\_)(_/(____)\____/

clang-format on */

#include "adc.h"
#include "keycodes.h"
#include "main.h"
#include "switches.h"
#include "user_config.h"

// TMUX1308

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim10;

static const uint16_t select_pins[] = {
    MUX_SELECT_0_Pin,
    MUX_SELECT_1_Pin,
    MUX_SELECT_2_Pin,
};

static GPIO_TypeDef *select_ports[] = {
    MUX_SELECT_0_GPIO_Port,
    MUX_SELECT_1_GPIO_Port,
    MUX_SELECT_2_GPIO_Port,
};

static const uint16_t matrix[] = {
    1, 2, 3, 0, 7, 4, 6, 5,
};

//--------------------------------------------------------------------+
// User Implementations
//--------------------------------------------------------------------+

void adc_init(void) { HAL_ADC_Start_IT(&hadc1); }

uint16_t calibration_initial_min(uint8_t sw_id) { return 2300; }

uint16_t calibration_delta(uint8_t sw_id) { return 700; }

//--------------------------------------------------------------------+
// HAL Callbacks
//--------------------------------------------------------------------+

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    static uint8_t mux_index = 0;

    if (hadc == &hadc1) {
        // Invert the ADC value since we use voltage followers
        store_adc_value(matrix[mux_index], 4095 - HAL_ADC_GetValue(&hadc1));

        mux_index = (mux_index + 1) & 0x7;
        for (uint32_t i = 0; i < 3; i++)
            HAL_GPIO_WritePin(select_ports[i], select_pins[i],
                              (GPIO_PinState)((mux_index >> i) & 1));

        // Delay before the next ADC conversion to allow the voltage to settle
        HAL_TIM_Base_Start_IT(&htim10);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim10) {
        HAL_TIM_Base_Stop_IT(&htim10);
        HAL_ADC_Start_IT(&hadc1);
    }
}

//--------------------------------------------------------------------+
// Default User Configuration
//--------------------------------------------------------------------+

#define NM                                                                     \
    {                                                                          \
        .tapping_term = 200, .mode = KEY_MODE_NORMAL,                          \
        .nm = {                                                                \
            .actuation_distance = 40,                                          \
            .bottom_out_distance = 68,                                         \
        },                                                                     \
    }
#define RT                                                                     \
    {                                                                          \
        .tapping_term = 200, .mode = KEY_MODE_RAPID_TRIGGER,                   \
        .rt = {                                                                \
            .actuation_distance = 2,                                           \
            .reset_distance = 2,                                               \
            .rt_down_distance = 2,                                             \
            .rt_up_distance = 2,                                               \
        },                                                                     \
    }
#define DIGITAL_KEY_CONFIG                                                     \
    { NM, NM, NM, NM, NM, NM, NM, NM, }
#define ANALOG_KEY_CONFIG                                                      \
    { NM, RT, NM, NM, RT, RT, RT, NM, }

#define LT_VOLD LT(1, KC_VOLD)
#define MGC_BTL SP_MAGIC_BOOTLOADER
#define MGC_RBT SP_MAGIC_REBOOT
#define MGC_FAC SP_MAGIC_FACTORY_RESET
#define MGC_REC SP_MAGIC_RECALIBRATE
#define DEFAULT_KEYMAP                                                         \
    {                                                                          \
        {                                                                      \
            KC_MPLY, KC_UP,   KC_PSCR, KC_VOLU,                                \
            KC_LEFT, KC_DOWN, KC_RGHT, LT_VOLD,                                \
        },                                                                     \
            {                                                                  \
                PTO(0),  PTO(1),  PTO(2),  PTO(3),                             \
                MGC_BTL, MGC_RBT, MGC_FAC, MGC_REC,                            \
            },                                                                 \
            {                                                                  \
                _______, _______, _______, _______,                            \
                _______, _______, _______, _______,                            \
            },                                                                 \
            {                                                                  \
                _______, _______, _______, _______,                            \
                _______, _______, _______, _______,                            \
            },                                                                 \
    }

const user_config_t default_user_config = {
    .crc32 = 0,
    .version = USER_CONFIG_VERSION,

    .sw_id = SW_GATERON_MAGNETIC_JADE,
    .tap_hold = TAP_HOLD_HOLD_ON_OTHER_KEY_PRESS,
    .current_profile = 0,

    .key_config =
        {
            DIGITAL_KEY_CONFIG,
            ANALOG_KEY_CONFIG,
            ANALOG_KEY_CONFIG,
            ANALOG_KEY_CONFIG,
        },
    .keymap =
        {
            DEFAULT_KEYMAP,
            DEFAULT_KEYMAP,
            DEFAULT_KEYMAP,
            DEFAULT_KEYMAP,
        },
};
