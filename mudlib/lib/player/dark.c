
// refactored from player.c, neverbot 01/2025

int check_dark(int light)
{
  string s;

  if (this_object()->query_dead())
    return 0;

  if (s = this_object()->query_race_ob())
    return (int)load_object(s)->query_dark(light);

  return 0;
}
