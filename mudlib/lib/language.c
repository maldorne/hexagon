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
string scramble_sentence(string text, int speaker, int listener);

// called from the language handler, receives the full message 
// to "translate" and the start piece "xxx says", "xxx exclaims", etc
mixed garble(string start, string mess)
{
  return ({ start, garbler(mess) });
}

// called from this object, "translates" the real message to show.
// The reader does not understand this language at all (the handler only
// garbles when the language is not among the reader's known ones), so
// scramble every word at full distortion. scramble_sentence takes a
// speaker and a listener competence; 0/0 means "no shared understanding",
// which drives total_distortion above AVERAGE_LEVEL for every word.
string garbler(string mess)
{
  return scramble_sentence(mess, 0, 0);
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

// Distort a single word using the speaker / listener competence, or return
// it unchanged. Empty input yields empty output. Split out of
// scramble_sentence so the sentence walker can scramble word runs while
// leaving whitespace untouched.
private string scramble_one_word(string word, int speaker, int listener)
{
  int speaker_distortion, listener_distortion, total_distortion;

  if (!strlen(word))
    return "";

  speaker_distortion = strlen(word) - speaker;
  if (speaker_distortion < 0)
    speaker_distortion = 0;

  listener_distortion = strlen(word) - listener;
  if (listener_distortion < 0)
    listener_distortion = 0;

  total_distortion = speaker_distortion + listener_distortion;

  if (random(AVERAGE_LEVEL) < total_distortion)
    return scramble_word(word, total_distortion);

  return word;
}

// Scramble each whole word of `text` while preserving every space, tab and
// newline exactly, so a laid-out sign (columns, indentation) keeps its shape.
// Walks the string collecting word runs and flushing them scrambled whenever
// a whitespace character is hit; the whitespace itself is copied verbatim.
// (The old version exploded/imploded on a single space, which collapsed
// runs of spaces and dropped indentation.)
string scramble_sentence(string text, int speaker, int listener)
{
  string out, word;
  int i, len, c;

  out = "";
  word = "";
  len = strlen(text);

  for (i = 0; i < len; i++)
  {
    c = text[i];

    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
    {
      out += scramble_one_word(word, speaker, listener);
      word = "";
      out += text[i .. i];
    }
    else
      word += text[i .. i];
  }

  // flush the trailing word (if the text did not end in whitespace)
  out += scramble_one_word(word, speaker, listener);

  return out;
} /* scramble_sentence() */
