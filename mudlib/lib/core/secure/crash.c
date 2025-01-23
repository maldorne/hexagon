void crash(string crash_mess, object command_giver, object current_object)
{
  // int i;
  // object *obs;

  log_file("crash", ctime(time())+" ");

  if (current_object)
    log_file("crash", "Current Object: "+file_name(current_object)+" ");

  if (command_giver)
    log_file("crash", "Command Giver: "+command_giver->query_cap_name());

  log_file("crash", " Reason: "+crash_mess+"\n");
  cp("/log/debug.log", "/log/debug.log.old");

  // "/secure/simul_efun"->shout("\n\nFallo general, desconectando.\n\n");

  /* Want to make sure we dont go and bomb in here. */
  /* Next stuff just ends up meaning that the mud never goes down.
  obs = users();
  for (i=0;i<sizeof(obs);i++)
    obs[i]->do_force_on_me("quit");
  */
} /* crash() */
