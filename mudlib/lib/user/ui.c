
// #include <user/ui.h>
#include <language.h>

#define BLANK_LINE "                "

// sidebar activated true/false 
// why static??? next connection could be from a non vt100 terminal, and
// that would be chaos in the screen
static int sidebar;
static int _drawing;

// is any special ui feature in use?
int ui_activated() { return sidebar && true; }

int ui_query_sidebar() { return sidebar; }

int ui_in_use() { return _drawing; }

void create()
{
  sidebar = false;
}

void ui_commands()
{
  add_action("do_sidebar", "sidebar");
}

int do_sidebar(string str)
{
  if (!strlen(str))
  {
    notify_fail(_LANG_UI_SIDEBAR_SYNTAX);
    return 0;
  }
  
  if (str == "on")
    sidebar = true;
  if (str == "off")
    sidebar = false;

  return 1;
}

void _draw_lines(string * lines, int x, int y)
{
  int i;
  string position;

  for (i = 0; i < sizeof(lines); i++)
  {
    position = sprintf("%c[%d;%df", 27, x+i, y);
    this_object()->send_message(position + lines[i]);
  }
}

void ui_redraw()
{
  string * pieces;
  int i; //, x, y;
  int lines_drawn;

  if (!this_object()->player())
    return;

  _drawing = true;
  lines_drawn = 0;

  // save cursor position
  // this_object()->send_message(sprintf("%c[s", 27));
  // save cursor position and attributes
  this_object()->send_message(sprintf("%c7", 27));

  if (sidebar)
  {
    string text, path;

    // draw separator
    _draw_lines(({ BLANK_LINE }), lines_drawn + 1, this_object()->query_cols() - 12);
    lines_drawn++;

    // draw worldmap
    path = doc("fake_map.txt");
    text = read_file(path);

    if (text)
    {
      text = handler("frames")->frame(text, "] " + _LANG_UI_MAP + " [", 16, 0, "sidebar");
      pieces = explode(text, "\n");

      _draw_lines(pieces, lines_drawn + 1, this_object()->query_cols() - 12);
      lines_drawn += sizeof(pieces);
    }

    // draw separator
    _draw_lines(({ BLANK_LINE }), lines_drawn + 1, this_object()->query_cols() - 12);
    lines_drawn++;

    // draw health bars
    text = "HPs:\n" + 
      bar(this_object()->player()->query_hp() * 100 / this_object()->player()->query_max_hp()) + 
      "\nGPs:\n" + 
      bar(this_object()->player()->query_gp() * 100 / this_object()->player()->query_max_gp()) + 
      "\n";

    text = handler("frames")->frame(text, "] " + _LANG_UI_STATS + " [", 16, 0, "sidebar");
    pieces = explode(text, "\n");

    _draw_lines(pieces, lines_drawn + 1, this_object()->query_cols() - 12);
    lines_drawn += sizeof(pieces);
  }

  // restore cursor position
  // this_object()->send_message(sprintf("%c[u", 27));
  // restore cursor position and attributes
  this_object()->send_message(sprintf("%c8", 27));

  _drawing = false;
}

mixed * stats()
{
  return ({
    ({ "UI Sidebar", sidebar, }),
          });
}
