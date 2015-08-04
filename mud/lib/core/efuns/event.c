/*
 *  sibbis@suvangi donated this for Rd's use. Many thanks goes
 *  out to him for his generosity.
 *  And Baldrick didn't even ask Sibbis, guess that makes us closer to even.
 *
 */

// TODO remove comment when auto::event and auto::event_except are no longer needed

void event( mixed obs, string event_name, mixed arg... )
{
  object who;
  object * event_obs;
  int i;

  who = previous_object();

  if( arrayp( obs ) )
  {
    event_obs = obs;
  } 
  else 
  {
    if( objectp( obs ) )
    {
      event_obs = ({ obs }) + all_inventory( obs );
      
      if( obs != who )
      {
        event_obs -= ({ who });
      }
    } 
    else 
    {
      return;
    }
  }

  for (i = 0; i < sizeof(event_obs); i++)
    call_other( event_obs[i], "event_"+ event_name, who, arg... );
}

