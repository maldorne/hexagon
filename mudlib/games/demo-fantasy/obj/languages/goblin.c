/*
 * The grunting goblinoid language.
 */

inherit "/lib/language.c";

string garbler(string mess);

string long()
{
  return "Consists of mostly grunting.\n";
}

mixed garble(string start, string mess)
{
  return ({ start, garbler(mess) });
}

string garbler(string mess)
{
  mess = implode(explode(mess, "k"), "kk");
  mess = implode(explode(mess, "p"), "hr");
  mess = implode(explode(mess, "g"), "ck");
  mess = implode(explode(mess, "p"), "");
  mess = implode(explode(mess, "t"), "fr");
  mess = implode(explode(mess, "y"), "o");
  mess = implode(explode(mess, "e"), "a");
  mess = implode(explode(mess, "a"), "e");
  mess = implode(explode(mess, "d"), "g");
  mess = implode(explode(mess, "i"), "ic");
  mess = implode(explode(mess, "ick"), "aal");
  mess = implode(explode(mess, "h"), "d");
  mess = implode(explode(mess, "ave"), "ertt");
  mess = implode(explode(mess, "I"), "Ick");
  mess = implode(explode(mess, "s"), "tha");
  mess = implode(explode(mess, "v"), "s");
  mess = implode(explode(mess, "m"), "");
  mess = implode(explode(mess, "v"), "f");

  return mess;
}
