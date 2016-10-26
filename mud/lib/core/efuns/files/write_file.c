
// write_file - appends a string to a file
// int write_file( string file, string str, int flag );

// Append the string `str' into the file `file'. Returns 0 or 1 for
// failure or success.  If flag is 1, write_file overwrites instead of
// appending.

int write_file(string file, string str, varargs int offset)
{
  if (!SECURE->valid_write(file, geteuid(), previous_function()))
    return 0;

  return ::write_file(file, str, offset);
}

