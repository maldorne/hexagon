/* Wonderflug 96, got rid of global initialization in declaration,
 * added a create()
 *
 * Antiguamente esto se heredaba en object.c y todos los objetos del mud
 *  tenian esta funcionalidad. Extraido ahora de la cadena de herencias y solo
 *  disponible en item.c, Folken 24/08/2008
 */

/* this one also contians info on read_messages... */
#include <basic/communicate.h>

static mixed *read_mess;
static int max_size, cur_size;

void create()
{
  max_size = 100;
  cur_size = 0;
}

mixed add_read_mess(mixed str, string type, string lang, int size);

void set_max_size(int siz) { max_size = siz; }
int query_max_size() { return max_size; }

void set_cur_size(int siz) { cur_size = siz; }
int query_cur_size() { return cur_size; }

void add_read_this(object ob) {
  ob->add_command("leer", this_object());
} /* add_read_this() */

/*
* calling this is very rude unless you are createing the object as it
* erases all of the writing off it.
*/
void set_read_mess(mixed str, varargs string lang, int size) 
{
  if (pointerp(str)) 
  {
    read_mess = str;
    return ;
  }

  if (!read_mess && str && environment()) 
  {
    filter_array(all_inventory(environment()), "add_read_this", this_object());
    environment()->add_command("leer", this_object());

  }

  if (!lang)
    lang = "comun";
  if (!size)
    size = 1;

  if (str)
    read_mess = ({ ({ str, 0, lang, size }) });
  else
    read_mess = ({ });
} /* set_read_mess() */

string *query_read_mess() { return read_mess; }

/* This adds a new messages onto the object. */
mixed add_read_mess(mixed str, string type, string lang, int size) 
{
  int de_size;

  /* fail! */
  if (cur_size >= max_size) 
    return "";
  if (!size) 
    size = 1;

  if (!read_mess && environment()) 
  {
    filter_array(all_inventory(environment()), "add_read_this", this_object());
    environment()->add_command("leer", this_object());
  }

  de_size = size*((int)LANGUAGE_HANDLER->query_language_size(lang, str));

  if (cur_size+de_size > max_size) 
  {
    /* Try and squidge the text on somehow... */
    str = (string)LANGUAGE_HANDLER->squidge_text(lang, str,
                (max_size-cur_size)/size);

    if (!strlen(str))
      return "";
    cur_size += size*((int)LANGUAGE_HANDLER->query_language_size(lang, str));
  }

  if (!read_mess)
    read_mess = ({ ({ str, type, lang, size }) });
  else
    read_mess += ({ ({ str, type, lang, size }) });
  return str;
} /* add_read_mess() */

/*
* If you know the actual message or the language or the type you can remove
* that message.
*
* Using the type and or language is a very dodgy way of doing this.  Using
* a combination is much better.
*/
int remove_read_mess(string str, string type, string lang) 
{
  int i;

  for (i = 0; i < sizeof(read_mess); i++) 
  {
    if (str && read_mess[i][READ_STR] != str)
      continue;
    if (type && read_mess[i][READ_TYPE] != type)
      continue;
    if (lang && read_mess[i][READ_LANG] != lang)
      continue;

    cur_size -= read_mess[i][READ_SIZE] *
              ((int)LANGUAGE_HANDLER->query_language_size(lang, read_mess[i][READ_STR]));
    read_mess = delete(read_mess, i, 1);
    return 1;
  }
  /* Nup.  We traversed it all...  and there was nothing! */
  return 0;
} /* remove_read_mess() */

void init() 
{
  if (read_mess)
  {
    this_player()->add_command("leer", this_object());
    environment()->add_command("leer", this_object());
  }
} /* init() */

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
*                                                 bing[i][READ_TYPE],
*                                                 bing[i][READ_LANG],
*                                                 bing[i][READ_SIZE]);
*    return ret;
* }
*/

/*
* Yeppers, this actually reads the object.  Handles reading of actual
* messages and labels.
*/

// Folken 4/2003, despues de traducir el comando 'read' por 'leer' a algun
// inteligente se le olvido cambiar la funcion tambien (el sistema busca automaticamente
// la funcion 'do_' + verbo :(
// mixed do_read() {
mixed do_leer() 
{
  string s1, s2, s3, str, str2;
  // Folken 4/2003
  string ret;
  int i;

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
        /*
        write((string)this_player()->read_message(str2 + str,
        read_mess[i][READ_TYPE],
        read_mess[i][READ_LANG],
        read_mess[i][READ_SIZE]));
        */
        ret = (string)this_player()->read_message(str2 + str,
        read_mess[i][READ_TYPE],
        read_mess[i][READ_LANG],
        read_mess[i][READ_SIZE]);
      } 
      else 
      {
        /* It magic!  She blinded me with science! */
        /*
        write((string)this_player()->read_message(str,
        read_mess[i][READ_TYPE],
        read_mess[i][READ_LANG],
        read_mess[i][READ_SIZE]));
        */
        ret = (string)this_player()->read_message(str,
        read_mess[i][READ_TYPE],
        read_mess[i][READ_LANG],
        read_mess[i][READ_SIZE]);
      }
    }
  }

  /*
  * Ok...  now we stick all the label info in...
  * All this stuff is useless
  *
  * labels = labels - ({ 0 });
  * if (sizeof(labels))
  * {
  *   str = implode(map_array("create_read_array", this_object()), "");
  *   write(str);
  *   if (!read_mess)
  *     if ((s1 = (string)this_object()->query_property("read id")))
  *       return query_multiple_short(labels)+" en "+s1;
  *     else
  *       return query_multiple_short(labels)+" en "+short(0);
  * }
  * if ((s1 = (string)this_object()->query_property("read id")))
  *   return s1;
  */

  // Folken 4/2003
  if (ret && (ret != "")) 
  {
    ret = sprintf("%*-=s",
              (this_player()?this_player()->query_cols():79),
              ret);
    write(ret);
    return this_object();
  }
  return 1;
} /* do_read() */

/* Modify the long if we have something written on us */
string long(varargs string str, int dark) 
{
  /*
  if (read_mess)
    return ::long(str) + "Parece que tiene algo escrito.\n";
  return ::long(str, dark);
  */
  if (read_mess)
    return "Parece que tiene algo escrito.\n";
  return "";
}

/* global/basic/read_desc: */
/*
mixed *query_init_data()
{
return ::query_init_data() +
({ "read_mess", read_mess, "set_read_mess/P0/P2/","" });
}
*/

// Stats añadido
mixed stats()
{
  return ({
    ({ "Read Mess (nosave)", read_mess, }),
    ({ "Max Size (nosave)", max_size, }),
    ({ "Cur Size (nosave)", cur_size, }),
  });
}
