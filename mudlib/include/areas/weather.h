
#define WEATHER_HANDLER "weather"

// Default save file for the shared lib weather instance. Per-game
// subclasses override query_save_file() to point elsewhere.
#define SAVE_FILE "/save/weather.o"
#define LOG_FILE "weather"

#define WEATHER_TABLE "weather_table"
#define WEATHER_SH "/lib/obj/shadows/weather_sh.c"

#define CALENDAR_HANDLER "calendar"

// Number of weather zones mutated on each update tick.
#define MODIFY 3

// Default increment when the computed delta is under the smoothing threshold.
#define DEF_INC 15

// Flags dispatched via weather_inform / event_weather.
#define FLAG_DAWN 1
#define FLAG_NIGHTFALL 2
#define FLAG_CLIMATE 3
#define FLAG_CHECK 4

#define SNOWBALL_OB "/obj/environ/snowball.c"
#define SNOWMAN_OB "/obj/environ/snowman.c"
