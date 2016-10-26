/*
 * Originally..
 * 'the bit of the soul that all players should have ;)'
 * By Pinkfish, presumably
 *
 * Now it really is that bit of the soul that all players should have.
 * Command queue stuff ax'd out and moved to std/living/queue.c
 *
 * By Wonderflug, 1996.
 *
 * and now it's Baldrick's time. gonna move it out of the living object.
 * I will put it in the /cmds/cmd_handler somehow.
 *
 */

#include <mud/cmd.h>

void create()
{

}

int soul_com_force(string str)
{
  // string str1, str2, str3;

  if ( this_object()->action_blocked() &&
       !this_object()->query_admin() )
    return 0;

  if (file_name(previous_object()) != SOUL_OBJECT)
    return 0;

  /* Flode - 020998
    command(str);
   */

  // removed, neverbot 02/2006
  /*
  if (sscanf(str,"%s %s %s", str1, str2, str3) == 3){

    if (str1 == "alias"){
       // this_object()->exec_alias(str2,str3);
       this_object()->process_input(str2 + " " + str3);
       return 1;
    }
  }
  */

  CMD_HANDLER->soul_com(str, TO);

  return 1;
}

mixed * stats()
{
  return ({
      });
}
