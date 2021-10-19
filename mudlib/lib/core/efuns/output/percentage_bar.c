
// useful for different commands
// neverbot 08/05, based on the score command

static string percentage_bar(int value)
{
  string str;
  string aux_1, aux_2;
  int i;

  aux_1 = "********************";
  aux_2 = "                    ";

  if (value < 1)
    value = 1;
  if (value > 100)
    value = 100;

  i = (20 * value)/100;

  str = aux_1[0..i-1] + aux_2[i..19];
  aux_1 = "%^BOLD%^RED%^" + str[0..3] +
          "%^MAGENTA%^" + str[4..7] +
          "%^YELLOW%^" + str[8..11] +
          "%^GREEN%^" + str[12..19] +
          "%^RESET%^";
  /*
  aux_1 = ""         + str[0..9] +
          "%^BOLD%^" + str[10..13] +
          "%^CYAN%^" + str[14..17] +
          "%^RED%^"  + str[18..19] +
          "%^RESET%^";
  */
  return aux_1;
}

// useful for different commands
// neverbot 09/21, based on the score command

static string bar(int value)
{
  string result, full_character, empty_character;
  int i, j;

  result = "";

  // exec return char_codes("░")

  // █ == 226 150 136
  // full_character = "" + chr(226) + chr(150) + chr(136);
  // ▓ == 226 150 147
  full_character = "" + chr(226) + chr(150) + chr(147);
  // ░ == 226 150 145
  empty_character = "" + chr(226) + chr(150) + chr(145);
  // empty_character = " ";

  if (value < 1)
    value = 1;
  if (value > 100)
    value = 100;

  j = (10 * value)/100;

  result += "%^BOLD%^RED%^";
  for (i = 0; i < 10; i++)
  {
    if (i < j && i == 2)
      result += "%^MAGENTA%^";
    if (i < j && i == 4)
      result += "%^YELLOW%^";
    if (i < j && i == 7)
      result += "%^GREEN%^";
    if (i == j)
      result += "%^RESET%^";

    if (i < j)
      result += full_character;
    else
      result += empty_character;
  }
  result += "%^RESET%^";

  return result;
}
