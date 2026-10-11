// Board selector: a notice board a coder carries, which reads any board of the
// mud. It is an ordinary board (read, post, reply... work as on any other),
// and two commands change which board it is:
//
//   board <name>        read that board
//   board next | prev   the next or the previous one in the list
//   boards              every board registered, with its notes and how many
//                       are new
//
// From the selective board of ancient-kingdoms (obj/misc/board_selec.c),
// Radix 01/1996, translated by Vaeleth 11/2000.

#include <item/board.h>
#include <language.h>

inherit "/lib/obj/board.c";

int do_board(string str);
int do_boards(string str);

void create()
{
  ::create();

  set_name(_LANG_BOARD_SELECTOR_NAME);
  set_aliases(_LANG_BOARD_SELECTOR_ALIASES);
  set_short(_LANG_BOARD_SELECTOR_SHORT);
  set_main_plural(_LANG_BOARD_SELECTOR_PLURAL);
  set_plurals(_LANG_BOARD_SELECTOR_PLURALS);
  set_long(_LANG_BOARD_SELECTOR_LONG);
  set_get();
  set_weight(5);
}

void init()
{
  ::init();
  add_action("do_board", "board");
  add_action("do_boards", "boards");
}

// Only a coder carries one: anybody else gets nothing from it.
int test_coder()
{
  return this_player() && this_player()->query_coder();
}

int do_board(string str)
{
  string * boards;
  int i;

  if (!test_coder())
    return 0;

  boards = (string *)BOARD_HAND->list_of_boards();
  if (!sizeof(boards))
  {
    notify_fail("There are no boards.\n");
    return 0;
  }

  if (!str || !strlen(str))
  {
    write("Reading the board '" + board_name + "'. " +
          "Usage: board <name|next|prev>, boards\n");
    return 1;
  }

  i = member_array(board_name, boards);

  if (str == "next")
    str = boards[(i + 1) % sizeof(boards)];
  else if (str == "prev")
    str = boards[(i - 1 + sizeof(boards)) % sizeof(boards)];
  else if (member_array(str, boards) == -1)
  {
    // switching never creates a board: only the registered ones are read
    notify_fail("There is no board called '" + str + "'. " +
                "Type 'boards' to list them.\n");
    return 0;
  }

  board_name = str;
  write("Now reading the board '" + board_name + "'.\n");
  return 1;
}

int do_boards(string str)
{
  string * boards;
  string ret;
  mixed * notes;
  mapping news_rc;
  int i, j, unread;

  if (!test_coder())
    return 0;

  boards = sort_array((string *)BOARD_HAND->list_of_boards());
  news_rc = (mapping)this_player()->query_property(NEWS_RC);
  if (!mappingp(news_rc))
    news_rc = ([ ]);

  ret = "Boards registered (" + sizeof(boards) + "):\n";
  for (i = 0; i < sizeof(boards); i++)
  {
    notes = (mixed *)BOARD_HAND->get_subjects(boards[i]);
    unread = 0;
    for (j = 0; j < sizeof(notes); j++)
      if (notes[j][B_TIME] > news_rc[boards[i]])
        unread++;

    ret += sprintf("  %s %-30s %3d note%s%s\n",
                   (boards[i] == board_name ? "*" : " "), boards[i],
                   sizeof(notes), (sizeof(notes) == 1 ? " " : "s"),
                   (unread ? ", " + unread + " new" : ""));
  }

  write(ret);
  return 1;
}
