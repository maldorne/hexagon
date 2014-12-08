#pragma save_types
/*
 * Originally..
 * 'the bit of the soul that all players should have ;)'
 * By Pinkfish, presumably
 *
 * This is the action queue, now, however.  Soul stuff removed.
 * This file has no soul.  It also uses private stuff.  Imagine that!
 *
 * By Wonderflug, 1996.
 * And Baldrick hacked some on it to make it worse dec '97.
 * Wonderflug, Sept 1998
 *   Put ia_message's back in, also a flag to turn them off
 *   (set_show_interrupt(1) = show, 0 = don't).
 *   Fixed ia_complete so stringp's get printed out.
 *   Added query_action_pending and some stuff in aq_add/insert
 *   so that monsters' heartbeats can be active anytime there's
 *   an action pending.
 * Folken, Abril 2003
 *   Arreglados algunos problemas con el mensaje de error de notify_fail().
 *   Restauradas las variables eliminadas por Goku.
 */

#include <living/queue.h>
#include <files/cmd.h>

/*
 * These must be saved in permanent storage; in fact, these should be
 * permanent stats of a character and handled as such.
 */
private int max_time;          /* Maximum 'spare' time this object can have  */
private int default_time;      /* Used for non-trivial actions w/o times set */

/* This allows the ia_message to be turned on or off.  Some
 *  players don't like them but newbies should see them.
 */
private int ia_show_interrupt;

/* the actionq can contain function pointers or strings ;
 * function pointers are evaluated, strings are passed to
 * command() when the action is due to be done.
 * The actionq is allocate()'d, since commands will constantly be added
 * and deleted from it, and growing and shrinking it would be needless
 * waste.  Admittedly this means the majority of the queue slots are empty
 * most of the time.
 *
 * Actually it's not; we let it grow and shrink dynamically, to save heaps
 * and heaps of memory.  I do have the old implementation lying around
 * ready to be plugged in though, should we be able to spare the memory,
 * and not the cpu. (the other version isn't particularly faster though)
 */
private static mixed* actionq;

/*
 * Security array; this is an array of flags, each corresponding to
 * an action in the action queue, indicating if the action was forced by
 * some other object or not.
 */
private static int* action_forcedq;

/* If the current command was forced or not; 
 * -1 means no command is in progress (and thus, to most checks, has the
 * conservative view that the action WAS forced)
 */
private static int curr_forced;

private static int time_left;  /* Time left for this round. */

/* Allows prompts to be disabled after a given command completes */
private static int show_prompt;

/* for bookkeeping purposes */
private static string command_in_progress; /* copy of a command in progress */
private static int time_adjusted;          /* flag if time's been adjusted  */
private static int trivial_actions_performed;  /* number performed this hb  */
private static int trivial_action_in_progress;  /* flag if command trivial  */
private static int notified;
/* Gosh this is ugly, Baldrick 1998-11-05 */
private static mixed notify_fail_msg;

private static string temp_verb; 

/* interruptable action stuff */
private static int ia_in_progress; /* flag indicating in the middle of an ia */
private static mixed ia_abort;     /* function/message on abort              */
private static mixed ia_complete;  /* function/message on complete           */
private static string ia_message;  /* message on any command not an abort    */

void create() 
{
  ia_show_interrupt = 1;
  time_left = this_object()->query_bits_per_beat();
  actionq = ({ });
  action_forcedq = ({ });
  curr_forced = -1;

  if ( undefinedp(max_time) )    
    max_time = this_object()->query_bits_per_beat();
  if ( undefinedp(default_time) )
    default_time = max_time;

  command_in_progress = "";
  time_adjusted = 0;
  trivial_actions_performed = 0;
  ia_in_progress = 0;
  ia_abort = 0;
  ia_complete = 0;
  ia_message = "";

  show_prompt = 1;
} 

mixed* debug_actionq() { return actionq; }
void debug_resetq() {  actionq = ({ }); }

/* debug of course */
int query_bits_per_beat() { return 10; }

string query_verb() 
{
  if ( strlen(temp_verb) > 0)
    return temp_verb;
  else
    return "";
    // return efun::query_verb();
}

// Esta funcion es llamada desde la simul notify_fail
void set_notify_fail_msg(mixed txt) { notify_fail_msg = txt; }
/*
void set_notify_fail_msg(string str) { 
    notify_fail_msg = str;
    return;
}
*/
string query_notify_fail_msg() 
{ 
  return stringp(notify_fail_msg)?notify_fail_msg:evaluate(notify_fail_msg);
}
/*
string query_notify_fail_msg() { 
    string tmp_m;
    tmp_m = notify_fail_msg;
    notify_fail_msg = "";
    return tmp_m;
}
*/

int query_time_left() { return time_left; }
int adjust_time_left(varargs int i) 
{ 
  time_adjusted = 1;

  if (!i)
    return time_left;

  time_left += i;
  if(time_left > max_time)
    time_left = max_time;
  return time_left;
}

/*
void adjust_time_left(){ 
    time_adjusted = 1;
}
*/

int query_show_interrupt() { 
  return ia_show_interrupt;
    // return this_object()->query_consent("debug");
}
int set_show_interrupt(int i) { 
  return ia_show_interrupt = i;
  // return this_object()->set_consent("debug", i);
}
int query_max_time() { return max_time; }
int set_max_time(int i){
  if ( i < 0 )
    return max_time = 0;
  else
    return max_time = i;
}

int query_default_action_time() { return default_time; }
int set_default_action_time(int i) { return default_time = i; }

int query_action_pending(){
  return ( (sizeof(actionq) != 0) || ia_in_progress);
}

void set_notified(int fi){
  notified = fi;
}

void set_trivial_action() { 
  trivial_action_in_progress = 1; 
  trivial_actions_performed++;
}

int set_interruptable_action(int time, string message, mixed abort, mixed complete){
  if ( ia_in_progress )
    return AQ_ERROR;

  // if ( !stringp(abort) && !functionp(abort) )
  if ( !stringp(abort) )
    return AQ_ERROR;

  ia_in_progress = 1;
  this_object()->adjust_time_left( -time );
  // this_object()->adjust_time_left();

  /* Hmm, is this one nessesary? 
   * give looks ugly with it at least.
   * Baldrick.
   * Wonderflug - Try picking up a very heavy object with wimpy
   * str, it is good to have feedback for why nothing is working.
   * the actual message could be changed, but it should be there
   * nonetheless.
   */
  /*
    ia_message = "Command queued since "+message+"\n" +
     "Type 'abort' to attempt to end prematurely.\n";
  */
  ia_message = (message?message:"Estás ocupado realizando una acción abortable.\n");

  ia_abort = abort;
  ia_complete = complete;

  return AQ_OK;
}

int abort_interruptable_action()
{
  string ret;

  if ( !ia_in_progress )
    return AQ_OK;

  // if ( functionp( ia_abort ) && !evaluate(ia_abort) )
  //   return AQ_ERROR;
  // else if ( stringp( ia_abort ) )
  //   tell_object( this_object(), ia_abort );

  if (! (ret = evaluate(ia_abort) ))
    return AQ_ERROR;
  else
    tell_object( this_object(), ret );

  ia_in_progress = 0;
  ia_abort = 0;
  ia_complete = 0;
  ia_message = "";
  time_left = 0;

  return AQ_OK;
}

/*
 * Small interface functions used in the prompting system
 */
int query_busy()
{
  // if ( time_left < 0 )
  if(sizeof(actionq) && (time_left < 0))
  {
    if ( ia_in_progress )
      return INTERRUPTABLE_BUSY;
    else
      return NON_INTERRUPTABLE_BUSY;
  }
  else if ( trivial_actions_performed > MAXIMUM_COMMANDS_PER_HB )
    return NON_INTERRUPTABLE_BUSY;
  return NOT_BUSY;
}

void set_no_prompt() { show_prompt = 0; }

/****
 * Queue maintenance routines
 ****/

/*
 * Determines who originated the current thread of execution. (action)
 * Returns 0 if this_object(); 1 if another object or it cannot
 * be determined for sure that it was this_object()
 * See queue_security (in some doc dir) to see the argument for
 * correctness of this.
 */
private int aq_determine_forced()
{
  /* this_user() determines it for sure, if set */
  if ( this_user() )
  {
    if ( this_user() == this_object() )
      return 0;
    else
      return 1;
  }

  /* if this_player is set, and not this_object, we know it IS forced */
  if ( this_player() && this_player() != this_object() )
    return 1;

  /* if this_player is this_object, or this_player isn't set, use
   * the current forced status; note if it's -1 then of course we are
   * being forced.
   */
  if ( curr_forced == 0 )
    return 0;
  else
    return 1;
}

/*
 * Tries to insert an action at the head of the queue.
 */
private int aq_insert( mixed val )
{
  // if ( !stringp( val ) && !functionp( val ) )
  if ( !stringp( val ) )
    return AQ_ERROR;
  if ( sizeof( actionq ) >= AQ_MAX_ACTIONS )
    return AQ_FULL;

  actionq = ({ val }) + actionq;
  action_forcedq = ({ aq_determine_forced() }) + action_forcedq;

  /* Hack so monster's HB's are started if they do something.
   */
  if ( !(this_object()->query_player())
    && !query_heart_beat(this_object()) )
    set_heart_beat(1);

  return AQ_OK;
}

/*
 * Tries to add an action to the end of queue
 */
private int aq_add( mixed val )
{
  // if ( !stringp( val ) && !functionp( val ) )
  if ( !stringp( val ) )
    return AQ_ERROR;

  if ( sizeof( actionq ) >= AQ_MAX_ACTIONS )
    return AQ_FULL;

  actionq += ({ val });
  action_forcedq += ({ aq_determine_forced() });

  // Hack so monster's HB's are started if they do something.
  if ( !(this_object()->query_player())
    && !query_heart_beat(this_object()) )
    set_heart_beat(1);

  return AQ_OK;
}

/*
 * Removes the head of the queue, of course, and returns it
 * -1 if there are no entries in the queue.
 */
private mixed aq_decapitate()
{
  mixed ret;

  if ( !sizeof(actionq) )
    return AQ_EMPTY;

  ret = actionq[0];

  actionq = actionq[1..];
  return ret;
}

/*
 * Deletes user actions from the queue.  Any command string is a user
 * action.  (supposedly)
 */
private int aq_delete_user_actions()
{
  int i;
  mixed* newq; 
  int* new_fq;

  newq = ({ });
  new_fq = ({ });
  for( i=0; i<sizeof(actionq); i++ )
    // if ( functionp(actionq[i]) )
    if ( stringp(actionq[i]) )
    {
      newq += ({ actionq[i] });
      new_fq += ({ action_forcedq[i] });
    }

  actionq = newq;
  action_forcedq = new_fq;

  return AQ_OK;
}

/* The do_cmd is a part of the external command handling system.
 * Made by Chrisy and gotten from RD. oct '95.
 * Moved here by Baldrick so that the whole living-tree can use the 
 * commands in that system.
 */   
int do_cmd(string tail)
{
  string verb, t;

  sscanf(tail, "%s %s", verb, t);
  if(!verb)
    verb = tail;

  return (int)CMD_HANDLER->cmd(verb, t, this_object());
} /* do_cmd() */

/*
 * This looks for an action to perform and does it if there's one waiting
 * and time left.  Otherwise it returns.  If it does perform an action it'll
 * return 1, otherwise 0.
 */
private int perform_next_action()
{
  mixed curr_act;
  //int verb, t;
  string verb, t;

  show_prompt = 1;
  // if ( ia_in_progress )
  if ( ia_in_progress && time_left >= 0 )
  {
    string ret;

    /* this means the interruptable action is complete. */
    // if ( functionp(ia_complete) )
    //   evaluate(ia_complete);
    // else if ( stringp(ia_complete) )
    //   tell_object( this_object(), ia_complete );

    if (( ret = evaluate(ia_complete) ) == ia_complete)
      tell_object( this_object(), ret );

    ia_in_progress = 0;
    ia_complete = 0;
    ia_abort = 0;
    ia_message = "";
    curr_forced = -1;
    /* now we go ahead and try to perform the next action as usual */
  }

  // if (trivial_actions_performed > MAXIMUM_COMMANDS_PER_HB )
  if (time_left < 0 || trivial_actions_performed > MAXIMUM_COMMANDS_PER_HB )
    return 0;

  if ( sizeof(actionq) == 0 ) 
  {
    // Folken 4/2003, esta funcion no existe!!!
    // Mal vamos con esta zona de codigo, si ni query_in_combat ni
    //   determine_action existen... las dejare comentadas por si algun
    //   dia el misterio se explica.
    /*
    if ( this_object()->query_in_combat() )  {
        mixed act = this_object()->determine_action() ;

        if ( !(stringp(act) || functionp(act)) 
          || aq_insert( act ) != AQ_OK )
        {
      tell_object(this_object(),
        "perform_next_action: determine_action buggered up\n");
      return 0;
        }
    }
    else
    */
    return 0;
  }

  curr_act = aq_decapitate();

  if ( intp(curr_act) )  /* it can't be good */
    return 0;

  curr_forced = action_forcedq[0];
  action_forcedq = action_forcedq[1..];

  trivial_action_in_progress = 0;

  /* testing something.. 
   * the right order of this may have to be tweaked
   * Baldrick */
  notify_fail("");
  // notified = 0;


  // if ( functionp( curr_act ) )    
  // {
  //   evaluate( curr_act );
  // }
  // else if ( stringp( curr_act ) )    

  if (evaluate(curr_act) == curr_act)
  {
    notify_fail_msg = 0;
    command_in_progress = curr_act;
    time_adjusted = 0;

    sscanf(curr_act, "%s %s", verb, t);
    if(!verb)
      verb = curr_act;
    temp_verb = verb;

    if (!command( curr_act ))
    {
      if (!(environment() &&
        environment(this_object())->do_move_command(curr_act)) )
        if (!this_object()->do_gr_command(verb, t))
          if (!CMD_HANDLER->cmd(verb, t, this_object()))
            if (!this_object()->parse_comm(verb, t ))
            {
              if (!CMD_HANDLER->soul_com(curr_act, this_object()))
                /* The end, either the command didn't work or we have 
                 * a notify fail. in that case, it writes a new message, if not,
                 * it writes the one we made earlier, a standard one.
                 */
                if(notify_fail_msg)
                  tell_object(this_object(),query_notify_fail_msg());
                else
                  tell_object(this_object(),"El intento de hacer '%^RED%^" + curr_act + 
                    "%^RESET%^' no funcionó.\n");
              /*
              // Hack, baldrick 
              this_object()->set_notify_fail_msg("El intento de hacer '%^RED%^" + curr_act
               + "%^RESET%^' no funcionó.\n");
              */
            }
    }
  
    command_in_progress = "";
  }
  else
  {
    temp_verb = "";
    return 0;
  }

  /* if the action didn't flag itself as trivial, and it didn't adjust
   * time left, then adjust time by a default
   *
   * Have to check for this_object(), alas, since 'quit' will destroy it.
   * Particularly stupid, I must say, makes no sense.
   */

  /*
  if ( !trivial_action_in_progress && !time_adjusted && this_object() )
   adjust_time_left( -this_object()->query_default_action_time() );
  */

  if ( !trivial_action_in_progress && !time_adjusted && this_object() )
    adjust_time_left();

  if ( !ia_in_progress )
    curr_forced = -1;

  temp_verb = "";
  return 1;
} /* perform_next_action() */

/****
 * The real routines
 ****/

/*
 * The outside interface for inserting actions to the head of the queue.
 */
nomask int insert_action( mixed val ) { return aq_insert( val ); }
nomask int queue_action( mixed val ) { return aq_add( val ); }
int action_queue_empty() { return sizeof(actionq) == 0; }

nomask int query_current_action_forced() { return curr_forced; }

/*
 * This should be called each heart beat..
 * This loops on perform_next_action until time is done or the max eval is
 * is hit, probably.  For security we might want to ENSURE this is only
 * called once per heartbeat.
 */
void act() 
{
  trivial_actions_performed = 0;

  if ( time_left < max_time )
  {
    time_left += this_object()->query_bits_per_beat();
    time_left = ( time_left > max_time ) ? max_time : time_left;
  }

  while( perform_next_action() )
  {
    if ( show_prompt && this_object() )
    {
      this_object()->show_prompt();
      this_object()->write_prompt();  
    }
  }
} /* act() */


/*
 * The great command enqueuer.  Fwheee.
 * If this_object has no heartbeat, it won't do anything.  Actions
 * can go right on through, then.
 * Otherwise, it sticks it on the queue and tells everyone its been done.
 * Well it's worse than that.  If we're the one trying to do the
 * action, then we let it through.
 */
nomask int action_check(string str) 
{
    int i;
    string tmp;

    /* this is ridiculous.  MudOS will not show these strings as equal. */
    if ( str == command_in_progress )
      return 0;

    /* Expand some common aliases
     * Moved from process_input.
     */
    switch( str[0] )
    {   
      case '\'' : str = "decir "+   str[1..]; break;
      case ':'  : str = "emocion "+ str[1..]; break;
      // case '\"' : str = "lsay "+  str[1..<1]; break;
      // case ';'  : str = "parse "+ str[1..<1]; break;
    }

    if( (tmp = EXPANSION[str]) ) 
      str = tmp;

    /* check for some special queue-affecting commands */
    switch( str )
    {
      case "restart":
      case "reiniciar":
        if ( query_heart_beat(this_object()) )
          tell_object(this_object(),
            // "No necesitas que tu heartbeat sea reseteado.\n");
            "No necesitas reiniciar tu personaje en estos momentos.\n");
        else
        {
          tell_object(this_object(),"Intentando reiniciar tu personaje...\n");
          actionq = ({ });  /* will get cascading failure without this */
          set_heart_beat(1);
          catch(this_object()->flush_spell_effects());
        }
        return 1;

      case "stop":
      case "parar":
        if ( sizeof(actionq) != 0 )
            aq_delete_user_actions();
          tell_object(this_object(),"Cola de comandos borrada.\n");
          // "(stop-fight parara los ataques si es lo que querias)\n");
        return 1;

      case "abort":
      case "abortar":
        if ( !ia_in_progress )
        {
          tell_object(this_object(),"No estás en medio de una acción que "+
            "pueda ser abortada.\n");
          return 1;
        }
        abort_interruptable_action();
        return 1;
      /*
      case "huir":
          environment(this_object())->do_random_move_command();
          return 1;
      */
      case "test":
        write(object_name(this_object()) + "\n");
        write(print_object(::status(this_object())) + "\n");
        write(print_object(this_object()->stats()) + "\n");
        // return 1;
    }

    if ( query_heart_beat(this_object()) == 0 )
    {
      // tell_object(this_object(),"%^BOLD%^ALERTA%^RESET%^:\n"
      //   "No tienes heartbeat; prueba '%^BOLD%^reiniciar%^RESET%^' para intentar "
      //   "solucionarlo.  No podrás hacer nada sin tu "
      //   "heartbeat.\n");
      tell_object(this_object(),"%^BOLD%^ALERTA%^RESET%^:\n"+
        "Se ha producido un error. Prueba a '%^BOLD%^reiniciar%^RESET%^' para intentar "+
        "solucionarlo.\n");
    }

    if ( sizeof(actionq) >= AQ_MAX_ACTIONS )
    {
      tell_object(this_object(), "Ya hay demasiados comandos en la cola; "+
        "ignorando '"+str+"'.\n");
      return 1;
    }

    if(this_object()->action_blocked())
    {
      /* Wonderflug, Dec 95, making this more flexible --
       * can specify the string that the block puts out.
       * A year later, adding ability to make it a function pointer too.
       */
      mixed pop;
      mixed ret;

      pop = this_object()->action_blocked();

      // if ( stringp(pop) )
      // {
      //   tell_object(this_object(),pop);
      //   if(!this_object()->query_admin())
      //     return 1;
      // }
      // else if ( functionp( pop) )
      //   return evaluate( pop );
      // else
      // {
      //   tell_object(this_object(),
      //     "Estás aturdido, no puedes hacer nada.\n");
      //   if(!this_object()->query_admin())
      //     return 1;
      // }

      if (stringp(pop))
      {
        if (( ret = evaluate(pop) ) != pop)
        {
          return ret;
        }
        else
        {
          tell_object(this_object(), ret);
          if(!this_object()->query_admin())
            return 1;
        }
      }
      else
      {
        tell_object(this_object(),
          "Estás aturdido, no puedes hacer nada.\n");
        if(!this_object()->query_admin())
          return 1;
      }
    }

    /* not sure why this is necessary.. */
    str = replace_string(str,"@@","");

    /* Bishop - moving to process_input
    this_object()->add_history(str);
     */
    /* The latency is too much, so we've added a check to see
     * if it's ok to execute the command right away.
     */
    if ( (i=aq_add( str )) != AQ_OK )
      tell_object(this_object(),"Error insertando en la cola de comandos '"
        +str+"', "+i+"\n");
    else if ( ia_in_progress && query_show_interrupt() )
      tell_object(this_object(),ia_message);
    else 
    {
      /* added the while below to remove latency in history/alias commands 
       * as well; to get prompting correct, I've also unrolled the first
       * iteration of the loop into an if.
       */
      // if ( ( sizeof(actionq) >= 1 ) 
      if ( ( sizeof(actionq) >= 1 ) && ( time_left > 0 )
        && ( trivial_actions_performed < MAXIMUM_COMMANDS_PER_HB ) )
          perform_next_action();

      // while ( sizeof(actionq) >= 1 
      while ( sizeof(actionq) >= 1 && time_left > 0 
        && trivial_actions_performed < MAXIMUM_COMMANDS_PER_HB )
      {
        this_object()->show_prompt();
        this_object()->write_prompt();
        perform_next_action();
      }
    }

    if ( show_prompt )
      this_object()->show_prompt();

    return 1;
} /* action_check() */

/* Always return 0 to get the driver's notify_fail;
 * however getting here, we can be pretty sure that no 'real' action
 * was done, so we know a trivial command was done.  Anything using a
 * notify_fail shouldn't be a real action anyway, or can adjust time itself.
 */
nomask int lower_check(string str) 
{
  if ( command_in_progress )
  {
    trivial_action_in_progress = 1;
    trivial_actions_performed++;
  }

  return 0;
} /* lower_check() */

mixed * stats() 
{
  return ({ });
}
