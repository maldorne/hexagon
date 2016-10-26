
string read_file(string file, varargs int offset, int size)
{
  if (!SECURE->valid_read(file, geteuid(), previous_function()))
    return "";

  return ::read_file(file, offset, size)  ;
}


// read_file - read a file into a string
// string read_file( string file, int start_line, int number_of_lines );

// Read a line of text from a file into a string.  The second and third
// arguments are optional.  If only the first argument is specified, the
// entire file is returned (as a string).

// The start_line is the line number of the line you wish to read.  This routine
// will return 0 if you try to read past the end of the file, or if you try to
// read from a nonpositive line.

string read_file_line(string file, varargs int start_line, int number_of_lines)
{
  int i, current_line, where, line_count;
  string read, ret;

  if (start_line < 0)
    return "";

  if (!SECURE->valid_read(file, geteuid(), previous_function()))
    return "";

  ret = "";
  current_line = 1;
  line_count = 0;
  where = 0;

  if (!number_of_lines)
    number_of_lines = 1;

  catch
  {
    if (start_line)
      while ((read = ::read_file(file, where, 1)) && (current_line < start_line))
      {
        where++;
        if (read == "\n")
          current_line++;
      }

    while ((read = ::read_file(file, where, 1)) && (line_count < number_of_lines))
    {
      ret += read;
      where ++;

      if (read == "\n")
        line_count++;
    }
  }

  return ret;
}

