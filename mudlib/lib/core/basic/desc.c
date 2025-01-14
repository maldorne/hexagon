
#include <language.h>

static string short_d,
       plural_d,
       long_d;

void create()
{
  short_d = "";
  plural_d = "";
  long_d = "";
}

// Changed implode(explode(.. to replace_string but screwed it up
// anyway - Radix, Wonderflug fixed his drool : Jan 11, 1997
void set_short(string str)
{
  if (stringp(str)) 
    str = replace_string(str,"\n","");
  short_d = str;
  return;
}

void set_long(string str) { long_d = str; }
void set_main_plural(string str) { plural_d = str; }

string query_short() { return short_d; }
string query_long() { return long_d; }
string query_main_plural() { return plural_d; }

string short(varargs int dark) 
{
  return query_short(); 
}

string pretty_short(varargs int dark) 
{
  return (string)this_object()->short(dark);
}

string long(varargs string str, int dark) 
{
  if (!query_long())
    return _LANG_NO_LONG;
        
  return query_long();
}

string query_plural() 
{
  if (!query_main_plural())
    if (!short(0))
      return nil;
    else
      return pluralize(short(0));
  return query_main_plural();
}

mixed pretty_plural() 
{
  return query_plural();
}

// Stats added
mixed * stats()
{
	return ({
		({ "Short (nosave)", short_d, }),
		({ "Plural (nosave)", plural_d, }),
		({ "Long (nosave)", long_d, }),
	      });
}
