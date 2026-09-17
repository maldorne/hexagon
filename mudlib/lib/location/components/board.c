// Board component: a notice board standing in the location. The board item
// is transient; the component keeps which board it shows (boards live in
// /lib/handlers/boards.c by name) and puts a fresh one in place on every load.

#include <room/location.h>

#define BOARD_OB "/lib/obj/board.c"

inherit component "/lib/location/component.c";

private string board_name;
static object board;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BOARD);
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc || !strlen(board_name))
    return;

  // a reconversion runs this again on a live location: never two boards
  if (board)
    board->dest_me();

  board = clone_object(BOARD_OB);
  board->set_datafile(board_name);
  board->move(loc);
}

void set_board_name(string name) { board_name = name; }
string query_board_name() { return board_name; }

string query_info() { return strlen(board_name) ? board_name : ""; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "board_name" : board_name ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["board_name"]))
    board_name = args["board_name"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Board name", board_name, }), });
}
