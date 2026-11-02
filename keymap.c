// Copyright 2022 Cole Smith <cole@boadsource.xyz>
// SPDX-License-Identifier: GPL-2.0-or-later

// Elite-C (ATmega32U4) controllers. Flash each half on its own, no reset button needed:
// unplug USB and TRRS, hold that half's outermost top key (Esc on the left, = on the right),
// plug only that half in (bootmagic: it enters the DFU bootloader). Easiest: run flash.sh <hex> first,
// type the sudo password, then do the unplugging; it waits and flashes. Or, once in the bootloader:
//   qmk flash -kb boardsource/lulu/avr -km twizLulu
// RAISE + top-right (QK_BOOT) also works, but only resets the half on USB.

#include QMK_KEYBOARD_H

enum layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _ADJUST
};


// fancy alt tab timer
bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;
#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)


#define ES_TSK LT(0,KC_1)
#define TABL LT(0,KC_F1)
#define TABR LT(0,KC_F2)
#define TABCLOSE LT(0,KC_F3)
#define RE_CACHE LT(0,KC_F5)
#define QUIT LT(0,KC_6)
#define AUD_5 LT(0,KC_H) // 5 on tap, toggle audio source on hold
#define TD_RD LT(0,KC_G) // macro for labelling in gmail
#define WT_SV LT(0,KC_J) // macro for labelling in gmail
#define ABRC LT(0,KC_K) // Brackets (ambidexterous)
#define LNKPST LT(0,KC_L) // hyperlink and paste clipboard


#define DRK_BANG LT(0,KC_Z)
#define VIS_HASH LT(0,KC_C)
#define TAB_RES LT(0,KC_ESC)
#define SHALTAB LT(0, KC_F21) // when held: shift, when tapped: alt tab with timeout functionality
#define KVMONE LT(0,KC_LGUI)
#define KVMTWO LT(0,KC_RGUI)




#define AUDIOSRC MEH(KC_G)
#define SH_TAB LSFT_T(KC_TAB)
#define SSNIP LGUI(LSFT(KC_S)) // GUI+SHIFT+S for snipping tool
#define SH_SLS RSFT_T(KC_SLSH)
#define SH_BSLS RSFT_T(KC_BSLS)

#define CLFT LCTL(KC_LEFT)
#define CRGT LCTL(KC_RIGHT)
#define CUP LCTL(KC_UP)
#define CDN LCTL(KC_DOWN)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* QWERTY
 *  + denotes a tap + hold key (hold action in process_record_user)
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC+ |  1   |  2   |  3   |  4   |  5   |                    |  6   |  7   |  8   |  9   |  0   |  =   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  `   |  Q   |  W   |  E   |  R   |  T   |                    |  Y   |  U   |  I   |  O   |  P   |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |SH+TAB|  A   |  S   |  D   |  F   |  G   |-------.    ,-------|  H   |  J   |  K   |  L   |  ;   |  '   |
 * |------+------+------+------+------+------| INS   |    | MPLY  |------+------+------+------+------+------|
 * | CTL  |  Z   |  X   |  C   |  V   |  B   |-------|    |-------|  N   |  M   |  ,   |  .   |  [+] | SH+/ |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | RAISE| LGUI | LALT | /Space  /       \BackSP\  |Enter |LOWER | DEL  |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
 [_QWERTY] = LAYOUT(
  ES_TSK,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  KC_GRV,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  SH_TAB,   KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LCTL,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_INS,  KC_MPLY,  KC_N,    KC_M,    KC_COMM, KC_DOT,  ABRC,    SH_SLS,
                            RAISE,    KC_LGUI, KC_LALT, KC_SPC,  KC_BSPC,  KC_ENT,  LOWER, KC_DELETE
),
/* LOWER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |TABRS+| TABL+|CTRLW+| TABR+|  F4+ |  F5+ |                    |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  1   |  2   |  3   |  4   |  5+  |                    |  6   |  7   |  UP  |  9   |  0   | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |ALTAB+|      |      |      |      | SNIP |-------.    ,-------|      | LEFT | DOWN | RIGHT|      |      |
 * |------+------+------+------+------+------| MSEL  |    |       |------+------+------+------+------+------|
 * |      |  !+  |  @   |  #+  |LNKPST|  %   |-------|    |-------|  ^   |  &   |  *   |      |      | SH+\ |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /       /       \      \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_LOWER] = LAYOUT(
  TAB_RES, TABL,    TABCLOSE,TABR,    QUIT,    RE_CACHE,                   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
  _______, KC_1,    KC_2,    KC_3,    KC_4,    AUD_5,                      KC_6,    KC_7,    KC_UP,   KC_9,    KC_0,    KC_F12,
  SHALTAB, _______, _______, _______, _______, SSNIP,                      _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______,
  _______, DRK_BANG,KC_AT,   VIS_HASH,LNKPST,  KC_PERC,  KC_MSEL, _______, KC_CIRC,KC_AMPR, KC_ASTR,  _______, _______, SH_BSLS,
                             _______, _______, _______,  _______, _______, _______, _______, _______
),
/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | RGB> | RGB< | SAT+ | SAT- | VAL+ | VAL- |                    |TODO+ |      |      |      |      | BOOT |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | PGUP |      |      |      |                    |WAIT+ |      | C-UP |      |      | WAKE |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | HOME | PGDN | END  |      |      |-------.    ,-------|      |C-LEFT| C-DN |C-RGHT|      | PWR  |
 * |------+------+------+------+------+------|RGB TOG|    |   E   |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      | SLEEP|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      | KVM1 | KVM2 | /       /       \      \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_RAISE] = LAYOUT(
  RM_NEXT, RM_PREV, RM_SATU, RM_SATD, RM_VALU, RM_VALD,                     TD_RD,   _______, _______, _______, _______, QK_BOOT,
  _______, _______, KC_PGUP, _______,  _______, _______,                     WT_SV,   _______, CUP,     _______, _______, KC_WAKE,
  _______, KC_HOME, KC_PGDN, KC_END,   _______, _______,                     _______, CLFT,    CDN,     CRGT,    _______, KC_PWR,
  _______, _______, _______, _______,  _______, _______,  RM_TOGG, KC_E   ,  _______, _______, _______, _______, _______, KC_SLEP,
                             _______,  KVMONE,  KVMTWO,   _______, _______,  _______, _______, _______
),
/* ADJUST (LOWER + RAISE)
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | RGB> | RGB< | SAT+ | SAT- | VAL+ | VAL- |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      | HUE+ | HUE- |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      |      |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /       /       \      \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
  [_ADJUST] = LAYOUT(
  RM_NEXT, RM_PREV, RM_SATU, RM_SATD, RM_VALU, RM_VALD,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RM_HUEU, RM_HUED,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             _______, _______, _______, _______, _______,  _______, _______, _______
  )
};

// Hold-on-other-key only for the shift tap-holds; elsewhere a fast roll must stay a tap.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SH_TAB:
        case SH_SLS:
        case SH_BSLS:
        case SHALTAB:
            return true;
        default:
            return false;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
   return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KVMONE:
          if (record->event.pressed) {
              SEND_STRING(SS_TAP(X_SCROLL_LOCK) SS_DELAY(20) SS_TAP(X_SCROLL_LOCK) SS_DELAY(20) "1" SS_DELAY(20));  // SS_TAP(X_ENTER)
          } else{
          }
          return false;
        case KVMTWO:
          if (record->event.pressed) {
              SEND_STRING(SS_TAP(X_SCROLL_LOCK) SS_DELAY(20) SS_TAP(X_SCROLL_LOCK) SS_DELAY(20) "2" SS_DELAY(20));  // SS_TAP(X_ENTER)
          } else{
          }
          return false;
        case LNKPST:
          if (record->event.pressed) {
              SEND_STRING(SS_LCTL("k") SS_DELAY(20) SS_LCTL("v"));
          } else {
          }
          return false;
        case SHALTAB:
          if (record->tap.count && record->event.pressed) {
             if (record->event.pressed) { // alt tab timer active
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code16(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
          } else if (record->event.pressed) {
              register_code(KC_LSFT);
          }
          else {
              unregister_code(KC_LSFT);
          }
          return false;
        case ES_TSK:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_ESC);
          } else if (record->event.pressed) {
              tap_code16(LCTL(LSFT(KC_ESC)));
          }
          return false;

        case ABRC:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_LBRC);
          } else if (record->event.pressed) {
              tap_code16(KC_RBRC);
          }
          return false;
        case TAB_RES:
          if (record->tap.count && record->event.pressed) {
              tap_code16(RCTL(RSFT(KC_T)));
          } else if (record->event.pressed) {
              tap_code16(KC_F);
          }
          return false;
        case TABL:
          if (record->tap.count && record->event.pressed) {
              tap_code16(RCTL(RSFT(KC_TAB)));
          } else if (record->event.pressed) {
              tap_code16(KC_F1);
          }
          return false;
        case TABCLOSE:
          if (record->tap.count && record->event.pressed) {
            tap_code16(RCTL(KC_W));
          } else if (record->event.pressed) {
              tap_code16(KC_F2);
          }
          return false;
        case TABR:
          if (record->tap.count && record->event.pressed) {
              tap_code16(RCTL(KC_TAB));
          } else if (record->event.pressed) {
              tap_code16(KC_F3);
          }
          return false;
        case RE_CACHE:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_F5);
          } else if (record->event.pressed) {
              tap_code16(RCTL(KC_F5));
          }
          return false;
        case QUIT:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_F4);
          } else if (record->event.pressed) {
              tap_code16(RALT(KC_F4));
          }
          return false;
        case AUD_5:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_5);
          } else if (record->event.pressed) {
              tap_code16(AUDIOSRC);
          }
          return false;
        case DRK_BANG:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_EXLM);
          } else if (record->event.pressed) {
              tap_code16(LSFT(LALT(KC_F)));
          }
          return false;
        case VIS_HASH:
          if (record->tap.count && record->event.pressed) {
              tap_code16(KC_HASH);
          } else if (record->event.pressed) {
              tap_code16(LGUI(LALT(KC_V)));
          }
          return false;
        case TD_RD:
          if (record->tap.count && record ->event.pressed){
            SEND_STRING("vto-do\n");
          }
          else if (record->event.pressed){
            SEND_STRING("vread-through\n");
          }
          return false;
        case WT_SV:
          if (record->tap.count && record -> event.pressed){
            SEND_STRING("vwaiting\n");
          }
          else if (record->event.pressed){
              SEND_STRING("vsave\n");
          }
          return false;
    }
    return true;
}

// alt tab timer
void matrix_scan_user(void) {
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }
}

// rotary encoders
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { // unused atm
     /*   if(IS_LAYER_ON(_RAISE)  ){
            if (clockwise) {
                rgb_matrix_increase_hue();
            } else {
                rgb_matrix_decrease_hue();
            }
        }else if(IS_LAYER_ON(_LOWER)){
            if (clockwise){
                tap_code(KC_VOLU);
            } else{
                tap_code(KC_VOLD);
            }
        }else{
            if (clockwise){
                tap_code(KC_PGUP);
            } else{
                tap_code(KC_PGDN);
            }
        }*/
    } else if (index == 1) {
         if(IS_LAYER_ON(_RAISE)  ){
            if (clockwise) {
                tap_code(KC_J);
            } else {
                tap_code(KC_K);
            }
        }else if(IS_LAYER_ON(_LOWER)){
            if (clockwise){
                tap_code(MS_WHLU);
            } else{
                tap_code(MS_WHLD);
            }
        }else{
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
        }
    }
    return false; // skip lulu.c's encoder_update_kb, which would also send PgUp/PgDn
}
#endif
