// A book: an item whose text is a markdown file, read page by page with
// 'read <book>' (see /lib/handlers/books.c for the format of the text). The
// title on its cover is the first heading of the text.
//
// Ported from the CcMud book, Folken 10/2009.

#include <language.h>

inherit "/lib/item.c";

private string text_file, title;

int query_book() { return 1; }

void create()
{
  ::create();

  set_name(_LANG_BOOK_NAME);
  add_alias(_LANG_BOOK_ALIASES);
  set_main_plural(_LANG_BOOK_PLURAL);
  add_plural(_LANG_BOOK_PLURALS);
  set_short(_LANG_BOOK_SHORT);
  set_long(_LANG_BOOK_LONG);
  set_weight(10);
}

// The text of this book: its short and its cover come from the title.
void set_text_file(string path)
{
  text_file = path;
  title = handler("books")->query_title(path);

  if (title && strlen(title))
  {
    set_short(_LANG_BOOK_SHORT_TITLED);
    set_long(_LANG_BOOK_LONG_TITLED);
  }
}

string query_text_file() { return text_file; }
string query_title() { return title; }

// A book says it has something written, the way anything with writing does.
string long(varargs string str, int dark)
{
  return ::long(str, dark) + _LANG_BOOK_HAS_TEXT;
}

mapping query_auto_load_attributes()
{
  return ([ "::" : ::query_auto_load_attributes() ]) +
         (strlen(text_file) ? ([ "text file" : text_file ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
  if (!undefinedp(attribute_map["text file"]))
    set_text_file(attribute_map["text file"]);
}
