
/*
 * windows, frames, etc
 *
 * just strings management to make them pretty
 *
 */

#include <common/frames.h>

inherit "/lib/core/object.c";

mapping styles;

void create()
{
  ::create();
  styles = ([ ]);

  // each style is defined by an array of:
  //    [0] -> extra width in each side           STYLE_EXTRA_WIDTH_MARGIN
  //    [1] -> extra height in each side          STYLE_EXTRA_HEIGHT_MARGIN <------- NOT USED BY NOW
  //    [2] -> extra width padding in each side   STYLE_EXTRA_WIDTH_PADDING
  //    [3] -> extra height padding in each side  STYLE_EXTRA_HEIGHT_PADDING
  //    ...
  //    [6] -> STYLE_UP_LEFT_CORNER
  //    [7] -> STYLE_UP_RIGHT_CORNER
  //    [8] -> STYLE_DOWN_RIGHT_CORNER
  //    [9] -> STYLE_DOWN_LEFT_CORNER
  //    ...
  //    [12] -> STYLE_TITLE_LEFT
  //    [13] -> STYLE_TITLE_RIGHT

  styles["simple"] = ({ 1, 1, 2, 1,
              "|", "|",
              "", "", "", "",
              "-", "-",
              "| ", " |" });

  styles[DEFAULT_FRAME_STYLE] = ({ 1, 1, 2, 1,
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(148) + chr(140), // "┌", 
              "" + chr(226) + chr(148) + chr(144), // "┐", 
              "" + chr(226) + chr(148) + chr(152), // "┘", 
              "" + chr(226) + chr(148) + chr(148), // "└",
              "" + chr(226) + chr(148) + chr(128), // "─"
              "" + chr(226) + chr(148) + chr(128), // "─"
              "" + chr(226) + chr(148) + chr(164) + " ", // "┤ ",
              " " + chr(226) + chr(148) + chr(156), // " ├" 
            });

  // exec return char_codes("═╛") to see the char sequence
  // exec return handler("frames")->query_styles() to see the final characters

  styles["alert"] = ({ 1, 1, 3, 1,
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(149) + chr(146) + chr(226) + chr(149) + chr(144), // "╒═"
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(149), // "═╕",
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(155), // "═╛",
              "" + chr(226) + chr(149) + chr(152) + chr(226) + chr(149) + chr(144), // "╘═",
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(149) + chr(161) + " ", // "╡ ",
              " " + chr(226) + chr(149) + chr(158), // " ╞" 
            });

  styles["notifications"] = ({ 1, 1, 3, 1,
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(148) + chr(130), // "│"
              "" + chr(226) + chr(149) + chr(146) + chr(226) + chr(149) + chr(144), // "╒═"
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(149), // "═╕",
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(155), // "═╛",
              "" + chr(226) + chr(149) + chr(152) + chr(226) + chr(149) + chr(144) + 
                   chr(226) + chr(149) + chr(149) + "/", // "╘═╕/",
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(149) + chr(161) + " ", // "╡ ",
              " " + chr(226) + chr(149) + chr(158), // " ╞" 
            });

  styles["sidebar"] = ({ 1, 0, 1, 0,
              "" + chr(226) + chr(149) + chr(145), // "║"
              "" + chr(226) + chr(149) + chr(145), // "║"
              "" + chr(226) + chr(149) + chr(148) + chr(226) + chr(149) + chr(144), // "╔═"
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(151), // "═╗",
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(157), // "═╝",
              "" + chr(226) + chr(149) + chr(154) + chr(226) + chr(149) + chr(144), // "╚═",
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(149) + chr(144), // "═"
              "" + chr(226) + chr(148) + chr(164) + " ", // "┤ ",
              "" + chr(226) + chr(149) + chr(161) + " ", // "╡ ",
              " " + chr(226) + chr(149) + chr(158), // " ╞" 
            });
}

mapping query_styles() { return styles; }

string _header(string left, string title, string right, int width, int margin, string padding_str)
{
  string margin_str;
  int odd;

  margin_str = "";
  odd = true;
  
  while (strlen(margin_str) < margin)
    margin_str += " ";

  while (visible_strlen(margin_str + left + title + right + margin_str) < width)
  {
    if (odd)
    {
      right = padding_str + right;
      odd = false;
    }
    else
    {
      left += padding_str;
      odd = true;
    }
  }

  return margin_str + left + title + right + margin_str + "\n";
}

string _footer(string left, string right, int width, int margin, string padding_str)
{
  string margin_str;
  margin_str = "";
  
  while (strlen(margin_str) < margin)
    margin_str += " ";

  while (visible_strlen(margin_str + left + right + margin_str) < width)
    left += padding_str;

  return margin_str + left + right + margin_str + "\n";
}

string _spacer(string left, string right, int width, int margin, int padding)
{
  string margin_str;
  margin_str = "";

  while (strlen(margin_str) < margin)
    margin_str += " ";

  while (visible_strlen(margin_str + left + right + margin_str) < width)
    left += " ";

  return margin_str + left + right + margin_str + "\n";
}

string _line(string left, string content, string right, int width, int margin, int padding)
{
  string margin_str, padding_str;
  margin_str = "";
  padding_str = "";

  while (strlen(margin_str) < margin)
    margin_str += " ";
  while (strlen(padding_str) < padding)
    padding_str += " ";

  while (visible_strlen(margin_str + left + padding_str + content + padding_str + right + margin_str) < width)
    right = " " + right;

  return margin_str + left + padding_str + content + padding_str + right + margin_str + "\n";
}

string frame(string content, varargs string title, int width, int height, string style_name)
{
  int i;
  mixed * style;
  string * lines, retval;
  retval = "";

  if (!style_name)
    style_name = DEFAULT_FRAME_STYLE;

  style = styles[style_name];

  // unknown style, do nothing
  if (!style)
    return content;

  // wrong info in this handler, should not happen
  if (sizeof(style) < 2)
    return content;

  if (!title)
    title = "";

  // colors, etc
  title = fix_string(title);

  lines = explode(content, "\n");

  for (i = 0; i < sizeof(lines); i++)
    lines[i] = fix_string(lines[i]);

  if (!width)
  {
    // width = DEFAULT_WIDTH;
    int max_length, len;
    // minimum, the header of the frame
    max_length = visible_strlen(title) +  
      (visible_strlen(style[STYLE_LEFT_PAD]) + visible_strlen(style[STYLE_RIGHT_PAD])) +
      (style[STYLE_EXTRA_WIDTH_PADDING] + style[STYLE_EXTRA_WIDTH_MARGIN]) * 2;

    for (i = 0; i < sizeof(lines); i++)
    {
      if ((len = visible_strlen(lines[i])) > max_length)
        max_length = len;
    }

    width = max_length + 
      (visible_strlen(style[STYLE_LEFT_PAD]) + visible_strlen(style[STYLE_RIGHT_PAD])) +
      (style[STYLE_EXTRA_WIDTH_PADDING] + style[STYLE_EXTRA_WIDTH_MARGIN]) * 2;
  }

  if (!height)
    height = DEFAULT_HEIGHT;

  if (strlen(title))
    title = style[STYLE_TITLE_LEFT] + title + style[STYLE_TITLE_RIGHT];

  retval += _header(style[STYLE_UP_LEFT_CORNER], 
    title, 
    style[STYLE_UP_RIGHT_CORNER], 
    width, 
    style[STYLE_EXTRA_WIDTH_MARGIN], 
    style[STYLE_UP_PAD]);

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += _spacer(style[STYLE_LEFT_PAD],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH_MARGIN],
      style[STYLE_EXTRA_WIDTH_PADDING]);
  }

  for (i = 0; i < sizeof(lines); i++)
  {
    retval += _line(style[STYLE_LEFT_PAD],
      lines[i],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH_MARGIN],
      style[STYLE_EXTRA_WIDTH_PADDING]);
  }

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += _spacer(style[STYLE_LEFT_PAD],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH_MARGIN],
      style[STYLE_EXTRA_WIDTH_PADDING]);
  }

  retval += _footer(style[STYLE_DOWN_LEFT_CORNER], 
    style[STYLE_DOWN_RIGHT_CORNER], 
    width, 
    style[STYLE_EXTRA_WIDTH_MARGIN], 
    style[STYLE_DOWN_PAD]);

  return retval;
}
