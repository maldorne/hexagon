
// code taken from monster.c, neverbot 04/2009

#include <language.h>

mixed *chat_string,    /* ({ ({ int chances }), ({ chat strings }) }) */
      *achat_string;   /* ({ ({ int chances }), ({ chat strings }) }) */
int chat_chance,       /* Chance of chatting any given round */
    achat_chance;      /* Chance of doing an attack chat any round */    
    // in_talk;        /* So stuff I say doesn't get catch_talk'd */

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
  // in_talk = 0;    
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

// Aux function for load_chat and load_a_chat in monster.c
// If we include in the message certain words between dollar symbols ($)
// they will be changed: i.e. $race$ --> ob->query_race()
string chat_expand_string(object me, string in_str, varargs object on, object attacker)
{
  string *str, ret;
  int i, j, add_dollar;
  int k,l;

  object liv, *obs, ob, *attacker_list;

  str = explode(in_str, "$");
  ret = "";

  // Fix by Wonderflug, only parse every SECOND argument, just tack on the 
  // others.
  for (i = 0; i < sizeof(str); i++)
  {
    if ( !( i % 2) )
      ret += str[i];
    else
    {
      switch (str[i][0])
      {
        case 'm' :
          ob = me;
          break;
        case 'l' :
          if (!ob)
          {
            if (!liv)
            {
              obs = all_inventory(environment(me));
              k = sizeof(obs);
              l = random(k);
              for (j = 0; j < k; j++)
              {
                if (living(obs[l]) && obs[l] != me)
                {
                  liv = obs[l];
                  break;
                }
                l++;
                l %= k;
              }
            }
            if (!liv) // No one here to see us blow up anyway ;) 
              ob = me;
            ob = liv;
          }

          if (!ob)
          {
            attacker_list = me->query_attacker_list();
            
            if (!sizeof(attacker_list))
              break;
            
            if (!attacker)
              attacker = attacker_list[random(sizeof(attacker_list))];
            
            ob = attacker;
          }
          
        case 'o' :
          if (!ob)
          {
            if (!on)
            {
              obs = all_inventory(environment(me));
              k = sizeof(obs);
              l = random(k);
              ob = me; // should all else fail
              for (j = 0; j < k; j++)
              {
                if ( obs[l] != me)
                {
                  on = obs[l];
                  break;
                }
                l++;
                l %= k;
              }
            }
            else
              ob = on;
          } // if (!ob)

          switch (str[i][1..])
          {
            case "name" :
              ret += (string)ob->query_name();
              add_dollar = 0;
              break;
            case "cname" :
              ret += (string)ob->query_cap_name();
              add_dollar = 0;
              break;
            case "gender" :
              ret += (string)ob->query_gender_string();
              add_dollar = 0;
              break;
            case "poss" :
              ret += (string)ob->query_possessive();
              add_dollar = 0;
              break;
            case "obj" :
              ret += (string)ob->query_objective();
              add_dollar = 0;
              break;
            case "gtitle" :
              ret += (string)ob->query_gender_title();
              add_dollar = 0;
              break;
            case "pronoun" :
              ret += (string)ob->query_pronoun();
              add_dollar = 0;
              break;
            case "race":
              ret += (string)ob->query_race_name();
              add_dollar =0;
              break;
            case "guild":
              ret += (string)ob->query_guild_name();
              add_dollar =0;
              break;
            case "group":
              ret += (string)ob->query_group_name();
              add_dollar =0;
              break;
            case "racegroup":
              ret += (string)ob->query_race_group_name();
              add_dollar =0;
              break;
            case "class":
              ret += (string)ob->query_class_name();
              add_dollar =0;
              break;

            default :
              if (add_dollar)
                ret += "$";
              ret += str[i];
              add_dollar = 1;
              break;
          } // switch str[i][1..

          ob = nil;
          break;

        default :
          if (add_dollar)
            ret += "$";
          ret += str[i];
          add_dollar = 1;
          ob = nil;
          break;
      }
    }
  }

  if (ret[strlen(ret)-1] == '$')
    ret = ret[0..strlen(ret)-2];
  
  if (!stringp(ret))
    return _LANG_SEEMS_CONFUSED;
  
  return ret;
}

void expand_string(string str)
{
  int special;

  special = 0;

  if (!strlen(str))
    return;

  switch( str[0] )
  {
    case '\'' : 
      // let's try a new way... do not queue command just for chatting
      this_object()->do_say(str[1..]);
      special = 1;
      break;
    
    case ':'  :
      str = "\n" + this_object()->query_cap_name() + " " +
              (string)chat_expand_string(this_object(), str[1..]) + "\n";
      tell_room(environment(this_object()), str, ({ this_object() }));
      special = 1;
      break;
    
    case '@':
      tell_room(environment(this_object()), "\n" + str[1..] + "\n", ({ this_object()} ));
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

// void catch_talk(string str)
// {
//   string s1, s2;
//   int i;

//   if (in_talk)
//     return ;

//   in_talk = 1;
//   for (i=0;i<sizeof(talk_string);i+=2)
//     if (sscanf(str, "%s"+talk_string[i]+"%s", s1, s2))
//       if (stringp(talk_string[i+1]))
//         call_other(this_object(), talk_string[i+1], str, s1, s2);
//       else if (pointerp(talk_string[i+1]))
//         call_other(talk_string[i+1][0], talk_string[i+1][1], str, s1, s2);
//   in_talk = 0;
// }

// does the basic chat handling
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
      expand_string(story[which_story++]);
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
      expand_string(chat[1][j+1]);
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
    // ({ "In talk (nosave)", in_talk }),
    ({ "Talk String", talk_string }),
    ({ "Story (nosave)", story }),
    ({ "Which Story (nosave)", which_story }),
          });

  return ret;
}