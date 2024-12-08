
// write_file - appends a string to a file
// int write_file( string file, string str, int flag );

// Append the string `str' into the file `file'. Returns 0 or 1 for
// failure or success.  If flag is 1, write_file overwrites instead of
// appending.

// TODO: in DGD the third parameter is an offset, not the overwrite flag
// review the code to see where we need this

static int write_file(string file, string str, varargs int offset)
{
  string * pieces, current;
  int i;

  debug("files", "write_file: " + file + "\n");

  if (!SECURE->valid_write(file, geteuid(), previous_function()))
  {
    debug("files", "write_file invalid write: " + file + "\n");
    return 0;
  }

  pieces = explode(file, "/");
  current = "";

  // create every subdirectory if it does not exist
  for (i = 0; i < sizeof(pieces) - 1; i++)
  {
    current += "/" + pieces[i];

    if (file_size(current) > 0)
      return 0;

    if (file_size(current) == -1)
      make_dir(current);
  }

  return ::write_file(file, str, offset);
}

