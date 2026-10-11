// Books handler: reads the text of a book, splits it into pages and pages
// it for whoever reads it.
//
// A book's text is a markdown file, and the file is the book: its path is
// what a book item, a library shelf and the reader's encyclopedia keep. The
// file is written like this:
//
//   # The title of the book
//
//   A paragraph, as long as it needs; single line breaks inside it are
//   ignored, and it is wrapped to the reader's columns.
//
//   ## A heading
//
//   - a list item
//
//   ---
//
//   The next page starts after a line that holds only three dashes.
//
// `**words**` are shown in bold, and `[[words|id]]` (a link to another
// text) shows only its words.
//
// Ported from the CcMud books and their text format, Folken 10/2009 and
// 03/2010.

#include <user/exploration.h>
#include <language.h>

inherit "/lib/core/object.c";

#define BOOK_PAGE_BREAK "---"

// path -> ({ title, ({ page, page... }) }), each page its raw markdown
static mapping books;

void create()
{
  ::create();
  books = ([ ]);
}

// Read and split a book's text, once; nil when the file is missing or empty.
private mixed * _load(string path)
{
  string text, title, page;
  string * lines, * pages;
  int i;

  if (!path || !strlen(path))
    return nil;
  if (books[path])
    return books[path];
  if (!file_exists(path))
    return nil;

  text = read_file(path);
  if (!text || !strlen(text))
    return nil;

  lines = explode(text, "\n");
  title = "";
  pages = ({ });
  page = "";

  for (i = 0; i < sizeof(lines); i++)
  {
    // the first top-level heading is the title, not part of any page
    if (!strlen(title) && lines[i][0..1] == "# ")
    {
      title = lines[i][2..];
      continue;
    }

    if (lines[i] == BOOK_PAGE_BREAK)
    {
      pages += ({ page });
      page = "";
      continue;
    }

    page += lines[i] + "\n";
  }
  pages += ({ page });

  books[path] = ({ title, pages });
  return books[path];
}

// Forget what was read of a book, so the next reading takes the file anew.
void reload_book(string path)
{
  if (path)
    books[path] = nil;
}

string query_title(string path)
{
  mixed * book;

  book = _load(path);
  return book ? book[0] : nil;
}

int query_page_count(string path)
{
  mixed * book;

  book = _load(path);
  return book ? sizeof(book[1]) : 0;
}

// Bold words and the words of a link, inside one line of text.
private string _inline(string line)
{
  string a, b, c;

  while (sscanf(line, "%s**%s**%s", a, b, c) == 3)
    line = a + "%^BOLD%^" + b + "%^RESET%^" + c;

  while (sscanf(line, "%s[[%s]]%s", a, b, c) == 3)
  {
    // [[words|id]] shows its words, [[words]] the same
    sscanf(b, "%s|%*s", b);
    line = a + b + c;
  }

  return line;
}

// The columns a word takes on screen: its colour codes take none.
private int _visible_length(string word)
{
  string a, b, c;

  while (sscanf(word, "%s%%^%s%%^%s", a, b, c) == 3)
    word = a + c;

  return strlen(word, true);
}

// Wrap a block of text to `width`, word by word, every row starting with
// `first` and the rows after the first with `rest`. The wrap efun would count
// the colour codes as text and leave the rows that carry them short.
private string _block(string text, int width, string first, string rest)
{
  string * words;
  string out, row;
  int i, room, used, len;

  words = explode(_inline(text), " ") - ({ "" });
  out = "";
  row = "";
  used = 0;

  for (i = 0; i < sizeof(words); i++)
  {
    room = width - strlen(strlen(out) ? rest : first);
    len = _visible_length(words[i]);

    if (used && used + 1 + len > room)
    {
      out += (strlen(out) ? rest : first) + row + "\n";
      row = "";
      used = 0;
    }

    row += (used ? " " : "") + words[i];
    used += (used ? 1 : 0) + len;
  }

  if (used)
    out += (strlen(out) ? rest : first) + row + "\n";

  return out;
}

// One page as the reader sees it: paragraphs wrapped to `width`, headings in
// bold, list items with a hanging indent, a blank line between blocks.
private string _render(string page, int width)
{
  string * lines, * blocks;
  string paragraph;
  int i;

  lines = explode(page, "\n");
  blocks = ({ });
  paragraph = "";

  for (i = 0; i <= sizeof(lines); i++)
  {
    string line;

    line = (i < sizeof(lines)) ? lines[i] : "";

    // a blank line, a heading or a list item ends the paragraph in progress
    if (!strlen(line) || line[0..2] == "## " || line[0..1] == "- " ||
        line[0..1] == "* ")
    {
      if (strlen(paragraph))
        blocks += ({ _block(paragraph, width, "  ", "  ") });
      paragraph = "";
    }

    if (!strlen(line))
      continue;

    if (line[0..2] == "## ")
      blocks += ({ "%^BOLD%^" + _inline(line[3..]) + "%^RESET%^\n" });
    else if (line[0..1] == "- " || line[0..1] == "* ")
      blocks += ({ _block(line[2..], width, "  - ", "    ") });
    else
      paragraph += (strlen(paragraph) ? " " : "") + line;
  }

  return implode(blocks, "\n");
}

private int _reader_width()
{
  int cols;

  cols = this_user() ? (int)this_user()->query_cols() : 79;
  return (cols > 20) ? cols - 2 : 78;
}

// The line under every page: where the reader is and what they can type.
private string _footer(int page, int pages)
{
  string text;
  int width, gap;

  text = _LANG_BOOKS_FOOTER;
  width = _reader_width();
  gap = (width - strlen(text, true)) / 2;

  return (gap > 0 ? sprintf("%" + gap + "s", "") : "") + text + "\n";
}

// The game a book belongs to: the one its text lives under, or else the
// reader's, so a book of the mudlib is filed in the game it was read in.
private string _book_game(string path, object reader)
{
  string game;

  game = game_from_path(path);
  if (!game || !strlen(game))
    game = game_name(reader);

  return game;
}

// Show one page and wait for what the reader types next. Reaching the last
// page files the book in the reader's encyclopedia.
void show_page(object reader, string path, int page)
{
  mixed * book;
  int pages;

  book = _load(path);
  if (!book || !reader)
    return;

  pages = sizeof(book[1]);
  if (page < 0)
    page = 0;
  if (page >= pages)
    page = pages - 1;

  tell_object(reader, "\n" + _render(book[1][page], _reader_width()) + "\n" +
                      _footer(page, pages));

  if (page == pages - 1 &&
      reader->add_encyclopedia_article(_book_game(path, reader), path))
    tell_object(reader, _LANG_BOOKS_FILED);

  input_to("reader_input", 0, path, page);
}

// Open a book for a reader at its first page.
int start_reading(object reader, string path)
{
  mixed * book;

  book = _load(path);
  if (!reader)
    return 0;

  if (!book || !sizeof(book[1]))
  {
    tell_object(reader, _LANG_BOOKS_UNREADABLE);
    return 0;
  }

  tell_object(reader, _LANG_BOOKS_OPEN);
  if (environment(reader))
    tell_room(environment(reader), _LANG_BOOKS_OPEN_OTHERS, ({ reader }));

  show_page(reader, path, 0);
  return 1;
}

// What the reader typed while the book was open: the next page by default,
// the previous one, a page by its number, help, or closing it.
void reader_input(string str, string path, int page)
{
  object reader;
  int pages, num;

  reader = this_player();
  pages = query_page_count(path);

  if (!reader || !pages)
    return;

  if (!str)
    str = "";
  str = lower_case(str);

  if (str == "" || str == "+")
  {
    if (page + 1 >= pages)
    {
      tell_object(reader, _LANG_BOOKS_CLOSE);
      return;
    }
    show_page(reader, path, page + 1);
    return;
  }

  if (str == "-")
  {
    show_page(reader, path, page - 1);
    return;
  }

  if (sscanf(str, "%d", num) == 1)
  {
    if (num >= 1 && num <= pages)
    {
      show_page(reader, path, num - 1);
      return;
    }
    tell_object(reader, _LANG_BOOKS_NO_SUCH_PAGE);
    input_to("reader_input", 0, path, page);
    return;
  }

  if (member_array(str, _LANG_BOOKS_HELP_KEYS) != -1)
  {
    tell_object(reader, _LANG_BOOKS_HELP);
    input_to("reader_input", 0, path, page);
    return;
  }

  // anything else closes the book: q, or a command the reader meant to type
  tell_object(reader, _LANG_BOOKS_CLOSE);
}
