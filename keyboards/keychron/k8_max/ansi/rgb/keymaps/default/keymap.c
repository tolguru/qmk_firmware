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
#include "keychron_common.h"

enum layers {
    BASE,
    FN1,
    FN2,
    FN3,
};

enum tap_dances {
    TD_A_B,
};

void a_b_finished(tap_dance_state_t *state, void *user_data);
void a_b_reset(tap_dance_state_t *state, void *user_data);

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_ansi_87(
         KC_ESC,                  KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_SIRI,  RGB_MOD,
         KC_GRV,        KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
         KC_TAB,        KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
         TD(TD_A_B),    KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
         KC_LSFT,                 KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
         KC_LCTL,       KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, MO(FN1),  KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [FN1] = LAYOUT_ansi_87(
       	XXXXXXX,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   	XXXXXXX,  	XXXXXXX,  RGB_TOG,
       	XXXXXXX,  BT_HST1,  BT_HST2,  BT_HST3,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,
       	XXXXXXX,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,            	XXXXXXX,
       	XXXXXXX,            	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  BAT_LVL,  NK_TOGG,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,            	XXXXXXX,            	XXXXXXX,
       	XXXXXXX,  	XXXXXXX,  	XXXXXXX,                                	XXXXXXX,                                	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX),

    [FN2] = LAYOUT_ansi_87(
         KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_CTANA, RGB_MOD,
         KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
         KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
         KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
         KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
         KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RGUI, MO(FN3),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [FN3] = LAYOUT_ansi_87(
         	XXXXXXX,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  	XXXXXXX,  	XXXXXXX,  RGB_TOG,
         	XXXXXXX,  BT_HST1,  BT_HST2,  BT_HST3,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,
         RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,
         	XXXXXXX,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,            	XXXXXXX,
         	XXXXXXX,            	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  BAT_LVL,  NK_TOGG,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,            	XXXXXXX,            	XXXXXXX,
         	XXXXXXX,  	XXXXXXX,  	XXXXXXX,                                	XXXXXXX,                                	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX,  	XXXXXXX)
};

// clang-format on
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }
    return true;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_A_B] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, a_b_finished, a_b_reset)
};

int td_pressed = 0;

void a_b_finished(tap_dance_state_t *state, void *user_data) {
    if (state -> count == 1) {
        if (state -> interrupted || !state -> pressed) {
            register_code(KC_A);
            td_pressed = KC_A;
            return;
        } else {
            register_code(KC_B);
            td_pressed = KC_B;
            return;
        }
    } else {
        register_code(KC_A);
        td_pressed = KC_A;
        return;
    }
}

void a_b_reset(tap_dance_state_t *state, void *user_data) {
    if (td_pressed == KC_A) {
        unregister_code(KC_A);
    } else if (td_pressed == KC_B) {
        unregister_code(KC_B);
    }
}