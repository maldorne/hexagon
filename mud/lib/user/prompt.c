/*
 * By Wonderflug, 1997.
 *
 * Configurable prompting.
 */

#include <living/queue.h>

/* This is the original prompt string that the player sets; this is saved,
 * and then re-parsed each time the player logs on, since the parsed version
 * contains function pointers (or likely will)
 */
string prompt_string;

/* This is an array representing the parsed prompt_string; when the prompt
 * is shown, the array is traversed; strings are simply output, and
 * function pointers are evaluated and their results output.
 */
static string * prompt;

void create()
{

}

void parse_prompt();

/* initialization; called from start_player or somewhere */
void init_prompt()
{
  parse_prompt();
}

/*
 * Returns a short string showing if the current player is 'busy' or not
 * This is used by the driver whenever it needs to show a prompt.
 */
void write_prompt()
{
    /*
    int i;

    // Added by baldrick, new ed.
    if ( ( i = query_ed_mode() ) != -1)
    { 
        switch (i)
        {
            case -2:
                write ("more:");
                return;
            case 0:
                write ("ed:");
                return;
            default:
                write (i + ":");
                return;
        }
    } 
    */

    switch( this_object()->query_busy() )
    {
        case NON_INTERRUPTABLE_BUSY:
            write("] ");
            break;
        case INTERRUPTABLE_BUSY:
            write("} ");
            break;
        case NOT_BUSY:
            write("> ");
    }
}

/*
 * Returns a short string showing if the current player is 'busy' or not
 * This is used in the configurable prompt system, as the busy indicator.
 */
string query_busy_string()
{
  switch( this_object()->query_busy() )
  {
    case NON_INTERRUPTABLE_BUSY:
      return "* ";
      break;
    case INTERRUPTABLE_BUSY:
      return "+ ";
      break;
  }
}

/*
 * Parse each element of a prompt string, that is, $<whatever> 's
 */
mixed parse_prompt_element(string str)
{
  switch( str[0..0] )
  {
    case "h":
      return ({ "@query_hp", str[1..] });

    case "g":
      return ({ "@query_gp", str[1..] });

    /* Additions by Baldrick, taken from RoD. some of this may be 
     * immortal only. */
    case "B":
      return ({ "%^BOLD%^",  str[1..] });

    case "b":
      return ({ "%^RESET%^",  str[1..] });

    case "m":
      return ({ mud_name(),  str[1..] });

    // case "t":
    //  return ({ yymmddhhmmss()[6..9],  str[1..<0] });
  }

  if (this_object()->query_coder()) 
  {
    switch( str[0..0] )
    {
    case "~":
      return ({ "@query_current_path",  str[1..] });

    case "/":
      return ({ "@short_path",  str[1..] });
    }
  }
  return ({ str });
}

void parse_prompt()
{
  int i;
  string* p;

  /* Need to reset this first */
  prompt = ({ });
  if ( strlen(prompt_string) <= 0 )
  {
    return ;
  }

  p = explode( prompt_string, "$" );

  if ( sizeof(p) < 1 )
  {
    /* this only happens with a string of all $'s */
    prompt = ({ prompt_string });
    return ;
  }
  
  if ( prompt_string[0] == '$' )
    prompt += parse_prompt_element(p[0]);
  else
    prompt = ({ p[0] });
  
  for( i=1; i<sizeof(p); i++ )
    prompt += parse_prompt_element(p[i]);

  prompt -= ({ 0 });
}

int set_prompt(string str)
{
  prompt_string = str;
  parse_prompt();
  return 1;
}

void show_prompt()
{
  int i;
  string s;

  s = "";

  for (i = 0; i < sizeof(prompt); i++)
  {
    // if ( stringp(prompt[i]) )
    //   s += prompt[i];
    // else if ( functionp(prompt[i]) )
    //   s += (string)evaluate(prompt[i]);

    if (prompt[i][0..0] == "@")
      s+= call_other(this_object(), prompt[i][1..]);
    else
      s += prompt[i];
  }
  write(s);
}

// debugging 
// string query_prompt_string() { return prompt_string; }
// mixed query_prompt() { return prompt; }

mixed * stats() 
{
  return ({ 
    ({ "Prompt", prompt_string, }),
          });
}

