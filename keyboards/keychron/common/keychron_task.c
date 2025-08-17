/* Copyright 2023~2025 @ Keychron (https://www.keychron.com)
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

#include <stdlib.h>
#include "keychron_task.h"
#include "quantum.h"
#include "keychron_common.h"
#ifdef FACTORY_TEST_ENABLE
#    include "factory_test.h"
#endif
#ifdef RETAIL_DEMO_ENABLE
#    include "retail_demo.h"
#endif

__attribute__((weak)) bool process_record_keychron_kb(uint16_t keycode, keyrecord_t *record) {
    return true;
}

bool process_record_keychron(uint16_t keycode, keyrecord_t *record) {
#ifdef LK_WIRELESS_ENABLE
    extern bool process_record_wireless(uint16_t keycode, keyrecord_t * record);
    if (!process_record_wireless(keycode, record)) return false;
#endif
#ifdef FACTORY_TEST_ENABLE
    if (!process_record_factory_test(keycode, record)) return false;
#endif

#ifdef SNAP_CLICK_ENABLE
    extern bool process_record_snap_click(uint16_t keycode, keyrecord_t * record);
    if (!process_record_snap_click(keycode, record)) return false;
#endif

    if (!process_record_keychron_kb(keycode, record)) return false;

#if defined(KEYCHRON_RGB_ENABLE) && defined(EECONFIG_SIZE_CUSTOM_RGB)
#    if defined(RETAIL_DEMO_ENABLE)
    if (!process_record_retail_demo(keycode, record)) {
        return false;
    }
#    endif

    extern bool process_record_keychron_rgb(uint16_t keycode, keyrecord_t *record);
    if (!process_record_keychron_rgb(keycode, record)) {
        return false;
    }
#endif

    return true;
}

#if defined(LED_MATRIX_ENABLE)
bool led_matrix_indicators_keychron(void) {
#    ifdef LK_WIRELESS_ENABLE
    extern bool led_matrix_indicators_bt(void);
    led_matrix_indicators_bt();
#    endif
#    ifdef FACTORY_TEST_ENABLE
    factory_test_indicator();
#    endif
    return true;
}
#endif

#if defined(RGB_MATRIX_ENABLE)
bool rgb_matrix_indicators_keychron(void) {
#    ifdef LK_WIRELESS_ENABLE
    extern bool rgb_matrix_indicators_bt(void);
    rgb_matrix_indicators_bt();
#    endif
#    ifdef FACTORY_TEST_ENABLE
    factory_test_indicator();
#    endif
    return true;
}
#endif

__attribute__((weak)) bool keychron_task_kb(void) {
    return true;
}

void keychron_task(void) {
#ifdef LK_WIRELESS_ENABLE
    extern void wireless_tasks(void);
    wireless_tasks();
#endif
#ifdef FACTORY_TEST_ENABLE
    factory_test_task();
#endif
#if defined(RETAIL_DEMO_ENABLE) && defined(KEYCHRON_RGB_ENABLE) && defined(EECONFIG_SIZE_CUSTOM_RGB)
    retail_demo_task();
#endif

    keychron_common_task();

    keychron_task_kb();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;
    
    if (!process_record_keychron(keycode, record)) return false;

    return true;
}

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) return false;

    rgb_matrix_indicators_keychron();

    return true;
}
#endif

#ifdef LED_MATRIX_ENABLE
bool led_matrix_indicators_kb(void) {
    if (!led_matrix_indicators_user()) return false;

    led_matrix_indicators_keychron();

    return true;
}
#endif

void housekeeping_task_kb(void) {
    keychron_task();
}
