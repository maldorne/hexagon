
private string number_as_string_aux(int n);
static string number_as_string(int n);

static string query_num(int n, int limit)
{
  if (limit && n>limit)
    return _LANG_NUMBERS_LOADS_OF; /* this is a little pointless ... */
  return number_as_string(n);
}

static string number_as_string(int n)
{
  string ret;
  int i;

  if (n < 0)
    return _LANG_NUMBERS_THOUSANDS_OF; /* fo wraps... */
  if (n > 99999)
    return _LANG_NUMBERS_THOUSANDS_OF; /* sorry 'bout the hard limit */

  ret = nil;

  if ((i = n/1000))
  {
    n = n%1000;
    if (i == 1)
      ret = _LANG_NUMBERS_ONE_THOUSAND;
    else
      ret = number_as_string_aux(i) + " " + _LANG_NUMBERS_THOUSAND;
    if (!n)
      return ret;
  }

  if ((i = n/100))
  {
    n = n%100;

    if (!ret)
      ret = "";
    else
      ret += " ";

    switch (i)
    {
      case 1: ret += _LANG_NUMBERS_100; break;
      case 2: ret += _LANG_NUMBERS_200; break;
      case 3: ret += _LANG_NUMBERS_300; break;
      case 4: ret += _LANG_NUMBERS_400; break;
      case 5: ret += _LANG_NUMBERS_500; break;
      case 6: ret += _LANG_NUMBERS_600; break;
      case 7: ret += _LANG_NUMBERS_700; break;
      case 8: ret += _LANG_NUMBERS_800; break;
      case 9: ret += _LANG_NUMBERS_900; break;
    }

    if (!n)
      return ret;
  }

  if (ret)
    return ret + " " + number_as_string_aux(n);

  return number_as_string_aux(n);
} /* query_num() */

private string number_as_string_aux(int n)
{
  string ret;

  if (!n)
    return _LANG_NUMBERS_ZERO;

  if (n > 99)
    return _LANG_NUMBERS_LOTS_OF;

  if (n<30 && n>9)
    return ({ _LANG_NUMBERS_10, _LANG_NUMBERS_11, _LANG_NUMBERS_12,
      _LANG_NUMBERS_13, _LANG_NUMBERS_14, _LANG_NUMBERS_15,
      _LANG_NUMBERS_16, _LANG_NUMBERS_17, _LANG_NUMBERS_18,
      _LANG_NUMBERS_19, _LANG_NUMBERS_20, _LANG_NUMBERS_21,
      _LANG_NUMBERS_22, _LANG_NUMBERS_23, _LANG_NUMBERS_24,
      _LANG_NUMBERS_25, _LANG_NUMBERS_26, _LANG_NUMBERS_27,
      _LANG_NUMBERS_28, _LANG_NUMBERS_29, })[n-10];

  ret = ({ "", "", "", _LANG_NUMBERS_30, _LANG_NUMBERS_40,
    _LANG_NUMBERS_50, _LANG_NUMBERS_60, _LANG_NUMBERS_70,
    _LANG_NUMBERS_80, _LANG_NUMBERS_90, })[n/10];

  if ((n = n%10) && (ret != ""))
    ret += _LANG_NUMBERS_CONCAT;

  return ret + ({ "", _LANG_NUMBERS_1, _LANG_NUMBERS_2,
    _LANG_NUMBERS_3, _LANG_NUMBERS_4, _LANG_NUMBERS_5,
    _LANG_NUMBERS_6, _LANG_NUMBERS_7, _LANG_NUMBERS_8,
    _LANG_NUMBERS_9, })[n];
} /* number_as_string() */

