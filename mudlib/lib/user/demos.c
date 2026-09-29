// The demos an account has been through: the ones it finished, with which
// character and when, and the character of it playing each demo now. A new
// account may only play the demos, one character in each; finishing one opens
// every other game.

// game -> ({ character, time })
mapping finished_demos;
// game -> character
mapping demo_players;

#define GAMES_HANDLER_FILE "/lib/handlers/games"

void create()
{
  finished_demos = ([ ]);
  demo_players = ([ ]);
}

int has_finished_demo()
{
  return mappingp(finished_demos) && map_sizeof(finished_demos) > 0;
}

mapping query_finished_demos()
{
  return mappingp(finished_demos) ? ([ ]) + finished_demos : ([ ]);
}

// The character of this account playing a demo now, or "". One that has been
// deleted since no longer holds the place.
string query_demo_player(string game)
{
  string character;

  if (!mappingp(demo_players) || !(character = demo_players[game]))
    return "";

  if (member_array(character, this_object()->query_player_list()) == -1)
    return "";

  return character;
}

// Both records are kept by the games handler alone: it is the one that lets a
// character into a demo and the one that takes it out.
void set_demo_player(string game, string character)
{
  if (base_name(previous_object()) != GAMES_HANDLER_FILE)
    return;

  if (!mappingp(demo_players))
    demo_players = ([ ]);

  demo_players[game] = character;
  this_object()->save_me();
}

void add_finished_demo(string game, string character)
{
  if (base_name(previous_object()) != GAMES_HANDLER_FILE)
    return;

  if (!mappingp(finished_demos))
    finished_demos = ([ ]);
  if (!mappingp(demo_players))
    demo_players = ([ ]);

  finished_demos[game] = ({ character, time() });

  // the character leaves the demo, and its place with it
  if (demo_players[game] == character)
    demo_players[game] = nil;

  this_object()->save_me();
}
