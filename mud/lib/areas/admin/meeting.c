#include "path.h"

inherit "/lib/room";

#define LOGGER "/obj/handlers/meeting_log"
object floor;
string chair;
int last_say;
int log_souls;

void setup() {
  set_short("Meeting room");
  set_long(
    "This room is the definition of comfort. Along the oak-paneled walls " +
    "on every side are book shelves stretching all the way to the high ceiling.  " +
    "One corner has a table adorned with various liquor decanters. The center " +
    "of the room is filled with all types of large, soft chairs. Each has a " +
    "foot stool and a small table beside it. Cowering in another corner you see " +
    "a young scribe rapidly taking notes.\n");

  add_exit("sur","/home/common","standard");
  set_light(45);

  add_item("wall","These are oak paneled walls, dummy.  What more would you "+
                  "have me say about them?\n");
  add_item(({"book","book shelf","book shelves"}),"The shelves are all "+
           "filled with ancient leather-bound volumes.  Their collection "+
           "must have taken quite some time.\n");
  add_item("table","All the tables are made of hard woods and are ornately "+
           "crafted.\n");
  add_item("ceiling","This room must be two stories high.  You have trouble "+
           "seeing any details of the ceiling.\n");
  add_item("liquor decanters","Made of a fine crystal, these decanters "+
           "hold any sort of alcohol you might want.\n");
  add_item("chair","The chair looks extremely comfortable.\n");
  add_item("foot stool","The stool is just what you need to fully relax "+
           "during a meeting.\n");
  add_item("scribe","Being a very young man, this scribe obviously feels "+
           "dwarfed by the immense characters of your party.  Dutifully "+
           "he takes down the minutes of the meeting.\n");
}

void reset() {
  if (floor)
    return ;
  floor = clone_object("/std/object");
  floor->set_name("floor");
  floor->set_short("The floor");
  floor->set_long(
    "This is the floor of the meeting.  It gives you the right to speak "+
    "during the meeting.  Please give this to the next person to speak "+
    "after you have finished.\n");
  floor->move(this_object());
}

void init() {
  string str;

  ::init();
  add_action("log_mode","logmode");
  /* So I just banged my gavel.
   * You did what?
   * I banged my gavel and did the "order in the court thing"
   */
  if ("/secure/master"->high_programmer((str=(string)this_player()->query_name()))
      || str == chair) {
    add_action("appoint", "appoint");
    add_action("bang", "bang");
    add_action("recover", "recover"); /* recovers the floor in case of
                                       * loss */
  }
}

int appoint(string str) {
  object ob;

  if (chair && (string)this_player()->query_name() != chair) {
    notify_fail("The chair has already been apointed.\n");
    return 0;
  }
  if (!(ob = find_player(str))) {
    notify_fail("Sorry, cannot find "+str+" to be the chair.\n");
    return 0;
  }
  chair = str;
  say(this_player()->query_cap_name()+" just appointed "+str+" as the "+
        "chair of the meeting.\n", ob);
  write("Ok, "+str+" is now the chair of the meeting.\n");
  tell_object(ob, this_player()->query_cap_name()+
        " just appointed you the chair of the meeting.\n");
  if (environment(ob) != this_object())
    ob->move(this_object());
  return 1;
}

int bang(string str) {
  if ((string)this_player()->query_name() != chair) {
    notify_fail("Only the chair can bang the gavel.\n");
    return 0;
  }
  tell_room(this_object(),
            this_player()->query_cap_name()+" bangs the gavel loudly.  STOP "+
            "STOP!\n");
  return 1;
}

int recover() {
  if (!floor)
    reset();
  floor->move(this_player());
  write("Floor recovered.\n");
  say(this_player()->query_cap_name()+" has recovered the floor.\n");
  return 1;
}

int log_mode(string str) {
  log_souls = !log_souls;
  
  if(log_souls)
    tell_object(this_player(),"Souls will be logged.\n");
  else
    tell_object(this_player(),"Souls will not be logged.\n");

  return 1;
}

/* this will divide up separate conversation. */
void check_expired() {
  if( (time() - last_say) > (600) )
    LOGGER->log_it("* * * * *\n");
    
  last_say = time();
}

void event_person_say(object ob, string start, string rest) {
  check_expired();
  LOGGER->log_it(start+rest+"\n");
}

void event_say(object ob, string rest) {
  check_expired();
  LOGGER->log_it(rest);
}

void event_soul(object ob, string rest) {
  if(log_souls) {
    check_expired();
    LOGGER->log_it(rest);
  }
}

void dest_me() {
  if (floor)
    floor->dest_me();
  ::dest_me();
}
