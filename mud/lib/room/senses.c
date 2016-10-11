/*
   Senses inheritable file, This stuff all gets inherited into std/room and
   is available for use from there - the functions are documented later in
   the file.
   By Sojan of Salford UK - I can be found as Sojan@discworld or Sojan@FR
   most frequently or at j.s.greenland@eee.salford.ac.uk
   Bing on.

   Version 1.1-Beta

   17/11/94 - Now upgraded for handling the ({item,item,item,}) syntax that
              people have requested ....
              Version # now 1.1

   01/15/96 - Added a default to all senses. When set to default the room
              displays the message when the sense command is typed ....
              Verkho - Version 1.2
*/

// function declarations

void create();
void init();

int do_smell(string what);
int do_taste(string what);
int do_feel(string what);
int do_listen(string what);

void add_smell(mixed smell, string smell_desc);
void add_taste(mixed taste, string taste_desc);
void add_sound(mixed sound, string sound_desc);
void add_feel(mixed feel, string feel_desc);

void remove_smell(string smell);
void remove_taste(string taste);
void remove_sound(string sound);
void remove_feel(string feel);

// variable declarations

static mapping smells, feels, tastes, sounds;

void create() 
{
  feels = ([ ]);
  sounds = ([ ]);
  tastes = ([ ]);
  smells = ([ ]);
}

void init()
{
  add_action("do_smell",  "olfatear");
  add_action("do_smell",  "oler");
  add_action("do_listen", "escuchar");
  add_action("do_listen", "oir");
  add_action("do_taste",  "probar");
  add_action("do_taste",  "saborear");
  add_action("do_feel",   "sentir");
  add_action("do_feel",   "tocar");
  ::init();
}

int do_smell(string what) {
  if(!what||!smells[what]) {
    if(smells["default"]) {
      notify_fail(smells["default"]+"\n");
      return 0;
    }
    notify_fail("¿"+capitalize(query_verb())+" el qué?\n");
    return 0;
  }
  write(smells[what]+"\n");
  return 1;
} 

int do_taste(string what) {
  if(!what||!tastes[what]) {
    if(tastes["default"]) {
      notify_fail(tastes["default"]+"\n");
      return 0;
    }
    notify_fail("¿"+capitalize(query_verb())+" el qué?\n");
    return 0;
  }
  write(tastes[what]+"\n");
  return 1;
} 

int do_feel(string what) {
  if(!what||!feels[what]) {
    if(feels["default"]) {
      notify_fail(feels["default"]+"\n");
      return 0;
    }
    notify_fail("¿"+capitalize(query_verb())+" el qué?\n");
    return 0;
  }
  write(feels[what]+"\n");
  return 1;
}

int do_listen(string what) {
  if(!what||!sounds[what]) {
    if(sounds["default"]) {
      notify_fail(sounds["default"]+"\n");
      return 0;
    }
    notify_fail("¿"+capitalize(query_verb())+" el qué?\n");
    return 0;
  }
  write(sounds[what]+"\n");
  return 1;
} 

// Okay - thats the actual functions for players to interact with the senses
// now for some control code for the programmers to add stuff to them with

void add_smell(mixed smell, string smell_desc) {
  int i;
  if(!smell||!smell_desc) return;
  if(pointerp(smell)) {
    for(i=0;i<sizeof(smell);i++)
       smells[smell[i]] = smell_desc;
    return;
  }
  smells[smell]=smell_desc;
  return;
} 

void remove_smell(string smell) {
  if(!smell) return;
  smells=m_delete(smells, smell);
  return;
} 

void add_taste(mixed taste, string taste_desc) {
  int i;
  if(!taste||!taste_desc) return;
  if(pointerp(taste)) {
    for(i=0;i<sizeof(taste);i++)
       tastes[taste[i]]=taste_desc;
    return;
  }
  tastes[taste]=taste_desc;
  return;
} 

void remove_taste(string taste) {
  if(!taste) return;
  tastes=m_delete(tastes, taste);
  return;
} 

void add_sound(mixed sound, string sound_desc) {
  int i;
  if(!sound||!sound_desc) return;
  if(pointerp(sound)) {
    for(i=0;i<sizeof(sound);i++)
       sounds[sound[i]]=sound_desc;
    return;
  }
  sounds[sound]=sound_desc;
  return;
} 

void remove_sound(string sound) {
  if(!sound) return;
  sounds=m_delete(sounds, sound);
  return;
} 

// I couldn't decide wether to call this pair XXX_feel or XXX_touch
// I'm still unsure but its XXX_feel for now.

void add_feel(mixed feel, string feel_desc) {
  int i;
  if(!feel||!feel_desc) return;
  if(pointerp(feel)) {
    for(i=0;i<sizeof(feel);i++)
       feels[feel[i]]=feel_desc;
    return;
  }
  feels[feel]=feel_desc;
  return;
} 

void remove_feel(string feel) {
  if(!feel) return;
  feels=m_delete(feels, feel);
  return;
} 

// Okay add/remove stuff over - in case you were wondering the add_XXX
// routine can actually be used to modify the sense in question as well
// due to the simplistic coding it uses

