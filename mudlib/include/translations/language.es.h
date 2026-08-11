
// STD_LANG and every language key are language-neutral lowercase English
// ids ("common", "elf", ...). They are what gets stored in savefiles and
// passed around the code; the real-world-language name shown to the player
// comes from _LANG_LANGUAGE_NAMES below, and user input is resolved back to
// an id through _LANG_LANGUAGE_ALIASES.
#define STD_LANG "common"
#define _LANG_PREPOSITION "en"

// language id -> display name in this instance's real-world language.
// Unknown ids fall back to the capitalised id (see languages handler).
#define _LANG_LANGUAGE_NAMES ([ "common" : "común", ])

// language id -> accepted input aliases (accent-less / variant forms), so
// `hablar comun` resolves to the id "common".
#define _LANG_LANGUAGE_ALIASES ([ "common" : ({ "common", "comun", "común" }), ])
