// file_size - get the size of a file
// int file_size( string file );
// file_size() returns the size of file 'file' in bytes.  Size -1
// indicates that 'file' either does not exist, or that it is not
// readable. Size -2 indicates that 'file' is a directory.

// implementation from http://phantasmal.sourceforge.net/DGD/external/file_size.c

static nomask int file_size(string path)
{
    int i;
    mixed **dir;
    string *comps, base;

    if (!strlen(path))
      return 0;

    if (path == "/")
      return -2;

    comps = explode(path, "/");
    base = comps[sizeof(comps) - 1];
    dir = get_dir(path);
    i = sizeof(dir[0]);

    while (i--)
      if (dir[0][i] == base)
        return dir[1][i];

    return -1;
}
