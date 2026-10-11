// Entry triggers a room declares for the location it becomes: whoever enters
// carrying a timed property (the mark) is told something and, optionally,
// taken through one of the room's exits, sealed or not. room2loc turns the
// list into a trigger component (/lib/location/components/trigger.c), which
// is what runs them; a room that is never converted only keeps the list.

private mixed * entry_triggers;

void create()
{
  entry_triggers = ({ });
}

/**
 * Declare an entry trigger.
 *
 * @param  mark          timed property the living must carry; it is removed
 *                       when the trigger fires
 * @param  message       what the living reads
 * @param  exit          direction of the exit it is taken through, if any
 * @param  room_message  what the others read after its name, if anything
 */
void add_entry_trigger(string mark, string message, varargs string exit,
                       string room_message)
{
  if (!entry_triggers)
    entry_triggers = ({ });
  entry_triggers += ({ ([ "mark": mark, "message": message,
                          "exit": exit, "room_message": room_message ]) });
}

mixed * query_entry_triggers()
{
  return entry_triggers ? ({ }) + entry_triggers : ({ });
}
