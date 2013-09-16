#ifndef __SYS_KEYS_H
#define __SYS_KEYS_H

typedef enum keycode_t {
	kKeyNull = -1,
	kKeyEscape, kKeyF1, kKeyF2, kKeyF3, kKeyF4, kKeyF5, kKeyF6, kKeyF7, kKeyF8, kKeyF9, kKeyF10, kKeyF11, kKeyF12,
	kKeyBacktick, kKey1, kKey2, kKey3, kKey4, kKey5, kKey6, kKey7, kKey8, kKey9, kKey0, kKeyDash, kKeyEquals, kKeyBackspace,
	kKeyTab, kKeyQ, kKeyW, kKeyE, kKeyR, kKeyT, kKeyY, kKeyU, kKeyI, kKeyO, kKeyP, kKeyLeftBracket, kKeyRightBracket, kKeyBackslash,
	kKeyCapsLock, kKeyA, kKeyS, kKeyD, kKeyF, kKeyG, kKeyH, kKeyJ, kKeyK, kKeyL, kKeySemicolon, kKeyApostrophe, kKeyEnter,
	kKeyLeftShift, kKeyZ, kKeyX, kKeyC, kKeyV, kKeyB, kKeyN, kKeyM, kKeyComma, kKeyPeriod, kKeySlash, kKeyRightShift,
	kKeyLeftControl, kKeyLeftAlt, kKeyLeftSuper, kKeySpace, kKeyRightSuper, kKeyRightAlt, kKeyRightControl,
    kKeyPadAsterisk, kKeyNumLock
} KeyCode;

static KeyCode scan1_to_key[70] = {
    kKeyNull,       kKeyEscape,   kKey1,         kKey2,         kKey3,          kKey4,           kKey5,           kKey6,            kKey7,        kKey8,
    kKey9,          kKey0,        kKeyDash,      kKeyEquals,    kKeyBackspace,  kKeyTab,         kKeyQ,           kKeyW,            kKeyE,        kKeyR, 
    kKeyT,          kKeyY,        kKeyU,         kKeyI,         kKeyO,          kKeyP,           kKeyLeftBracket, kKeyRightBracket, kKeyEnter,    kKeyLeftControl, 
    kKeyA,          kKeyS,        kKeyD,         kKeyF,         kKeyG,          kKeyH,           kKeyJ,           kKeyK,            kKeyL,        kKeySemicolon,
    kKeyApostrophe, kKeyBacktick, kKeyLeftShift, kKeyBackslash, kKeyZ,          kKeyX,           kKeyC,           kKeyV,            kKeyB,        kKeyN,
    kKeyM,          kKeyComma,    kKeyPeriod,    kKeySlash,     kKeyRightShift, kKeyPadAsterisk, kKeyLeftAlt,     kKeySpace,        kKeyCapsLock, kKeyF1,
    kKeyF2,         kKeyF3,       kKeyF4,        kKeyF5,        kKeyF6,         kKeyF7,          kKeyF8,          kKeyF9,           kKeyF10,      kKeyNumLock
};

static KeyCode scan2_to_key[48] = {
	kKeyNull, kKeyF9,      kKeyNull,	  kKeyF5,   kKeyF3,          kKeyF1, kKeyF2, kKeyF12,  kKeyNull, kKeyF10,  kKeyF8, kKeyF6, kKeyF4, kKeyTab, kKeyBacktick, kKeyNull,
	kKeyNull, kKeyLeftAlt, kKeyLeftShift, kKeyNull, kKeyLeftControl, kKeyQ,  kKey1,  kKeyNull, kKeyNull, kKeyNull, kKeyZ,  kKeyS,  kKeyA,  kKeyW,   kKey2,        kKeyNull,
	kKeyNull, kKeyC,       kKeyX,         kKeyD,    kKeyE,           kKey4,  kKey3,  kKeyNull, kKeyNull, kKeySpace, kKeyV, kKeyF,  kKeyT,  kKeyR,   kKey5,        kKeyNull
};

#endif
