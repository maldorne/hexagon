
static string read_file(string file, varargs int offset, int size)
{
  if (!SECURE->valid_read(file, geteuid(), previous_function()))
    return "";

  return ::read_file(file, offset, size);
}

// read_bytes - reads a contiguous series of bytes from a file into a string
// string read_bytes( string path, int start, int length );

// This function reads 'length' bytes beginning at byte # 'start' in the
// file named 'path'.  The bytes are returned as a string.  Note that
// (start + length) must not be past the end of the file or else read_bytes
// will fail.  If the second and third arguments are omitted, the entire file
// is returned.

static string read_bytes(string file, varargs int start, int length)
{
  // returns characters, not bytes
  return read_file(file, start, length);
}

// read_file - read a file into a string
// string read_file( string file, int start_line, int number_of_lines );

// Read a line of text from a file into a string.  The second and third
// arguments are optional.  If only the first argument is specified, the
// entire file is returned (as a string).

// The start_line is the line number of the line you wish to read.  This routine
// will return 0 if you try to read past the end of the file, or if you try to
// read from a nonpositive line.

static string read_file_line(string file, varargs int start_line, int number_of_lines)
{
  int i, current_line, where, line_count, size;
  string read, ret;

  if (start_line < 0)
    return "";

  if (!SECURE->valid_read(file, geteuid(), previous_function()))
    return "";

  ret = "";
  current_line = 1;
  line_count = 0;
  where = 0;
  size = file_size(file);

  if (!number_of_lines)
    number_of_lines = 1;

  catch
  {
    if (start_line)
      while ((read = ::read_file(file, where, 1)) && (current_line < start_line) && (where < size))
      {
        where++;
        if (read == "\n")
          current_line++;
      }

    while ((read = ::read_file(file, where, 1)) && (line_count < number_of_lines) && (where < size))
    {
      ret += read;
      where++;

      if (read == "\n")
        line_count++;
    }
  }

  return ret;
}

