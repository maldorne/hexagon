/* Hamlet, Mar 1997.
   Removes any unwanted words from text.
*/

#include <basic/communicate.h>

/* This file should be of the form:
      bad thing -> replacement

   One pair per line.  Either one (or both) may be a word or phrase.
   'bad thing' should always be entirely lowercase.
*/

string * bad_words;
int fsize;

void read_init_file();

void create() 
{
  read_init_file();
}

/* Call this one to replace stuff... */
string clean_language(string str)
 {
  string lowered;
  int i, loc;

  if (!str || !strlen(str))
    return str;

  if (file_size(PROFANITY_TABLE) != fsize)
    read_init_file();

  if (!bad_words || !sizeof(bad_words))
    return str;

  lowered = lower_case(str);
  i = 0; /* paranoia */

  while (i < sizeof(bad_words)) 
  {
    if ( (loc = strsrch(lowered, bad_words[i])) != -1 ) 
    {
      str = str[0..loc-1] + bad_words[i + 1] + 
            str[loc+strlen(bad_words[i])..strlen(str)-1];
      lowered = lowered[0..loc-1] + bad_words[i + 1] + 
            lowered[loc+strlen(bad_words[i])..strlen(lowered)-1];
    }
    else
      i += 2;
  }

  return str;      
}

void dest_me() 
{
  read_init_file();
}

void read_init_file() 
{
  string text;
  string *lines;
  int i,j;
  string *pair;

  fsize = file_size(PROFANITY_TABLE);

  if (!fsize)  
    return;

  text = read_file(PROFANITY_TABLE);
  if (!text)  
    return;
  lines = explode(text, "\n");

  bad_words = allocate(sizeof(lines) * 2);

  j = 0;  /* paranoia */

  for (i = 0; i < sizeof(lines); i++) 
  {
    pair = explode(lines[i], "->");

    if (sizeof(pair) != 2) /* Skip it */
      continue;

    bad_words[j] = lower_case(implode((explode(pair[0], " ") - ({ "" })), " "));
    bad_words[j+1] = implode((explode(pair[1], " ") - ({ "" })), " ");

    if (bad_words[j] == bad_words[j+1])  /* remove this pair, it's bad */
      continue;

    j+=2;
  }

  if (j != sizeof(bad_words))
    bad_words = bad_words[0..j-2];
}

string *show_wordlist() 
{
  return bad_words;
}
