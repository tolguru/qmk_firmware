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

#pragma once

/* Encoder Configuration */
#define ENCODER_DEFAULT_POS 0x3
#define ENCODER_MAP_KEY_DELAY 2

#ifdef LK_WIRELESS_ENABLE
/* Hardware configuration */
#    define P2P4_MODE_SELECT_PIN A10
#    define BT_MODE_SELECT_PIN A9

#    define LKBT51_RESET_PIN C4
#    define LKBT51_INT_INPUT_PIN B0
#    define BLUETOOTH_INT_OUTPUT_PIN A4

#    define USB_POWER_SENSE_PIN B1
#    define USB_POWER_CONNECTED_LEVEL 0

#    define BAT_CHARGING_PIN C11
#    define BAT_CHARGING_LEVEL 0

#    define BT_HOST_DEVICES_COUNT 3

#    if defined(RGB_MATRIX_ENABLE) 

#        define LED_DRIVER_SHUTDOWN_PIN A2

#        define BT_HOST_LED_MATRIX_LIST \
            { 0, 1, 2 }

#        define P2P4G_HOST_LED_MATRIX_LIST \
            { 3 }

/* Backlit disable timeout when keyboard is disconnected(unit: second) */
#        define DISCONNECTED_BACKLIGHT_DISABLE_TIMEOUT 40

/* Backlit disable timeout when keyboard is connected(unit: second) */
#        define CONNECTED_BACKLIGHT_DISABLE_TIMEOUT 600

/* Reinit LED driver on tranport changed */
#        define REINIT_LED_DRIVER 1

#    endif

/* Keep USB connection in blueooth mode */
#    define KEEP_USB_CONNECTION_IN_WIRELESS_MODE

/* Enable bluetooth NKRO */
#    define WIRELESS_NKRO_ENABLE

/* Raw hid command for factory test and bluetooth DFU */
#    define RAW_HID_CMD 0xAA ... 0xAB
#else
/* Raw hid command for factory test */
#    define RAW_HID_CMD 0xAB
#endif

/* Factory test keys */
#define FN_KEY_1 MO(1)
#define FN_BL_TRIG_KEY  KC_PMNS

#define BL_CYCLE_KEY    KC_MPLY   
#define FN_Z_KEY        RGB_SAD
#define FN_J_KEY        RGB_HUD

#define MATRIX_IO_DELAY 10
