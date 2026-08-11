
// STD_LANG and every language key are language-neutral lowercase English
// ids ("common", "elf", ...). They are what gets stored in savefiles and
// passed around the code; the real-world-language name shown to the player
// comes from _LANG_LANGUAGE_NAMES below, and user input is resolved back to
// an id through _LANG_LANGUAGE_ALIASES.
#define STD_LANG "common"
#define _LANG_PREPOSITION "in"

// language id -> display name in this instance's real-world language.
// Unknown ids fall back to the capitalised id (see languages handler).
#define _LANG_LANGUAGE_NAMES ([ "common" : "Common", ])

// language id -> accepted input aliases.
#define _LANG_LANGUAGE_ALIASES ([ "common" : ({ "common" }), ])
