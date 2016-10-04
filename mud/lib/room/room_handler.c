/* this is the room handler... it keeps track of all sorts of silly
 * things...
 * 
 * Añadida salida "rope", Folken 05/2009
 */

mapping exit_types;
mapping door_types;
mapping opposite;
mapping opp_dirs;

/* ({ mess, obv, size, func }) */
void create() {
  exit_types = ([
   "standard" :({ 0, 1,  10, 0 }),
   "corridor" :({ 0, 1,  10, 0 }),
   "plain"    :({ 0, 1, 100, 0 }), // very large
   "door"     :({ 0, 1,   5, 0 }),
   "stair"    :({ 0, 1,   5, 0 }), // going up? 
   "hidden"   :({ 0, 0,   5, 0 }), // hidden non door exit 
   "secret"   :({ 0, 0,   5, 0 }), // secret door 
   "gate"     :({ 0, 1,  15, 0 }),
   "road"     :({ 0, 1,  30, 0 }),
   "path"     :({ 0, 1,  20, 0 }),
   "rope"     :({ 0, 1,   5, 0 }), // cuerdas, escalas, etc.
  ]);
  door_types = ([
    "door"    :({ 0, 0, "generic_key", 100 }),
    "secret"  :({ 0, 0, "generic_key", 100 }),
    "gate"    :({ 1, 0, "generic_key", 100 }),
  ]);
  opposite = ([ "norte":({0,"el sur"}), "sur":({0,"el norte"}),
                "este":({0,"el oeste"}),  "oeste":({0,"el este"}),
                "noreste":({0,"el sudoeste"}),
                "noroeste":({0,"el sudeste"}),
                "sudeste":({0,"el noroeste"}),
                "sudoeste":({0,"el noreste"}),
                "arriba":({0,"abajo"}), "abajo":({0,"arriba"}),
                "fuera":({0,"dentro"}), "dentro":({0,"fuera"}),
                "salida":({0,"la entrada"}), "entrada":({0,"la salida"}), 
                "escaleras":({0,"las escaleras"}), ]);

  opp_dirs = ([ "norte":"sur", "sur":"norte",
                "este":"oeste",  "oeste":"este",
                "noreste":"sudoeste",
                "noroeste":"sudeste",
                "sudeste":"noroeste",
                "sudoeste":"noreste",
                "arriba":"abajo", "abajo":"arriba",
                "fuera":"dentro", "dentro":"fuera",
                "salida":"entrada", "entrada":"salida", 
                "escaleras":"escaleras", ]);
}

string query_opposite(string str)
{
  string * dirs;
  dirs = keys(opp_dirs);

  if (member_array(str, dirs) == -1)
    return "";
  return opp_dirs[str];
}

string query_exit_dir(string str){
  switch(str){
    case "norte":
    case "sur":
    case "este":
    case "oeste":
    case "noroeste":
    case "noreste":
    case "sudoeste":
    case "sudeste":
       return "el " + str;
    case "escaleras":
       return "las " + str;
    case "entrada":
    case "salida":
       return "la " + str;
    default:
       return str;
  }
  return str;
}

int add_exit_type(string type, mixed message, mixed obvious, int size,
                  mixed func) {
  if (exit_types[type])
    return 0;

  exit_types[type] = ({ message, obvious, size, func });
  return 1;
}

int remove_exit_type(string type) {
  exit_types = m_delete(exit_types, type);
  return 1;
}

mixed *query_door_type(string type, string direc, string dest) {

  if (!door_types[type])
    return door_types["gate"];

  /* If there isnt a door on the other side.  We don't join. 
     call_out("check_door", 0, ({ previous_object(), direc, dest }));
  */
  return door_types[type] + ({ 0 });
}

/*
void check_door(mixed bing) {
  mixed frog;

   if( stringp(bing[2]) && !find_object(bing[2]))
    if (catch(call_other(bing[2], "??"))) { **** room failed to load. ****
      bing[0]->modify_exit(bing[1], ({ "undoor", 0 }));
      return ;
    }
  frog = (mixed)bing[2]->query_door(bing[0]);
  *** this piece of code bugs MudOS v21+ so I have to do a check.
  *** Baldrick, sept '95
  ***
  if ((!sizeof(bing[0]) && !bing[0]) || (!sizeof(bing[1]) && !bing[1]) )
    return;
  if (!frog)
    bing[0]->modify_exit(bing[1], ({ "undoor", 0 }));
  else
    bing[0]->modify_exit(bing[1], ({ "other", frog }));
}
*/

mixed *query_exit_type(string type, string dir) {
  mixed s;

  if (!(s = opposite[dir])) {
   /* Commented out...
      log_file("BAD_DIRS", "Type: " + type + ", Dir: " + dir + ", Caller: " +
      file_name(previous_object()) + ".\n\n");
   */
    s = ({ 0, "algún sitio" });
  }
  if (!exit_types[type])
    return exit_types["standard"] + ({ s });
  return exit_types[type] + ({ s });
}

/* Hamlet.  Colors for the exit string. */
string exit_string_color(string which) {
  int i;
  string exit_color;
  string *allowed_colors;

  exit_color = "%^BOLD%^%^CYAN%^";
  allowed_colors = ({"red","%^RED%^","yellow","%^YELLOW%^",
                     "green","%^GREEN%^","cyan","%^CYAN%^",
                     "blue","%^BLUE%^","magenta","%^MAGENTA%^",
                     "white","%^WHITE%^","orange","%^ORANGE%^"
                     });
  if((i = member_array(which,allowed_colors)) == -1)
    return exit_color;
  if(i%2 == 0)  i++;
  exit_color = "%^BOLD%^"+allowed_colors[i];
  return exit_color;
}
