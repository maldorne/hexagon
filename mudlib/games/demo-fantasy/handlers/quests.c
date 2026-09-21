// The quests of the demo, registered on the shared core. Everything about how a
// quest is offered, advanced, handed in and paid lives in the mudlib handler;
// this only says which quests this game ships.

inherit "/lib/handlers/quests.c";

#define QUESTS "/games/demo-fantasy/quests/"

void create()
{
  ::create();

  add_quest(QUESTS + "learn-to-climb.c");
}
