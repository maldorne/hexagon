
#include <files/log.h>

// called from /lib/core/login after successful login with user name and password
// this is the end of the login process
void start(varargs int going_invis, int is_new_player)
{
  if (!query_cols())
    set_cols(79);

  if (last_on_from && query_coder())
    write(_LANG_LAST_CONNECTION);

  last_on_from = query_ip_name(this_object())+" ("+
                 query_ip_number(this_object())+")";

  /* Hamlet added this.  For logging on invis */
  if (going_invis != -1)
    set_invis(going_invis);

  // update the current prompt
  parse_prompt();

  if (query_coder())
    if (file_size("/home/" + name + "/" + PLAYER_ERROR_LOG) > 0)
      write("\nTienes informes de errores en /home/"+name+"/"+PLAYER_ERROR_LOG+"\n");

  // commands::show_commands();
}
