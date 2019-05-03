
mixed process_value(string func)
{
  int tmp;
  string arg;
  object obj;
  // mixed ret;

  if (!func || !stringp(func))
    return 0;

  if ((tmp = at('|', func)) != -1) {
    arg = func[tmp+1..];
    func = func[0..tmp-1];
  }
  if ((tmp = at(':', func)) != -1) {
    obj = find_object(func[tmp+1..]);
    func = func[0..tmp-1];
  } else {
    obj = previous_object();
  }
  if (!obj)
    return 0;

  return call_other(obj, func, explode(arg, "|")...);
}

string process_string(string str)
{
  int pr_start, il;
  string * parts;

  parts = explode(str, "@@");

  pr_start = !(str[0..1]=="@@");

  for (il = pr_start; il < sizeof(parts); il += 2) {
    string tmp;
    tmp = process_value(parts[il]);

    if (stringp(tmp))
      parts[il] = tmp;
  }

  return implode(parts, "");
}

