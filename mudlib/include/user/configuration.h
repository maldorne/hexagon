
// What a configuration entry says about one setting.
#define CONFIG_TYPE    "type"     // CONFIG_BOOL, CONFIG_INT, CONFIG_CHOICE or CONFIG_STRING
#define CONFIG_OWNER   "owner"    // CONFIG_ON_USER or CONFIG_ON_PLAYER
#define CONFIG_QUERY   "query"    // function that answers the current value
#define CONFIG_SET     "set"      // function that changes it
#define CONFIG_PARAM   "param"    // optional first argument to both functions
#define CONFIG_OPTIONS "options"  // the values a CONFIG_CHOICE accepts
#define CONFIG_MIN     "min"      // the bounds of a CONFIG_INT
#define CONFIG_MAX     "max"
#define CONFIG_COMMAND "command"  // the command that does the same, if any
#define CONFIG_REFUSES "refuses"  // the set function answers 0 when it refuses

#define CONFIG_BOOL   "bool"
#define CONFIG_INT    "int"
#define CONFIG_CHOICE "choice"
#define CONFIG_STRING "string"

#define CONFIG_ON_USER   "user"
#define CONFIG_ON_PLAYER "player"
