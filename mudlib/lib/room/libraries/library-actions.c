// Library: shelves of books that anybody may read where they stand, take a
// copy of, or add a book of their own to. Shared by the library room
// (/lib/room/libraries/library-room.c) and the library location component
// (/lib/location/components/library.c).
//
// A book on the shelves is the path of its text (see /lib/handlers/books.c),
// so the shelves hold texts rather than objects: taking one gives a fresh
// copy and leaves the text on the shelf, and leaving a book adds its text if
// the shelves do not have it yet.
//
// Ported from the CcMud libraries, Folken 12/2009.

#include <language.h>

#define LIBRARY_BOOK_OB "/lib/obj/book.c"

private string * library_books;
static object library_sign;

int query_library() { return 1; }

// Where the shelves stand. The component answers with its location.
object query_library_place() { return this_object(); }

// Called after the shelves change. The component saves its location.
void save_library() { }

void create()
{
  library_books = ({ });
}

string * query_library_books() { return library_books + ({ }); }

void set_library_books(string * paths)
{
  library_books = pointerp(paths) ? paths + ({ }) : ({ });
}

int add_library_book(string path)
{
  if (!path || !strlen(path) || member_array(path, library_books) != -1)
    return 0;

  library_books += ({ path });
  return 1;
}

int remove_library_book(string path)
{
  if (member_array(path, library_books) == -1)
    return 0;

  library_books -= ({ path });
  return 1;
}

// Put up the sign that says how the library is used, replacing any earlier
// one, so a place never has two.
object create_library_sign(object where)
{
  if (library_sign)
    library_sign->dest_me();

  library_sign = where->add_sign(_LANG_LIBRARY_SIGN_LONG, _LANG_LIBRARY_SIGN_TEXT);
  return library_sign;
}

// What a shelf a library puts up for itself looks like.
string query_library_shelf_description() { return _LANG_LIBRARY_SHELF_DESC; }

// Take the sign down again, when the library leaves its place.
void take_down_library_sign()
{
  if (library_sign)
    library_sign->dest_me();
  library_sign = nil;
}

void init()
{
  add_action("do_list_books", _LANG_LIBRARY_CMDS_LIST);
  add_action("do_take_book", _LANG_LIBRARY_CMDS_TAKE);
  add_action("do_leave_book", _LANG_LIBRARY_CMDS_LEAVE);
}

// The shelf a player named by its number in the list, or nil.
private string _book_by_number(string str)
{
  int num;

  if (!str || sscanf(str, "%d", num) != 1)
    return nil;
  if (num < 1 || num > sizeof(library_books))
    return nil;

  return library_books[num - 1];
}

int do_list_books(string str)
{
  string ret, title;
  int i;

  if (!sizeof(library_books))
  {
    tell_object(this_player(), _LANG_LIBRARY_EMPTY);
    return 1;
  }

  ret = _LANG_LIBRARY_LIST_HEADER;
  for (i = 0; i < sizeof(library_books); i++)
  {
    title = handler("books")->query_title(library_books[i]);
    ret += "\t" + (i + 1) + ") " +
           ((title && strlen(title)) ? title : _LANG_LIBRARY_DAMAGED) + "\n";
  }
  ret += _LANG_LIBRARY_LIST_FOOTER;

  tell_object(this_player(), ret);
  return 1;
}

// read <n>: read a book of the shelves without taking it. The read command
// asks the library for it when it is given a number.
int do_read_book(string str)
{
  string path;

  path = _book_by_number(str);
  if (!path)
    return 0;

  handler("books")->start_reading(this_player(), path);
  return 1;
}

// A copy of the book with that text among `obs`, or nil.
private object _copy_among(object * obs, string path)
{
  int i;

  for (i = 0; i < sizeof(obs); i++)
    if (obs[i]->query_book() && obs[i]->query_text_file() == path)
      return obs[i];

  return nil;
}

int do_take_book(string str)
{
  string path;
  object book;

  path = _book_by_number(str);
  if (!path)
  {
    notify_fail(_LANG_LIBRARY_WHICH);
    return 0;
  }

  // one copy per reader: none while they carry one, even in a bag, or one
  // lies here for the taking
  if (_copy_among(deep_inventory(this_player()), path))
  {
    notify_fail(_LANG_LIBRARY_ALREADY_CARRIED);
    return 0;
  }
  // the floor: the reader stands here too, and what they carry is checked
  // above
  if (environment(this_player()) &&
      _copy_among(all_inventory(environment(this_player())) -
                  ({ this_player() }), path))
  {
    notify_fail(_LANG_LIBRARY_COPY_HERE);
    return 0;
  }

  book = clone_object(LIBRARY_BOOK_OB);
  book->set_text_file(path);

  if (book->move(this_player()))
  {
    book->dest_me();
    notify_fail(_LANG_LIBRARY_CANNOT_CARRY);
    return 0;
  }

  tell_object(this_player(), _LANG_LIBRARY_TAKEN);
  tell_room(environment(this_player()), _LANG_LIBRARY_TAKEN_OTHERS,
            ({ this_player() }));
  return 1;
}

int do_leave_book(string str)
{
  object book, * obs;
  string path;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail(_LANG_LIBRARY_LEAVE_WHAT);
    return 0;
  }

  // the first carried book that answers to that name: a name may fit other
  // things too, and find_match would return only the first of them
  obs = all_inventory(this_player());
  book = nil;
  for (i = 0; i < sizeof(obs) && !book; i++)
    if (obs[i]->query_book() && obs[i]->id(str))
      book = obs[i];

  if (!book)
  {
    notify_fail(_LANG_LIBRARY_ONLY_BOOKS);
    return 0;
  }

  path = book->query_text_file();
  if (!add_library_book(path))
  {
    notify_fail(_LANG_LIBRARY_ALREADY_THERE);
    return 0;
  }

  save_library();
  tell_object(this_player(), _LANG_LIBRARY_LEFT);
  tell_room(environment(this_player()), _LANG_LIBRARY_LEFT_OTHERS,
            ({ this_player() }));
  book->dest_me();
  return 1;
}

mixed * stats()
{
  return ({ ({ "Library books", library_books, }), });
}
