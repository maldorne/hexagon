


#ifdef MUD_NAME
string mud_name() { return capitalize(MUD_NAME); }
#else
string mud_name() { return "Cc"; }
#endif

#ifdef VERSION
string version() { return VERSION; }
#else
#ifdef __VERSION__
string version() { return __VERSION__; }
#else
string version() { return "Desconocida"; }
#endif
#endif


