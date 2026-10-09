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

   10/2026  - Moved from /lib/room to /lib/core/basic and inherited by
              object.c, so anything can smell, sound, taste or feel of
              something: a room, a location, an item, an npc. Only the data
              is here; the player commands (smell, listen, taste, feel) ask
              for it. neverbot
*/

// What something smells, sounds, tastes and feels like, by the word a player
// uses: ([ sense : ([ word : text ]) ]). The word "default" is the thing
// itself, what a sense command answers without a word or when the word names
// this object. Empty until a sense is added, so the many objects without any
// pay nothing for it.
static mapping senses;

#define SENSE_DEFAULT "default"

private void add_sense(string sense, mixed words, string text)
{
  int i;

  if (!words || !text)
    return;

  if (!senses)
    senses = ([ ]);
  if (!senses[sense])
    senses[sense] = ([ ]);

  if (!pointerp(words))
    words = ({ words });

  for (i = 0; i < sizeof(words); i++)
    senses[sense][words[i]] = text;
}

private void remove_sense(string sense, string word)
{
  if (!word || !senses || !senses[sense])
    return;

  map_delete(senses[sense], word);
}

// The text of a sense for a word, or nil. Without a word, the thing itself.
string query_sense(string sense, varargs string word)
{
  if (!senses || !senses[sense])
    return nil;

  return senses[sense][word ? word : SENSE_DEFAULT];
}

// Every sense this object has, to be copied somewhere else (a location takes
// the ones of the room it is converted from).
mapping query_senses()
{
  mapping out;
  string * kinds;
  int i;

  out = ([ ]);
  if (!senses)
    return out;

  kinds = map_indices(senses);
  for (i = 0; i < sizeof(kinds); i++)
    out[kinds[i]] = ([ ]) + senses[kinds[i]];

  return out;
}

void set_senses(mapping m)
{
  string * kinds;
  int i;

  senses = ([ ]);
  if (!m)
    return;

  kinds = map_indices(m);
  for (i = 0; i < sizeof(kinds); i++)
    if (mappingp(m[kinds[i]]))
      senses[kinds[i]] = ([ ]) + m[kinds[i]];
}

// add_XXX takes a word or a list of words, and can also be used to change the
// text of a word already added
void add_smell(mixed smell, string smell_desc) { add_sense("smell", smell, smell_desc); }
void add_sound(mixed sound, string sound_desc) { add_sense("sound", sound, sound_desc); }
void add_taste(mixed taste, string taste_desc) { add_sense("taste", taste, taste_desc); }
void add_feel(mixed feel, string feel_desc)    { add_sense("feel", feel, feel_desc); }

void remove_smell(string smell) { remove_sense("smell", smell); }
void remove_sound(string sound) { remove_sense("sound", sound); }
void remove_taste(string taste) { remove_sense("taste", taste); }
void remove_feel(string feel)   { remove_sense("feel", feel); }

// What the thing itself smells, sounds, tastes or feels like.
void set_smell(string text) { add_sense("smell", SENSE_DEFAULT, text); }
void set_sound(string text) { add_sense("sound", SENSE_DEFAULT, text); }
void set_taste(string text) { add_sense("taste", SENSE_DEFAULT, text); }
void set_feel(string text)  { add_sense("feel", SENSE_DEFAULT, text); }
