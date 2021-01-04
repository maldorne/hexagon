
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
  //    [0] -> extra width in each side
  //    [1] -> extra height in each side
  // important!! the last two are characters, not strings!

  styles[DEFAULT_FRAME_STYLE] = ({ 1, 1, 1, 1,
              "| ", " |",
              "┌", "┐", "┘", "└",
              '-', '-' });
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
    int max_length, len;
    max_length = 0;

    // width = DEFAULT_WIDTH;
    for (i = 0; i < sizeof(lines); i++)
    {
      if ((len = strlen(lines[i])) > max_length)
        max_length = len;
    }

    width = max_length + (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2;
  }

  if (!height)
    height = DEFAULT_HEIGHT;

  // if (strlen(title) > width - style[STYLE_EXTRA_WIDTH] * 2)
  //   title = title[0..width - style[STYLE_EXTRA_WIDTH] * 2 - 1];

  retval += sprintf(style[STYLE_UP_LEFT_CORNER] + "%p%|*s" + style[STYLE_UP_RIGHT_CORNER] + "\n",
    style[STYLE_UP_PAD],
    width - style[STYLE_EXTRA_WIDTH] * 2 + (strlen(title) - visible_strlen(title)),
    title);

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += sprintf(style[STYLE_LEFT_PAD] + "%p%|*s" + style[STYLE_RIGHT_PAD] + "\n",
      ' ',
      width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2,
      "");
  }

  for (i = 0; i < sizeof(lines); i++)
  {
    lines[i] = fix_string(lines[i]);

    // crop lines if needed, add padding if needed
    retval += sprintf(style[STYLE_LEFT_PAD] + "%-*s" + style[STYLE_RIGHT_PAD] + "\n",
      width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2 + (strlen(lines[i]) - visible_strlen(lines[i])),
      lines[i]);
  }

  // vertical padding inside the frame
  for (i = 0; i < style[STYLE_EXTRA_HEIGHT_PADDING]; i++)
  {
    retval += sprintf(style[STYLE_LEFT_PAD] + "%p%|*s" + style[STYLE_RIGHT_PAD] + "\n",
      ' ',
      width - (style[STYLE_EXTRA_WIDTH] + style[STYLE_EXTRA_WIDTH_PADDING]) * 2,
      "");
  }

  retval += sprintf(style[STYLE_DOWN_LEFT_CORNER] + "%p%|*s" + style[STYLE_DOWN_RIGHT_CORNER] + "\n",
    style[STYLE_DOWN_PAD],
    (width - style[STYLE_EXTRA_WIDTH] * 2),
    "");

  return retval;
}
