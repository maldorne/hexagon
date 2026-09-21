// The quests of the demo, registered on the shared core. Everything about how a
// quest is offered, advanced, handed in and paid lives in the mudlib handler;
// this only says which quests this game ships.

inherit "/lib/handlers/quests.c";

#define QUESTS "/games/demo-fantasy/quests/"

void create()
{
  ::create();

  // the chain that ends with a character able to climb the Mallorn: the tavern
  // sends them after the wasps, and the old man they report to teaches them
  add_quest(QUESTS + "wasps-ruin-the-hunt.c");
  add_quest(QUESTS + "news-for-the-old-man.c");
}
