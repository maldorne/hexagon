/* language system */
/* Version 1 */

#include <language.h>

inherit "/lib/core/object.c";

#define AVERAGE_LEVEL   15

string long()
{
  return _LANG_LANGUAGE_DEFAULT_DESC;
}

string garbler(string mess);

// called from the language handler, receives the full message 
// to "translate" and the start piece "xxx says", "xxx exclaims", etc
mixed garble(string start, string mess)
{
  return ({ start, garbler(mess) });
}

// called from this object, "translates" the real message to show
string garbler(string mess)
{
  return mess;
}

string scramble_word(string word, int total_distortion)
{
  /* version. replace only the vowels in words */
  /* have an individual test for each change */
  /* that way different words can be scrambled */
  /* differently. makes for a better system */  
  if (random(AVERAGE_LEVEL) < total_distortion)
  {
    word = implode(explode(word, "a"), "ay");
    word = implode(explode(word, "A"), "Ay");
  }

  if (random(AVERAGE_LEVEL) < total_distortion)
  {
    word = implode(explode(word, "e"), "ee");
    word = implode(explode(word, "E"), "Ee");
  }

  if (random(AVERAGE_LEVEL) < total_distortion)
  {
    word = implode(explode(word, "i"), "iy");
    word = implode(explode(word, "I"), "Iy");
  }

  if (random(AVERAGE_LEVEL) < total_distortion)
  {
    word = implode(explode(word, "o"), "oh");
    word = implode(explode(word, "O"), "Oh");
  }

  if (random(AVERAGE_LEVEL) < total_distortion)
  {
    word = implode(explode(word, "u"), "yu");
    word = implode(explode(word, "U"), "Yu");
  }

  return word;
} /* scramble_letter() */

string scramble_sentence(string text, int speaker, int listener)
{
  string * words;
  int x;
  int speaker_distortion;
  int listener_distortion;
  int total_distortion;

  /* first thing to do is to split the sentence into words */
  words = explode(text, " ");
  /* then step through the words 1 by 1 scrambling them */
  for (x = 0; x < sizeof(words); x++)
  {
    /* establish how much distortion comes from the speaker */
    /* the 2 multiplier is optional */
    speaker_distortion = strlen(words[x]) - speaker;
    
    /* we must check that the speaker distortion cannot be negative */
    if (speaker_distortion < 0)
      speaker_distortion = 0;
    
    /* establish how much distortion comes from the listener */
    /* the 2 multiplier is optional */
    listener_distortion = strlen(words[x]) - listener;
    
    /* again we must check listener_distortion is not negative */
    if (listener_distortion < 0)
      listener_distortion = 0;
    
    /* add these to find the total distortion */
    total_distortion = speaker_distortion + listener_distortion;
    
    /* add a bit of randomness, makes things different*/
    /* if true you failed to understand */
    if (random(AVERAGE_LEVEL) < total_distortion)
      words[x] = scramble_word(words[x], total_distortion);
  }
  
  /* return the new sentence */
  return implode(words, " ");
} /* scramble() */
