
static nomask string show_trace()
{
  mixed **trace;
  string progname, objname, func, str, result;
  int i, sz, line, len;

  result = "";
  trace = call_trace();

  if ((sz = sizeof(trace) - 1) != 0)
  {
    for (i = 0; i < sz; i++)
    {
      objname  = trace[i][0];
      progname = trace[i][1];
      func     = trace[i][2];
      line     = trace[i][3];

      if (line == 0)
        str = "    ";
      else
      {
        str = "    " + line;
        str = str[strlen(str) - 4 ..];
      }

      str += " " + func + " ";
      len = strlen(func);

      if (len < 22)
        str += "                      "[len ..];

      str += " " + progname;

      if (progname != objname)
      {
        len = strlen(progname);
        if (len < strlen(objname) && progname == objname[.. len - 1])
          str += " (" + objname[len ..] + ")";
        else
          str += " (" + objname + ")";
      }

      // call_other is masked ("because of reasons"... shadows)
      // remove those messages from the error trace
      if (func == "call_other" &&
          progname == "/lib/core/auto")
        continue;

      // when we are using this efun from the error handler, we can 
      // ignore the last entry
      if (func == "runtime_error" &&
         (progname == "/lib/core/errors" || progname == "/lib/core/driver"))
        continue;

      result += str + "\n";
    }
  } 

  return result; 
}

static nomask string last_call()
{
  mixed **trace;
  string progname, objname, func, result;
  int i, sz, line, len;

  result = "";
  trace = call_trace();

  if ((sz = sizeof(trace) - 1) != 0)
  {
    // start from the end
    for (i = sz - 1; i >= 0; i--)
    {
      progname = trace[i][1];
      func = trace[i][2];

      if (progname == "/lib/core/driver")
        continue;

      if (func == "runtime_error" &&
          progname == "/lib/core/errors")
        continue;

      if (func == "call_other" &&
          progname == "/lib/core/auto")
        continue;

      objname  = trace[i][0];
      line     = trace[i][3];

      if (line == 0)
        result = "    ";
      else
      {
        result = "    " + line;
        result = result[strlen(result) - 4 ..];
      }

      result += " " + func + " ";
      len = strlen(func);

      if (len < 22)
        result += "                      "[len ..];

      result += " " + progname;

      if (progname != objname)
      {
        len = strlen(progname);
        if (len < strlen(objname) && progname == objname[.. len - 1])
          result += " (" + objname[len ..] + ")";
        else
          result += " (" + objname + ")";
      }

      return result + "\n";
    }
  } 

  return result; 
}
