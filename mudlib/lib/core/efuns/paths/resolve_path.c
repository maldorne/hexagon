
/**
 * Normalises a path string: expands `~` and `~name`, removes `.`,
 * resolves `..`. Pure string manipulation.
 *
 * Counterpart to `get_path`: this one is safe to call from contexts
 * with no logged-in user (driver callbacks like `object_type` and
 * `include_file`, master object, etc.). It only touches `this_player()`
 * when expanding `~`, and falls back to `/home/admin` in that one case
 * if no player is available. Everything else (`.`, `..`, plain paths)
 * needs nothing from the runtime context.
 *
 * Returns "" on invalid paths (e.g. leading `..`, or `..` that would
 * escape the root).
 *
 * Idea taken from the Melville mudlib.
 */
static nomask string resolve_path(string path)
{
  int i, j;
  string * dirs;

  if (!path) 
    return "";
  
  dirs = explode(path, "/");
  
  // remove any . from the array
  dirs -= ({ "." });

  for (i = 0; i < sizeof(dirs); i++) 
  {
    if (!dirs[i] || dirs[i]=="") 
      continue;
    
    if (dirs[i][0]=='~') 
    {
      if (!this_player())
        dirs[i] = "/home/admin";
      else if (strlen(dirs[i])==1) 
        dirs[i] = "/home/" + this_player()->query_name();
      else 
        dirs[i] = "/home/" + dirs[i][1..];

      dirs = dirs[i..];
    }
  }

  // remove any .. and the preceding element
  i = member_array("..", dirs);
  while (i > -1) 
  {
    // can't start with ..  
    if (i == 0) 
      return "";

    j = sizeof(dirs);

    // piece it back together, depending on if we remove the first two
    //   elements, the last two, or two from the middle. 
    if (i == 1) 
    {
      if (j == 2) 
        return "";
      dirs = dirs[i+1 .. j-1];
    } 
    else 
    {
      if (i == j-1) 
        dirs = dirs[0 .. i-2];
      else 
        dirs = dirs[0 .. i-2] + dirs[i+1 .. j-1];
    }

    // look for another .. in the path. 
    i = member_array("..",dirs);
  }

  path = implode(dirs,"/");
  return path;
}
