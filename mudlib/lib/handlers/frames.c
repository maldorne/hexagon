
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
  //    [0] -> extra width in each side           STYLE_EXTRA_WIDTH
  //    [1] -> extra height in each side          STYLE_EXTRA_HEIGHT
  //    [2] -> extra width padding in each side   STYLE_EXTRA_WIDTH_PADDING
  //    [3] -> extra height padding in each side  STYLE_EXTRA_HEIGHT_PADDING
  //    ...
  //    [6] -> STYLE_UP_LEFT_CORNER
  //    [7] -> STYLE_UP_RIGHT_CORNER
  //    [8] -> STYLE_DOWN_RIGHT_CORNER
  //    [9] -> STYLE_DOWN_LEFT_CORNER
  //    ...

  styles[DEFAULT_FRAME_STYLE] = ({ 2, 2, 2, 2,
              "| ", " |",
              "┌", "┐", "┘", "└",
              "-", "-" });

  // exec return char_codes("═╛") to see the char sequence
  // exec return handler("frames")->query_styles() to see the final characters

  styles["notifications"] = ({ 1, 1, 1, 1,
              chr(226) + chr(148) + chr(130) + "  ", // "│  "
              "  " + chr(226) + chr(148) + chr(130), // "  │"
              "" + chr(226) + chr(149) + chr(146) + chr(226) + chr(149) + chr(144), // "╒═"
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(149), // "═╕",
              "" + chr(226) + chr(149) + chr(144) + chr(226) + chr(149) + chr(155), // "═╛",
              "" + chr(226) + chr(149) + chr(152) + chr(226) + chr(149) + chr(144) + 
                   chr(226) + chr(149) + chr(149) + "/", // "╘═╕/",
              chr(226) + chr(149) + chr(144), // "═"
              chr(226) + chr(149) + chr(144), // "═"
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
      left += padding_str;
      odd = false;
    }
    else
    {
      right = padding_str + right;
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
  string margin_str;
  margin_str = "";

  while (strlen(margin_str) < margin)
    margin_str += " ";

  while (visible_strlen(margin_str + left + content + right + margin_str) < width)
    right = " " + right;

  return margin_str + left + content + right + margin_str + "\n";
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

  if (!width)
  {
    // width = DEFAULT_WIDTH;
    int max_length, len;
    // minimum, the header of the frame
    max_length =  visible_strlen(title) +  
      (visible_strlen(style[STYLE_LEFT_PAD]) + visible_strlen(style[STYLE_RIGHT_PAD])) +
      style[STYLE_EXTRA_WIDTH] * 2;

    for (i = 0; i < sizeof(lines); i++)
    {
      if ((len = strlen(lines[i])) > max_length)
        max_length = len;
    }

    width = max_length + 
      (visible_strlen(style[STYLE_UP_LEFT_CORNER]) + visible_strlen(style[STYLE_UP_RIGHT_CORNER])) +
      (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2;
  }

  if (!height)
    height = DEFAULT_HEIGHT;

  // if (strlen(title) > width - style[STYLE_EXTRA_WIDTH] * 2)
  //   title = title[0..width - style[STYLE_EXTRA_WIDTH] * 2 - 1];

  retval += _header(style[STYLE_UP_LEFT_CORNER], 
    title, 
    style[STYLE_UP_RIGHT_CORNER], 
    width, 
    style[STYLE_EXTRA_WIDTH], 
    style[STYLE_UP_PAD]);

  // retval += sprintf(style[STYLE_UP_LEFT_CORNER] + "%p%|*s" + style[STYLE_UP_RIGHT_CORNER] + "\n",
  //   style[STYLE_UP_PAD],
  //   width - style[STYLE_EXTRA_WIDTH] * 2 + (strlen(title) - visible_strlen(title)),
  //   title);

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += _spacer(style[STYLE_LEFT_PAD],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH],
      style[STYLE_EXTRA_WIDTH_PADDING]);

    // retval += sprintf(style[STYLE_LEFT_PAD] + "%p%|*s" + style[STYLE_RIGHT_PAD] + "\n",
    //   ' ',
    //   width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2,
    //   "");
  }

  for (i = 0; i < sizeof(lines); i++)
  {
    lines[i] = fix_string(lines[i]);

    retval += _line(style[STYLE_LEFT_PAD],
      lines[i],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH],
      style[STYLE_EXTRA_WIDTH_PADDING]);

    // crop lines if needed, add padding if needed
    // retval += sprintf(style[STYLE_LEFT_PAD] + "%-*s" + style[STYLE_RIGHT_PAD] + "\n",
    //   width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2 + (strlen(lines[i]) - visible_strlen(lines[i])),
    //   lines[i]);
  }

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += _spacer(style[STYLE_LEFT_PAD],
      style[STYLE_RIGHT_PAD],
      width,
      style[STYLE_EXTRA_WIDTH],
      style[STYLE_EXTRA_WIDTH_PADDING]);

    // retval += sprintf(style[STYLE_LEFT_PAD] + "%p%|*s" + style[STYLE_RIGHT_PAD] + "\n",
    //   ' ',
    //   width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2,
    //   "");
  }

  retval += _footer(style[STYLE_DOWN_LEFT_CORNER], 
    style[STYLE_DOWN_RIGHT_CORNER], 
    width, 
    style[STYLE_EXTRA_WIDTH], 
    style[STYLE_DOWN_PAD]);

  // retval += sprintf(style[STYLE_DOWN_LEFT_CORNER] + "%p%|*s" + style[STYLE_DOWN_RIGHT_CORNER] + "\n",
  //   style[STYLE_DOWN_PAD],
  //   (width - style[STYLE_EXTRA_WIDTH] * 2),
  //   "");

  return retval;
}
