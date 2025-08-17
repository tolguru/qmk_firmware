/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include "keychron_common.h"
#include "keychron_rgb_type.h"
#include "eeconfig_kb.h"
#include "usb_main.h"
#include "color.h"
#ifdef LK_WIRELESS_ENABLE
#include "transport.h"
#endif
#include <lib/lib8tion/lib8tion.h>

#if defined(KEYCHRON_RGB_ENABLE) && defined(EECONFIG_SIZE_CUSTOM_RGB)

#    define PER_KEY_RGB_VER 0x0001

#    define OFFSET_OS_INDICATOR ((uint8_t *)(EECONFIG_BASE_CUSTOM_RGB))
#    define OFFSET_RETAIL_DEMO  (OFFSET_OS_INDICATOR + sizeof(os_indicator_config_t))
#    define OFFSET_PER_KEY_RGB_TYPE (OFFSET_RETAIL_DEMO + sizeof(retail_demo_enable))
#    define OFFSET_PER_KEY_RGBS (OFFSET_PER_KEY_RGB_TYPE + sizeof(per_key_rgb_type))
#    define OFFSET_LAYER_FLAGS (OFFSET_PER_KEY_RGBS + sizeof(per_key_led))
#    define OFFSET_EFFECT_LIST (OFFSET_LAYER_FLAGS + sizeof(regions))

enum {
    RGB_GET_PROTOCOL_VER = 0x01,
    RGB_SAVE,
    GET_INDICATORS_CONFIG,
    SET_INDICATORS_CONFIG,
    RGB_GET_LED_COUNT,
    RGB_GET_LED_IDX,
    PER_KEY_RGB_GET_TYPE,
    PER_KEY_RGB_SET_TYPE,
    PER_KEY_RGB_GET_COLOR,
    PER_KEY_RGB_SET_COLOR,  //10
    MIXED_EFFECT_RGB_GET_INFO,
    MIXED_EFFECT_RGB_GET_REGIONS,
    MIXED_EFFECT_RGB_SET_REGIONS,
    MIXED_EFFECT_RGB_GET_EFFECT_LIST,
    MIXED_EFFECT_RGB_SET_EFFECT_LIST,
};

extern uint8_t  retail_demo_enable;
extern uint8_t         per_key_rgb_type;
extern HSV             per_key_led[RGB_MATRIX_LED_COUNT];
extern HSV default_per_key_led[RGB_MATRIX_LED_COUNT];

extern uint8_t         regions[RGB_MATRIX_LED_COUNT];
extern uint8_t         rgb_regions[RGB_MATRIX_LED_COUNT];
extern effect_config_t effect_list[EFFECT_LAYERS][EFFECTS_PER_LAYER];
extern uint8_t default_region[RGB_MATRIX_LED_COUNT];

os_indicator_config_t os_ind_cfg;

extern void update_mixed_rgb_effect_count(void);

void eeconfig_reset_custom_rgb(void) {
    os_ind_cfg.disable.raw = 0;
    os_ind_cfg.hsv.s = 0;
    os_ind_cfg.hsv.h = os_ind_cfg.hsv.v = 0xFF;

    eeprom_update_block(&os_ind_cfg, OFFSET_OS_INDICATOR, sizeof(os_ind_cfg));
    retail_demo_enable = 0;
    eeprom_read_block(&retail_demo_enable, (uint8_t *)(OFFSET_RETAIL_DEMO), sizeof(retail_demo_enable));
    per_key_rgb_type = 0;
    eeprom_update_block(&per_key_rgb_type, OFFSET_PER_KEY_RGB_TYPE, sizeof(per_key_rgb_type));

    memcpy(per_key_led, default_per_key_led, sizeof(per_key_led));
    eeprom_update_block(per_key_led, OFFSET_PER_KEY_RGBS, sizeof(per_key_led));

    memcpy(regions, default_region, RGB_MATRIX_LED_COUNT);
    eeprom_update_block(regions, OFFSET_LAYER_FLAGS, sizeof(regions));

    memset(effect_list, 0, sizeof(effect_list));

    effect_list[0][0].effect = 5;
    effect_list[0][0].sat = 255;
    effect_list[0][0].speed = 127;
    effect_list[0][0].time = 5000;

    effect_list[1][0].effect = 2;
    effect_list[1][0].hue = 0;
    effect_list[1][0].sat = 255;
    effect_list[1][0].speed = 127;
    effect_list[1][0].time = 5000;

    eeprom_update_block(effect_list, OFFSET_EFFECT_LIST, sizeof(effect_list));
    update_mixed_rgb_effect_count();
}

void eeconfig_init_custom_rgb(void) {
    memcpy(per_key_led, default_per_key_led, sizeof(per_key_led));
    eeprom_update_dword(EECONFIG_KEYBOARD, (EECONFIG_KB_DATA_VERSION));

    eeprom_read_block(&os_ind_cfg, OFFSET_OS_INDICATOR, sizeof(os_ind_cfg));
    eeprom_read_block(&retail_demo_enable, (uint8_t *)(OFFSET_RETAIL_DEMO), sizeof(retail_demo_enable));

    if (os_ind_cfg.hsv.v < 128) os_ind_cfg.hsv.v = 128;
    // Load per key rgb led
    eeprom_read_block(&per_key_rgb_type, OFFSET_PER_KEY_RGB_TYPE, sizeof(per_key_rgb_type));
    eeprom_read_block(per_key_led, OFFSET_PER_KEY_RGBS, sizeof(per_key_led));
    // Load mixed rgb
    eeprom_read_block(regions, OFFSET_LAYER_FLAGS, sizeof(regions));
    eeprom_read_block(effect_list, OFFSET_EFFECT_LIST, sizeof(effect_list));
    update_mixed_rgb_effect_count();

}

void rgb_save_retail_demo(void) {
    eeprom_update_block(&retail_demo_enable, (uint8_t *)(OFFSET_RETAIL_DEMO), sizeof(retail_demo_enable));
}

static bool rgb_get_version(uint8_t *data) {
    data[1] = PER_KEY_RGB_VER & 0xFF;
    data[2] = (PER_KEY_RGB_VER >> 8) & 0xFF;

    return true;
}

static bool rgb_get_led_count(uint8_t *data) {
    data[1] = RGB_MATRIX_LED_COUNT;

    return true;
}

static bool rgb_get_led_idx(uint8_t *data) {
    uint8_t row = data[0];
    if (row > MATRIX_ROWS) return false;

    uint8_t  led_idx[128];
    uint32_t row_mask = 0;
    memcpy(&row_mask, &data[1], 3);

    for (uint8_t c = 0; c < MATRIX_COLS; c++) {
        led_idx[0] = 0xFF;
        if (row_mask & (0x01 << c)) {
            rgb_matrix_map_row_column_to_led(row, c, led_idx);
        }
        data[1 + c] = led_idx[0];
    }

    return true;
}

static bool per_key_rgb_get_type(uint8_t *data) {
    extern uint8_t per_key_rgb_type;
    data[1] = per_key_rgb_type;

    return true;
}

static bool per_key_rgb_set_type(uint8_t *data) {
    uint8_t type = data[0];

    if (type >= PER_KEY_RGB_MAX) return false;

    per_key_rgb_type = data[0];

    return true;
}

static bool per_key_rgb_get_led_color(uint8_t *data) {
    uint8_t start = data[0];
    uint8_t count = data[1];

    if (count > 9) return false;

    for (uint8_t i = 0; i < count; i++) {
        data[1 + i * 3] = per_key_led[start + i].h;
        data[2 + i * 3] = per_key_led[start + i].s;
        data[3 + i * 3] = per_key_led[start + i].v;
    }

    return true;
}

static bool per_key_rgb_set_led_color(uint8_t *data) {
    uint8_t start = data[0];
    uint8_t count = data[1];

    if (count > 9) return false;

    for (uint8_t i = 0; i < count; i++) {
        per_key_led[start + i].h = data[2 + i * 3];
        per_key_led[start + i].s = data[3 + i * 3];
        per_key_led[start + i].v = data[4 + i * 3];
    }

    return true;
}

static bool mixed_rgb_get_effect_info(uint8_t *data) {
    data[1] = EFFECT_LAYERS;
    data[2] = EFFECTS_PER_LAYER;

    return true;
}

static bool mixed_rgb_get_regions(uint8_t *data) {
    uint8_t start = data[0];
    uint8_t count = data[1];

    if (count > 29 || start + count > RGB_MATRIX_LED_COUNT) return false;
    memcpy(&data[1], &regions[start], count);

    return true;
}

bool mixed_rgb_set_regions(uint8_t *data) {
    uint8_t start = data[0];
    uint8_t count = data[1];

    if (count > 28 || start + count > RGB_MATRIX_LED_COUNT) return false;
    for (uint8_t i = 0; i < count; i++)
        if (data[2 + i] >= EFFECT_LAYERS) return false;

    memcpy(&regions[start], &data[2], count);
    memcpy(&rgb_regions[start], &data[2], count);

    return true;
}
#define EFFECT_DATA_LEN  8

static bool mixed_rgb_get_effect_list(uint8_t *data) {
    uint8_t region = data[0];
    uint8_t start  = data[1];
    uint8_t count  = data[2];

    if (count > 3 || region > EFFECT_LAYERS || start + count > EFFECTS_PER_LAYER) return false;

    for (uint8_t i = 0; i < count; i++) {
        data[1 + i * EFFECT_DATA_LEN] = effect_list[region][start + i].effect;
        data[2 + i * EFFECT_DATA_LEN] = effect_list[region][start + i].hue;
        data[3 + i * EFFECT_DATA_LEN] = effect_list[region][start + i].sat;
        data[4 + i * EFFECT_DATA_LEN] = effect_list[region][start + i].speed;
        memcpy(&data[5 + i * EFFECT_DATA_LEN], &effect_list[region][start + i].time, 4);
    }

    return true;
}

bool mixed_rgb_set_effect_list(uint8_t *data) {
    uint8_t region = data[0];
    uint8_t start  = data[1];
    uint8_t count  = data[2];

    if (count > 3 || region > EFFECT_LAYERS || start + count > EFFECTS_PER_LAYER) return false;
    for (uint8_t i = 0; i < count; i++) {
        if (data[3 + i * EFFECT_DATA_LEN] >= RGB_MATRIX_CUSTOM_MIXED_RGB) return false;
    }

    for (uint8_t i = 0; i < count; i++) {
        effect_list[region][start + i].effect = data[3 + i * EFFECT_DATA_LEN];
        effect_list[region][start + i].hue = data[4 + i * EFFECT_DATA_LEN];
        effect_list[region][start + i].sat = data[5 + i * EFFECT_DATA_LEN];
        effect_list[region][start + i].speed = data[6 + i * EFFECT_DATA_LEN];
        memcpy(&effect_list[region][start + i].time, &data[7 + i * EFFECT_DATA_LEN], 4);
    }
    update_mixed_rgb_effect_count();

    return true;
}

static bool kc_rgb_save(void) {
    eeprom_update_block(&os_ind_cfg, OFFSET_OS_INDICATOR, sizeof(os_ind_cfg));
    eeprom_update_block(&per_key_rgb_type, OFFSET_PER_KEY_RGB_TYPE, sizeof(per_key_rgb_type));
    eeprom_update_block(per_key_led, OFFSET_PER_KEY_RGBS, RGB_MATRIX_LED_COUNT * sizeof(rgb_led_t));
    eeprom_update_block(regions, OFFSET_LAYER_FLAGS, RGB_MATRIX_LED_COUNT);
    eeprom_update_block(effect_list, OFFSET_EFFECT_LIST, sizeof(effect_list));

    return true;
}

static bool get_indicators_config(uint8_t *data) {
    data[1] = 0
#if defined(NUM_LOCK_INDEX) && !defined(DIM_NUM_LOCK)
    | (1 << 0x00)
#endif
#if defined(CAPS_LOCK_INDEX) && !defined(DIM_CAPS_LOCK)
    | (1 << 0x01)
#endif
#if defined(SCROLL_LOCK_INDEX)
    | (1 << 0x02)
#endif
#if defined(COMPOSE_LOCK_INDEX)
    | (1 << 0x03)
#endif
#if defined(KANA_LOCK_INDEX)
    | (1 << 0x04)
#endif
;
    data[2] = os_ind_cfg.disable.raw;
    data[3] = os_ind_cfg.hsv.h;
    data[4] = os_ind_cfg.hsv.s;
    data[5] = os_ind_cfg.hsv.v;

    return true;
}

static bool set_indicators_config(uint8_t *data) {
    os_ind_cfg.disable.raw = data[0];
    os_ind_cfg.hsv.h = data[1];
    os_ind_cfg.hsv.s = data[2];
    os_ind_cfg.hsv.v = data[3];

    if (os_ind_cfg.hsv.v < 128) os_ind_cfg.hsv.v = 128;
    led_update_kb(host_keyboard_led_state());

    return true;
}

void kc_rgb_matrix_rx(uint8_t *data, uint8_t length) {
    uint8_t cmd     = data[1];
    bool    success = true;

    switch (cmd) {
        case RGB_GET_PROTOCOL_VER:
            success = rgb_get_version(&data[2]);
            break;

        case RGB_SAVE:
            success = kc_rgb_save();
            break;

        case GET_INDICATORS_CONFIG:
            success = get_indicators_config(&data[2]);
            break;

        case SET_INDICATORS_CONFIG:
            success = set_indicators_config(&data[2]);
            break;

        case RGB_GET_LED_COUNT:
            success = rgb_get_led_count(&data[2]);
            break;

        case RGB_GET_LED_IDX:
            success = rgb_get_led_idx(&data[2]);
            break;

        case PER_KEY_RGB_GET_TYPE:
            success = per_key_rgb_get_type(&data[2]);
            break;

        case PER_KEY_RGB_SET_TYPE:
            success = per_key_rgb_set_type(&data[2]);
            break;

        case PER_KEY_RGB_GET_COLOR:
            success = per_key_rgb_get_led_color(&data[2]);
            break;

        case PER_KEY_RGB_SET_COLOR:
            success = per_key_rgb_set_led_color(&data[2]);
            break;

        case MIXED_EFFECT_RGB_GET_INFO:
            success = mixed_rgb_get_effect_info(&data[2]);
            break;

        case MIXED_EFFECT_RGB_GET_REGIONS:
            success = mixed_rgb_get_regions(&data[2]);
            break;

        case MIXED_EFFECT_RGB_SET_REGIONS:
            success = mixed_rgb_set_regions(&data[2]);
            break;

        case MIXED_EFFECT_RGB_GET_EFFECT_LIST:
            success = mixed_rgb_get_effect_list(&data[2]);
            break;

        case MIXED_EFFECT_RGB_SET_EFFECT_LIST:
            success = mixed_rgb_set_effect_list(&data[2]);
            break;

        default:
            data[0] = 0xFF;
            break;
    }

    data[2] = success ? 0 : 1;
}

void os_state_indicate(void) {
#    if defined(RGB_DISABLE_WHEN_USB_SUSPENDED) || defined(LED_DISABLE_WHEN_USB_SUSPENDED)
    if (get_transport() == TRANSPORT_USB && USB_DRIVER.state == USB_SUSPENDED) return;
#    endif

    RGB rgb = hsv_to_rgb(os_ind_cfg.hsv);

#    if defined(NUM_LOCK_INDEX)
    if (host_keyboard_led_state().num_lock && !os_ind_cfg.disable.num_lock) {
        rgb_matrix_set_color(NUM_LOCK_INDEX, rgb.r, rgb.g, rgb.b);
    }
#    endif
#    if defined(CAPS_LOCK_INDEX)
    if (host_keyboard_led_state().caps_lock && !os_ind_cfg.disable.caps_lock) {
        rgb_matrix_set_color(CAPS_LOCK_INDEX, rgb.r, rgb.g, rgb.b);
    }
#    endif
#    if defined(SCROLL_LOCK_INDEX)
    if (host_keyboard_led_state().compose && !os_ind_cfg.disable.scroll_lock) {
        rgb_matrix_set_color(SCROLL_LOCK_INDEX, rgb.r, rgb.g, rgb.b);
    }
#    endif
#    if defined(COMPOSE_LOCK_INDEX)
    if (host_keyboard_led_state().compose && !os_ind_cfg.disable.compose) {
        rgb_matrix_set_color(COMPOSE_LOCK_INDEX, rgb.r, rgb.g, rgb.b);
    }
#    endif
#    if defined(KANA_LOCK_INDEX)
    if (host_keyboard_led_state().kana && !os_ind_cfg.disable.kana) {
        rgb_matrix_set_color(KANA_LOCK_INDEX, rgb.r, rgb.g, rgb.b);
    }
#    endif
    (void)rgb;
}

bool process_record_keychron_rgb(uint16_t keycode, keyrecord_t *record) {
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_MIXED_RGB || rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_PER_KEY_RGB) {
        switch (keycode) {
            case RGB_HUI ... RGB_SAD:
                return false;

            case RGB_SPI:
                if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_MIXED_RGB) {
                    return false;
                } else {
                    rgb_matrix_config.speed = qadd8(rgb_matrix_config.speed, RGB_MATRIX_SPD_STEP);
                    eeprom_write_byte((uint8_t *)EECONFIG_RGB_MATRIX + offsetof(rgb_config_t, speed), rgb_matrix_config.speed);
                }
                break;
            case RGB_SPD:
                if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_MIXED_RGB) {
                    return false;
                } else {
                    rgb_matrix_config.speed = qsub8(rgb_matrix_config.speed, RGB_MATRIX_SPD_STEP);
                    eeprom_write_byte((uint8_t *)EECONFIG_RGB_MATRIX + offsetof(rgb_config_t, speed), rgb_matrix_config.speed);
                }
                break;

            case RGB_VAI:
#    ifdef RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL
                if (!rgb_matrix_config.enable) {
                    rgb_matrix_toggle();
                    return false;
                }
#    endif
                rgb_matrix_config.hsv.v = qadd8(rgb_matrix_config.hsv.v, RGB_MATRIX_VAL_STEP);
#    ifdef RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL
                while (rgb_matrix_config.hsv.v <= RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL)
                    rgb_matrix_config.hsv.v = qadd8(rgb_matrix_config.hsv.v, RGB_MATRIX_VAL_STEP);
#    endif
                eeprom_write_byte((uint8_t *)EECONFIG_RGB_MATRIX + offsetof(rgb_config_t, hsv.v), rgb_matrix_config.hsv.v);
                return false;

            case RGB_VAD:
#    ifdef RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL
                if (rgb_matrix_config.enable && rgb_matrix_config.hsv.v > RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL)
#    endif
                {
                    rgb_matrix_config.hsv.v = qsub8(rgb_matrix_config.hsv.v, RGB_MATRIX_VAL_STEP);
                    eeprom_write_byte((uint8_t *)EECONFIG_RGB_MATRIX + offsetof(rgb_config_t, hsv.v), rgb_matrix_config.hsv.v);
                }
#    ifdef RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL
                if (rgb_matrix_config.enable && rgb_matrix_config.hsv.v <= RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL) {
                    rgb_matrix_toggle();
                }
#    endif
                return false;

            default:
                break;
        }
    }
    return true;
}
#endif
