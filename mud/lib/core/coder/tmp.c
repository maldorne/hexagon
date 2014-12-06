
// tmp commands to help building the mudos -> dgd port


// taken from kernellib
static void cmd_compile(string str)
{
  // mixed *files;
  // string *names;
  // int num, i, len;
  object obj;
  string path;

  if (!str) 
  {
    write("Usage: compile <file>\n");
    return;
  }

  // write(" * compile this: " + str + "\n");
  path = resolve_path(str);
  // write(" * resolved path: " + path + "\n");

  if (!file_exists(path))
  {
    write("Not an LPC source file: " + str + "\n");
    return;
  }

  obj = compile_object(path[..strlen(path)-3]);

  if (obj) 
  {
    // store(obj);
    write(object_name(obj) + " compiled.\n");
  }


  // old kernellib cmd

  // files = expand(str, 1, TRUE); /* must exist, full filenames */
  // names = files[0];
  // num = sizeof(names);

  // for (i = 0; i < num; i++) {
  //   str = names[i];
  //   len = strlen(str);
  //   if (len < 2 || str[len - 2 ..] != ".c") {
  //       write("Not an LPC source file: " + str + "\n");
  //   } else {
  //       obj = compile_object(str[ .. len - 3]);
  //       if (obj) {
  //         store(obj);
  //       }
  //   }
  // }
}

