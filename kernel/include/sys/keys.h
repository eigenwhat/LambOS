#ifndef __SYS_KEYS_H
#define __SYS_KEYS_H

typedef enum keyevent_type_t {
    kKeyEventPressed, kKeyEventReleased
} KeyEventType;

typedef enum keycode_t {
    kKeyNull = 0,
    kKeyEscape, kKeyF1, kKeyF2, kKeyF3, kKeyF4, kKeyF5, kKeyF6, kKeyF7, kKeyF8, kKeyF9, kKeyF10, kKeyF11, kKeyF12,
    kKeyBacktick, kKey1, kKey2, kKey3, kKey4, kKey5, kKey6, kKey7, kKey8, kKey9, kKey0, kKeyDash, kKeyEquals, kKeyBackspace,
    kKeyTab, kKeyQ, kKeyW, kKeyE, kKeyR, kKeyT, kKeyY, kKeyU, kKeyI, kKeyO, kKeyP, kKeyLeftBracket, kKeyRightBracket, kKeyBackslash,
    kKeyCapsLock, kKeyA, kKeyS, kKeyD, kKeyF, kKeyG, kKeyH, kKeyJ, kKeyK, kKeyL, kKeySemicolon, kKeyApostrophe, kKeyEnter,
    kKeyLeftShift, kKeyZ, kKeyX, kKeyC, kKeyV, kKeyB, kKeyN, kKeyM, kKeyComma, kKeyPeriod, kKeySlash, kKeyRightShift,
    kKeyLeftControl, kKeyLeftSuper, kKeyLeftAlt, kKeySpace, kKeyRightAlt, kKeyRightSuper, kKeyMenu, kKeyRightControl,
    kKeyPrintScreen, kKeyScrollLock, kKeyPauseBreak,
    kKeyInsert, kKeyHome, kKeyPageUp,                kKeyNumLock, kKeyPadSlash, kKeyPadAsterisk, kKeyPadMinus,
    kKeyDelete, kKeyEnd, kKeyPageDown,               kKeyPad7, kKeyPad8, kKeyPad9, kKeyPadPlus,
                                                     kKeyPad4, kKeyPad5, kKeyPad6,
              kKeyUp,                                kKeyPad1, kKeyPad2, kKeyPad3, kKeyPadEnter,
    kKeyLeft, kKeyDown, kKeyRight,                   kKeyPad0, kKeyPadPeriod,

    kKeyCount
} KeyCode;

typedef struct keyevent_t {
    KeyEventType type;
    KeyCode keyCode;
} KeyEvent;

#endif
