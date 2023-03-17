
// begin.c

#define _LANG_START_SHORT "%^BOLD%^Light in the %^BLACK%^Darkness%^RESET%^"
#define _LANG_START_LONG "You find yourself in a small, dimly lit room with a dusty " + \
          "keyboard and mouse in front of an old computer terminal. " + \
          "A list of diverse games scrolls across the monitor, each with a " + \
          "brief description and a play button. As you contemplate the possibilities, " + \
          "the room seems to come alive with a faint hum of energy, and the " + \
          "game you have chosen begins to load. You feel a sense of " + \
          "anticipation as you wait for the adventure to begin."
#define _LANG_START_HINTS ({ "Write 'games' to see the list of available " + \
          "games in which you can play.", "Write 'play <number>' to start playing that game." })
#define _LANG_PLAY_VERBS ({ "play" })
#define _LANG_PLAY_FAIL "Play what?\n"
#define _LANG_PLAY_GAME_DO_NOT_LOAD "The chosen game seems to be broken.\n"
#define _LANG_PLAY_START "Ok, you choose to play " + games[i]->query_game_name() + \
          ". Your character gives its first step in what could be a long journey. Good luck!\n"
