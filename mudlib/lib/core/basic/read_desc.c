/* Wonderflug 96, got rid of global initialization in declaration,
 * added a create()
 *
 * This was inherited in object.c and every object in the mud had this feature.
 *  Now it is only available in item.c, neverbot 24/08/2008
 */

/* this one also contians info on read_messages... */
#include <basic/communicate.h>
#include <translations/language.h>
#include <language.h>

static mixed *read_mess;
static int max_size, cur_size;

void create()
{
  max_size = 100;
  cur_size = 0;
}

void init()
{
  // does not inherit from anywhere
  // ::init();
  add_action("do_read", _LANG_READ_CMDS);
}

mixed add_read_mess(mixed str, varargs string lang, int size, string frame_style);

void set_max_size(int siz) { max_size = siz; }
int query_max_size() { return max_size; }

void set_cur_size(int siz) { cur_size = siz; }
int query_cur_size() { return cur_size; }

/*
 * calling this is very rude unless you are createing the object as it
 * erases all of the writing off it.
 */
void set_read_mess(mixed str, varargs string lang, int size, string frame_style)
{
  if (arrayp(str))
  {
    read_mess = str;
    return;
  }

  if (!lang)
    lang = STD_LANG;
  if (!size)
    size = 1;
  if (!frame_style)
    frame_style = "";

  // only allow read_mess with written languages
  if (!handler("languages")->query_language_written(lang))
    read_mess = ({ });
  else if (stringp(str) && strlen(str))
    read_mess = ({ ({ str, lang, size, frame_style }) });
  else
    read_mess = ({ });
}

string *query_read_mess() { return read_mess; }

/* This adds a new messages onto the object. */
mixed add_read_mess(mixed str, varargs string lang, int size, string frame_style)
{
  int de_size;
  object handler;

  handler = handler("languages");

  // only allow read_mess with written languages
  if (!handler->query_language_written(lang))
    return "";

  /* fail! */
  if (cur_size >= max_size)
    return "";
  if (!lang)
    lang = STD_LANG;
  if (!size)
    size = 1;
  if (!frame_style)
    frame_style = "";

  de_size = size*(handler->query_language_size(lang, str));

  if (cur_size + de_size > max_size)
  {
    /* Try and squidge the text on somehow... */
    str = (string)LANGUAGE_HANDLER->squidge_text(lang, str,
                (max_size-cur_size)/size);

    if (!strlen(str))
      return "";

    cur_size += size*(handler->query_language_size(lang, str));
  }

  if (!read_mess)
    read_mess = ({ ({ str, lang, size, frame_style }) });
  else
    read_mess += ({ ({ str, lang, size, frame_style }) });
  return str;
}

/*
 * If you know the actual message or the language you can remove
 * that message.
 *
 * Using the language is a very dodgy way of doing this. Using
 * a combination is much better.
 */
int remove_read_mess(string str, string lang)
{
  int i;

  for (i = 0; i < sizeof(read_mess); i++)
  {
    if (str && read_mess[i][READ_STR] != str)
      continue;
    if (lang && read_mess[i][READ_LANG] != lang)
      continue;

    cur_size -= read_mess[i][READ_SIZE] *
              (handler("languages")->query_language_size(lang, read_mess[i][READ_STR]));
    read_mess = delete(read_mess, i, 1);
    return 1;
  }
  
  // Nup.  We traversed it all...  and there was nothing!
  return 0;
}

/*
 * This is used by the do_read procedure to create the message that
 * you will end up reading.  Useful huh?
 *
 * string create_read_array(object ob)
 * {
 *    mixed *bing;
 *    int i;
 *    string ret;
 *
 *    bing = (mixed *)ob->query_read_mess();
 *    ret = "";
 *    if (!bing || !sizeof(bing)) return "No hay nada escrito en "+ob->short()+".\n";
 *    for (i=0;i<sizeof(bing);i++)
 *      ret += (string)this_player()->read_message(bing[i][READ_STR],
 *                                                 bing[i][READ_LANG],
 *                                                 bing[i][READ_SIZE],
 *                                                 bing[i][READ_FRAME]);
 *    return ret;
 * }
 */

/*
 * Yeppers, this actually reads the object. Handles reading of actual
 * messages.
 */
int do_read(string what)
{
  string s1, s2, s3, str, str2;
  // neverbot 4/2003
  string ret;
  int i;

  if (!this_object()->id(what))
  {
    notify_fail(_LANG_READ_READ_WHAT);
    return 0;
  }

  if (!read_mess)
    return 0;

  if (read_mess)
  {
    for (i = 0; i < sizeof(read_mess); i++)
    {
      str = read_mess[i][READ_STR];

      /* Its not a string when we are dealing with magical writing */
      if (stringp(str))
      {
        str2 = "";
        while (sscanf(str, "%s$$%s$$%s", s1, s2, s3) == 3)
        {
          str2 += s1 + read_file(s2);
          str = s3;
        }

        ret = (string)this_player()->read_message(str2 + str,
          read_mess[i][READ_LANG],
          read_mess[i][READ_SIZE],
          read_mess[i][READ_FRAME]);
      }
      else
      {
        /* It magic!  She blinded me with science! */
        ret = (string)this_player()->read_message(str,
          read_mess[i][READ_LANG],
          read_mess[i][READ_SIZE],
          read_mess[i][READ_FRAME]);
      }
    }
  }

  // neverbot 4/2003
  if (strlen(ret))
    write(ret);

  return 1;
} /* do_read() */

/* Modify the long if we have something written on us */
string long(varargs string str, int dark)
{
  if (read_mess)
    return _LANG_READ_SOMETHING_WRITTEN;
  return "";
}

// stats added
mixed stats()
{
  return ({
    ({ "Read Mess (nosave)", read_mess, }),
    ({ "Max Size (nosave)", max_size, }),
    ({ "Cur Size (nosave)", cur_size, }),
  });
}
