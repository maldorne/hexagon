/*
 * The wonderful singing language of the elves.
 */

inherit "/lib/language.c";

string garbler(string mess);

string long()
{
  return "Consists of the musical elvish language.\n";
}

mixed garble(string start, string mess)
{
  return ({ start, garbler(mess) });
}

string garbler(string mess)
{
  mess = implode(explode(mess, "k"), "llew");
  mess = implode(explode(mess, "r"), "hle");
  mess = implode(explode(mess, "g"), "eal");
  mess = implode(explode(mess, "p"), "");
  mess = implode(explode(mess, "t"), "lh");
  mess = implode(explode(mess, "y"), "i");
  mess = implode(explode(mess, "e"), "a");
  mess = implode(explode(mess, "a"), "e");
  mess = implode(explode(mess, "d"), "ith");
  mess = implode(explode(mess, "ick"), "aal");
  mess = implode(explode(mess, "h"), "alh");
  mess = implode(explode(mess, "ave"), "ath");
  mess = implode(explode(mess, "I"), "Iil");
  mess = implode(explode(mess, "s"), "tha");
  mess = implode(explode(mess, "v"), "ou");
  mess = implode(explode(mess, "b"), "");
  mess = implode(explode(mess, "f"), "v");

  return mess;
}
