

#ifdef MUD_NAME
static nomask string mud_name() { return capitalize(MUD_NAME); }
#else
static nomask string mud_name() { return "Cc"; }
#endif

// #ifdef VERSION
// string version() { return VERSION; }
// #else
// #ifdef __VERSION__
// string version() { return __VERSION__; }
// #else
// string version() { return "Desconocida"; }
// #endif
// #endif

static nomask string version() { return status()[0]; }
