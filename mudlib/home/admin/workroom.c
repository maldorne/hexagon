#define CREATOR "admin"

/**
 * Workroom for newbie coders. Here we can find the most basic features 
 * that can be included in a room, and some more advanced things to learn.
 * 
 * Radix : September 27, 1995
 * Lummen : 18 Sept, 1997
 * Fairiel : 7 Aug, 2000 
 * doors added, neverbot, 25 Oct 2003
 * Translated and updated for Hexagon, neverbot 12/2020
 */

/**
 * The base features of a room will be inherited from other more generic
 * file. Depending of the type of room you want, you can inherit from:
 *  /lib/room.c        -> indoors rooms.
 *  /lib/outside.c     -> outdoors rooms, with weather, sun, etc.
 *  /lib/forest.c      -> forest type areas. 
 *  /lib/underground.c -> tunnels, sewers, etc.
 */
inherit "/lib/room.c";

#include <translations/exits.h>

// this variables and definitions will be used below
static object door1, door2;
static object trapdoor;

#define PROPERTY_CLEANED "room_cleaned"

// the setup() function is the main point of every room or npc.
// Will be called during the room creation and here we define every 
// feature of the room that will be different from the basic/generic
// room inherited
void setup()
{
  set_short(capitalize(CREATOR) + "'s workroom");

  set_long("The workroom you find yourself standing in has a new layer " +
    "of paint. A normal assortment of dusty furniture is " +
    "arranged around the room. The most important being the " +
    "large desk and old chair against the far wall facing you. " +
    "Hanging from the ceiling, a small lamp gives ample light for " +
    "the room. With some consideration, you decide the room " +
    "is in need of cleaning.\n");

  // sets the level of light in the room. "help light" for details
  set_light(80);
  
  // add_item() is used to describe all nouns you have in descriptions
  add_item("workroom", "The workroom surrounds you with the smell " +
    "of a fresh coat of paint. The walls glisten in the light.\n");

  add_item("wall", "All four walls are white freshly coated with " +
    "paint.\n");

  add_item("paint", "The entire room has been painted with a new " +
    "coat of paint The paint upon the walls reflects light from " +
    "lamp hanging from the center of the room.\n");
  
  // you can also give many items the same description using an 
  // array of words, or maybe use when you want to be able to look
  // at the same thing with single or plural words
  add_item(({ "furniture", "desk", "chair" }), "Facing you against " +
    "the far wall, a large desk and chair are set. The desk and " +
    "chair have a large layer of dust covering their entire " +
    "surface. The chair has nearly worn through and perhaps " +
    "is in need of repair.\n");

  add_item(({ "ceiling", "lamp", "small lamp" }),
    "Hanging from the center of the room's ceiling, a small " +
    "oil lamp burns continuously emitting ample lighting for " +
    "for the room.\n");

  // senses added by Sojan. This adds even more life to your rooms.
  // this is completely optional and is not very usual 
  add_smell(({ "room", "workroom", "air" }), "Here we put what the " +
    "player would get when they typed 'smell room'. They should " +
    "smell the paint in the air of course.\n");

  add_feel("desk", "Here we would put what the player would " +
    "get when they typed 'touch desk'.\n");

  add_taste("paint", "You lick the paint from the wall and soon " +
    "realize this was a major mistake... get this by typing " +
    "'taste paint'.\n");

  add_sound(({ "room", "workroom" }), "You hear a rumbling noise " +
    "coming from above. Get this by typing 'listen room'.\n");
  
  // Remember, when describing your future rooms, always describe
  // everything as richly as possible (not half-baked like these)

  // The following is used to "clone" objects into rooms.
  // These objects can range from npcs and monsters to weapons.
  // A separate file (much like this workroom) will be required
  // so we use the function add_clone to bring them into your rooms.
  // Its used with add_clone(file, num_of_clones);
  
  add_clone("/games/hexagon/areas/admin/items/button.c", 1);
  
  // Here are the exits from your room
  // add_exit(direction, destination, type)
  //   direction - What they must type to leave that direction
  //   destination - The room they will be moved to (its full path + filename)
  //   type - The exit type, can be "path", "corridor", "door"...

  // default door (can be open/closed) will be added to the room
  // every time we use the exit type "door"
  // add_exit(DIR_EAST, "/home/" + CREATOR + "/somewhere.c", "door");

  // you can add here any other exists that are useful to you
  // the rest of the setup() function is some advanced examples
  // about how to add and modify exits

  // if we want to use doors to do something different, add_exit
  // will return the door object. We use the variables defined at the
  // beginning of the file and we can modify them afterwards. Take a 
  // look to /lib/room/items/door.c

  door1 = add_exit(DIR_COMMON, "/home/common.c", "door");
  if (door1)
  {
    door1->set_init_status(0); // initially closed
    door1->reset_message();    // use a generic message
    door1->set_dir_other_side(CREATOR); // name of the exit to come back
  }

  // we can add another door in the common room to be able to come back
  door2 = "/home/common.c"->add_exit(CREATOR, "/home/" + CREATOR + "/workroom.c", "door");
  if (door2)
  {
    door2->set_init_status(0);
    door2->reset_message();
    door2->set_dir_other_side(DIR_COMMON);
  }
  // after modifying the exits outside of the setup() of our own room, 
  // a renew_exits is needed 
  "/home/common.c"->renew_exits();
}

// the init function will be called every time an object 
// is moved to the same environment as any other, so this place is
// generally speaking only used to add actions
void init()
{
  // very important, include the call to the init() function
  // of the inherited file. Without it every action of the
  // standar room will not be available when we enter this room
  ::init();

  // the add_action function binds a command to a local 
  // function in this file
  add_action("do_clean", "clean");
  add_action("do_move", ({ "move", "push" }));
}

// the functions binded to actions will be in the form:
// int function_name(string something)
//   we must return an int value
//     0 - could not do the action
//     1 - was done right
// we can have multiple objects with the same action
// (push the table, push an npc, etc). If we return a 0 the mud will 
// search for the same action in other objects.
int do_clean(string str)
{
  if (!strlen(str))
  {
    // notify_fail will NOT show the error message right now. As
    // we return a 0, the next object will be tried. If some object
    // returns 1, the nofity fail will not be used. If every object 
    // (maybe only this one) returns 0, the last notify fail will
    // be used.
    notify_fail("Clean what?\n");
    return 0;
  }

  if ((str == "room") || (str == "desk"))
  {
    // we can store any kind of value in any kind of object
    // using properties. the timed ones will disappear after some time.
    // when we clean the room we will store a property, so if we don't have
    // it now, we now it has not been cleaned lately
    if (query_timed_property(PROPERTY_CLEANED) == 0)
    {
      // if we haven't clean the room, do it now

      // store a property for 1000 seconds
      add_timed_property(PROPERTY_CLEANED, 1, 1000);

      // we can modify the description of an existing item
      modify_item("desk", "The desk seems to have been clean recently.\n");

      // we can give messages to the player that starts the action (this_player())
      // with tell_object or to every object inside this room with tell_room
      tell_object(this_player(), "You clean the desk and find a paper on it.\n");
      tell_room(this_object(), this_player()->query_cap_name() + " cleans the desk " +
          "finding a paper on it.\n", this_player());
      // the last parameter in tell_room is the object or list of objects to exclude.
      // As we already notified the player with tell_object, we want the tell_room
      // to nofify every OTHER player in the room.

      // we can clone an item and move it to this room
      clone_object("/games/hexagon/areas/admin/items/newcreator_paper.c")->move(this_object());

      // we return 1 because the action was finished right
      return 1;
    }

    // you have already clean the room
    tell_object(this_player(), "You already did that.\n");
    // important: this counts as finish the action right, too. If we 
    // return a 0, the system will try to do the action in other object, but
    // we know we have already found it
    return 1;
  }

  notify_fail("Clean what? Maybe you want to 'clean room'.\n");
  // return 0 because this was not the object we were trying to "clean",
  return 0;
}

int do_move(string str)
{
  if (!strlen(str))
  {
    notify_fail("Move what?\n");
    return 0;
  }  

  if (str == "desk")
  {
    tell_object(this_player(), "You move the desk, finding a trapdoor " +
      "on the floor.\n");
    tell_room(this_object(), this_player()->query_cap_name()+" moves the desk.\n",
      this_player());
    trapdoor = add_exit("trapdoor", "/games/hexagon/areas/admin/development.c", "door");
    trapdoor->set_init_status(0);

    // trapdoor->set_gender(2); <- for spanish language, not needed in english
    
    // when we modify the exits of an already existing room, we need to 
    // call renew_exits()
    renew_exits();
    // we can use this basic system to check exists and see if a player
    // is allowed to use it. We will modify the exit "trapdoor"
    // using a local function called "is_allowed". If it returns 0,
    // the player is not allowed
    modify_exit("trapdoor", ({ "function", "is_allowed" }));
    return 1;
  }

  notify_fail("Move what? Maybe you want to 'move desk'.\n");
  return 0;
}

// example local function to allow using an exit
// you can use it only if your are a coder
int is_allowed(string str, object ob, string special_mess)
{
  if (ob->query_coder())
  {
    return 1;
  }

  notify_fail("Only coders can use this exit.\n");
  return 0;
}
