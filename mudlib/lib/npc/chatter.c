
// code taken from monster.c, neverbot 04/2009

#include <npc/npc.h>

mixed *chat_string,    /* ({ ({ int chances }), ({ chat strings }) }) */
      *achat_string;   /* ({ ({ int chances }), ({ chat strings }) }) */
int chat_chance,       /* Chance of chatting any given round */
    achat_chance,      /* Chance of doing an attack chat any round */    
    in_talk;           /* So stuff I say doesn't get catch_talk'd */

mixed talk_string;     /* strings we shall watch for when people talk */
static string *story;        /* The current story (set of strings) */
static int which_story;      /* which bit of the story to come next */


void create()
{
  chat_string = ({ 0, ({ }) });
  achat_string = ({ 0, ({ }) });
  chat_chance = 0;
  achat_chance = 0;
  talk_string = ({ });
  which_story = 0;
  story = ({ });
  in_talk = 0;    
}

void load_chat(int chance, mixed * c_s)
{
  int i;
  for (i = 0; i < sizeof(c_s); i+=2)
  {
    chat_string[1] += ({ c_s[i], c_s[i+1] });
    chat_string[0] += c_s[i];
  }
  chat_chance = chance;
}

void set_chat_chance(int i) { chat_chance = i; }
int query_chat_chance() { return chat_chance; }
void set_chat_string(string *chat) { chat_string = chat; }
string *query_chat_string() { return chat_string; }

void add_chat_string(mixed weight, mixed chat)
{
  int i;

  if (pointerp(weight))
    for (i = 0; i < sizeof(weight); i+=2)
      add_chat_string(weight[i], weight[i+1]);
  else
    if (member_array(chat, chat_string[1]) == -1)
    {
      chat_string[1] += ({ weight, chat });
      chat_string[0] += weight;
    }
}

void remove_chat_string(mixed chat)
{
  int i;

  if (pointerp(chat))
    for (i = 0; i < sizeof(chat); i++)
      remove_chat_string(chat[i]);
  else
    if ((i = member_array(chat, chat_string[1])) != -1)
    {
      chat_string[0] -= chat_string[1][i-1];
      chat_string[1] = delete(chat_string[1], i-1, 2);
    }
}

void load_a_chat(int chance, mixed * c_s) 
{
  int i;
  for (i = 0; i < sizeof(c_s); i+=2)
  {
    achat_string[1] += ({ c_s[i], c_s[i+1] });
    achat_string[0] += c_s[i];
  }
  achat_chance = chance;
}

void set_achat_chance(int i) { achat_chance = i; }
int query_achat_chance() { return achat_chance; }
void set_achat_string(string *chat) { achat_string = chat; }
string *query_achat_string() { return achat_string; }

void add_achat_string(mixed weight, mixed chat)
{
  int i;

  if (pointerp(weight))
    for (i=0;i<sizeof(weight);i+=2)
      add_achat_string(weight[i], weight[i+1]);
  else
    if (member_array(chat, achat_string[1]) == -1)
    {
      achat_string[1] += ({ chat });
      achat_string[0] += weight;
    }
}

void remove_achat_string(mixed chat) 
{
  int i;

  if (pointerp(chat))
    for ( i = 0; i < sizeof(chat); i++)
      remove_achat_string(chat[i]);
  else
    if ((i = member_array(chat, achat_string[1])) != -1)
    {
      achat_string[0] -= achat_string[1][i-1];
      achat_string[1] = delete(achat_string[1], i-1, 1);
    }
}

void expand_mon_string(string str)
{
  int special;

  special = 0;

  if (!strlen(str))
    return;

  switch( str[0] )
  {
    // case '\'' : 
    //   str = "decir " + str[1..]; break;
    // case '\"' : 
    //  str = "lsay " + str[1..<1]; break;
    
    case '\'' : 
      // let's try a new way... do not queue command just for chatting
      this_object()->do_say(str[1..]);
      special = 1;
      break;
    
    case ':'  :
      str = this_object()->query_cap_name() + " " +
              (string)MONSTER_HAND->expand_string(this_object(), str[1..]) + "\n";
      tell_room(environment(this_object()), str, ({ this_object() }));
      special = 1;
      break;
    
    case '@':
      tell_room(environment(this_object()), str[1..] + "\n", this_object());
      special = 1;
      break;            
    
    case '#'  :
      call_other(this_object(), str[1..]);
      special = 1;
      break;
  }

  if (!special) 
    this_object()->queue_action(str);
}

void set_talk_string(mixed arr) { talk_string = arr; }
mixed query_talk_string() { return talk_string; }

void add_talk_string(string cat, mixed do_this) 
{
  talk_string += ({ cat, do_this });
}

void remove_talk_string(string cat)
 {
  int i;

  if ((i = member_array(cat, talk_string)) == -1)
    return ;
  talk_string = delete(talk_string, i, 2);
}

void catch_talk(string str)
{
  string s1, s2;
  int i;

  if (in_talk)
    return ;

  in_talk = 1;
  for (i=0;i<sizeof(talk_string);i+=2)
    if (sscanf(str, "%s"+talk_string[i]+"%s", s1, s2))
      if (stringp(talk_string[i+1]))
        call_other(this_object(), talk_string[i+1], str, s1, s2);
      else if (pointerp(talk_string[i+1]))
        call_other(talk_string[i+1][0], talk_string[i+1][1], str, s1, s2);
  in_talk = 0;
}

/* does the basic chat handling */
void chatter(int chance, mixed chat)
{
  int i, j;

  if (!sizeof(chat))
    return;
  
  if (this_object()->query_hide_shadow()) // neverbot
    return;
  
  if (sizeof(story)) 
  {
    if (which_story < sizeof(story))
      expand_mon_string(story[which_story++]);
    else
    {
      story = ({ });
      which_story = 0;
    }
  }
  else if ( random(1000) < chance )    
  {
    i = random(chat[0]+1);
    j = 0;
    while ( (i -= chat[1][j]) >0)
      j += 2;
    if (pointerp(chat[1][j+1]))
      story = chat[1][j+1];
    else
      expand_mon_string(chat[1][j+1]);
  }
}
    
mixed *stats() 
{
  mixed * ret;

  ret = ({
    ({ "Chat Chance", query_chat_chance() }),
    ({ "Achat Chance", query_achat_chance() }),
    ({ "Chat String", chat_string }),
    ({ "Achat String", achat_string }),
    ({ "In talk (nosave)", in_talk }),
    ({ "Talk String", talk_string }),
    ({ "Story (nosave)", story }),
    ({ "Which Story (nosave)", which_story }),
          });

  return ret;
}