#include QMK_KEYBOARD_H

// Custom double hold tap dance action
#define ACTION_TAP_DANCE_LAYER_DOUBLE_HOLD(layer1, layer2)                          \
    {                                                                               \
        .fn        = {NULL, td_double_hold_finished, td_double_hold_reset, NULL},   \
        .user_data = (void *)&((tap_dance_double_hold_t){layer1, layer2, TD_NONE}), \
    }

// Layer index
enum layers { _MAIN, _SYM, _NUM, _NAV, _MOUSE, _GAME };

// Custom macro keys
enum custom_keycodes { MOUSE_UP_RIGHT = SAFE_RANGE, MOUSE_UP_LEFT, MOUSE_DOWN_RIGHT, MOUSE_DOWN_LEFT, CUSTOM_KEYCODES_END };

// Tap dance keys
enum {
    TD_NAV_MOUSE,
};

// Tap dance states
typedef enum { TD_NONE, TD_UNKNOWN, TD_SINGLE_TAP_OR_HOLD, TD_DOUBLE_TAP_OR_HOLD } td_state_t;

// Tap dance context
typedef struct {
    uint8_t    layer1;
    uint8_t    layer2;
    td_state_t state;
} tap_dance_double_hold_t;

// Double key combination macro
typedef struct {
    uint16_t kc1;
    uint16_t kc2;
} double_key_combination_t;

// Functions that control what our tap dance keys do
td_state_t cur_dance(tap_dance_state_t *state);
void       td_double_hold_finished(tap_dance_state_t *state, void *user_data);
void       td_double_hold_reset(tap_dance_state_t *state, void *user_data);

// Tap dance key action configuration
tap_dance_action_t tap_dance_actions[] = {
    // Tap once for Escape, twice for Caps Lock
    [TD_NAV_MOUSE] = ACTION_TAP_DANCE_LAYER_DOUBLE_HOLD(_NAV, _MOUSE),
};

// Combination macro definitions
const double_key_combination_t combo_macros[] = {[MOUSE_UP_RIGHT - SAFE_RANGE] = {.kc1 = MS_UP, .kc2 = MS_RGHT}, [MOUSE_UP_LEFT - SAFE_RANGE] = {.kc1 = MS_UP, .kc2 = MS_LEFT}, [MOUSE_DOWN_RIGHT - SAFE_RANGE] = {.kc1 = MS_DOWN, .kc2 = MS_RGHT}, [MOUSE_DOWN_LEFT - SAFE_RANGE] = {.kc1 = MS_DOWN, .kc2 = MS_LEFT}};

// Process custom macro keys
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Check if this is a defined macro key
    if (keycode >= SAFE_RANGE && keycode < CUSTOM_KEYCODES_END) {
        // Execute combo macro
        double_key_combination_t combo = combo_macros[keycode - SAFE_RANGE];

        if (record->event.pressed) {
            register_code(combo.kc1);
            register_code(combo.kc2);
        } else {
            unregister_code(combo.kc1);
            unregister_code(combo.kc2);
        }
    }

    return true;
}

// Determine the current tap dance state
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        return TD_SINGLE_TAP_OR_HOLD;
    } else if (state->count == 2) {
        return TD_DOUBLE_TAP_OR_HOLD;
    }

    return TD_UNKNOWN;
}

// Tap dance state finish
void td_double_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_double_hold_t *td_user_data = (tap_dance_double_hold_t *)user_data;
    td_user_data->state                   = cur_dance(state);
    switch (td_user_data->state) {
        case TD_SINGLE_TAP_OR_HOLD:
            layer_lock_off(td_user_data->layer1); // Unlock the layer, in case it was locked
            layer_on(td_user_data->layer1);
            break;
        case TD_DOUBLE_TAP_OR_HOLD:
            layer_lock_off(td_user_data->layer2); // Unlock the layer, in case it was locked
            layer_on(td_user_data->layer2);
            break;
        default:
            break;
    }
}

// Tap dance state reset
void td_double_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_double_hold_t *td_user_data = (tap_dance_double_hold_t *)user_data;
    switch (td_user_data->state) {
        case TD_SINGLE_TAP_OR_HOLD:
            // Make sure the layer is not locked before turning it off
            if (!is_layer_locked(td_user_data->layer1)) {
                layer_off(td_user_data->layer1);
            }
            break;
        case TD_DOUBLE_TAP_OR_HOLD:
            // Make sure the layer is not locked before turning it off
            if (!is_layer_locked(td_user_data->layer2)) {
                layer_off(td_user_data->layer2);
            }
            break;
        default:
            break;
    }

    td_user_data->state = TD_NONE;
}

// Layered keymaps
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[_MAIN]  = LAYOUT(KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, CW_TOGG, KC_Q, KC_W, KC_E, KC_R, KC_T, TG(_GAME), KC_CAPS, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_SCRL, MO(_SYM), KC_A, KC_S, KC_D, KC_F, KC_G, KC_TAB, KC_DEL, KC_H, KC_J, KC_K, KC_L, KC_SCLN, MO(_SYM), KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, QK_LLCK, KC_PSCR, KC_APP, KC_INS, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_LALT, LCAG(KC_NO), MO(_NUM), KC_LGUI, KC_LCTL, KC_SPC, KC_BSPC, KC_ESC, KC_ENT, KC_RALT, RAG(KC_RCTL), TD(TD_NAV_MOUSE), KC_RGUI, KC_RCTL),
                                                              [_SYM]   = LAYOUT(KC_F13, KC_F14, KC_F15, KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, KC_F21, KC_F22, KC_F23, KC_F24, KC_TRNS, KC_TRNS, KC_BSLS, KC_LT, KC_GT, KC_TRNS, UG_NEXT, UG_VALU, KC_TRNS, KC_PIPE, KC_LCBR, KC_RCBR, KC_TRNS, UG_TOGG, KC_TRNS, KC_EQL, KC_PLUS, KC_MINS, KC_UNDS, KC_TILD, UG_PREV, UG_VALD, KC_GRV, KC_QUOT, KC_DQUO, KC_LPRN, KC_RPRN, KC_TRNS, KC_TRNS, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_SLEP, LCTL(KC_PSCR), RALT(KC_PAUS), RALT(KC_INS), KC_CIRC, KC_AMPR, KC_ASTR, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, LCTL(KC_SPC), LCTL(KC_BSPC), RALT(KC_ESC), RALT(KC_ENT), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                                                              [_NUM]   = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_7, KC_8, KC_9, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_COMM, KC_4, KC_5, KC_6, KC_DOT, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_1, KC_2, KC_3, KC_0, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                                                              [_NAV]   = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_PGUP, KC_UP, KC_PGDN, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LCTL, KC_TRNS, KC_NO, KC_HOME, KC_NO, KC_END, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                                                              [_MOUSE] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, MOUSE_UP_LEFT, MS_UP, MOUSE_UP_RIGHT, KC_NO, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_ACL0, KC_RALT, MS_WHLU, MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLL, MS_BTN3, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_ACL1, KC_TRNS, MS_WHLD, MOUSE_DOWN_LEFT, KC_NO, MOUSE_DOWN_RIGHT, MS_WHLR, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_ACL2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MS_BTN1, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                                                              [_GAME]  = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_TRNS, KC_TAB, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS)};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
