#include <std.h>
#include <mud/cmd.h>
#include <translations/exits.h>

inherit CMD_BASE;

// Rotate or mirror a zone by rewriting the exit directions of every room
// file in it. Rotating all exits by the same amount rotates the whole map,
// because every room's connections turn together; the room coordinates are
// re-derived from the exits when the zone is converted to locations, so
// nothing else has to change.
//
// Directions are recognised in two independent forms:
//   - the DIR_* constant as written in source (DIR_WEST): language-agnostic,
//     and never appears in prose, so it is safe to rewrite anywhere.
//   - the localized literal string ("oeste"): taken from
//     <translations/exits.h> so it follows the mud's compile language. Only
//     quoted occurrences are rewritten, so room descriptions that happen to
//     mention a direction word are left untouched.
//
// Rewriting is two-pass with unique sentinels: pass 1 turns each source
// direction into a marker that is NOT a direction, pass 2 expands the
// markers into the rotated directions. This stops an already-rotated exit
// from being rotated again (e.g. N->E then E->S turning the new E into S).
// Created from sar.c, Folken 02/2010; generalized 2026.

#define NDIRS 8

string query_short_help()
{
  return "Rotate (90 clockwise) or mirror a zone by rewriting its rooms' exits.";
}

// Directions in clockwise ring order: N, NE, E, SE, S, SW, W, NW. Returns the
// target index of each source index for the requested transform.
private int * _rotation(int mirror_x, int mirror_y)
{
  int i;
  int * m;

  // flip over the horizontal axis: N<->S, NE<->SE, SW<->NW; E, W fixed
  if (mirror_x)
    return ({ 4, 3, 2, 1, 0, 7, 6, 5 });
  // flip over the vertical axis: E<->W, NE<->NW, SE<->SW; N, S fixed
  if (mirror_y)
    return ({ 0, 7, 6, 5, 4, 3, 2, 1 });

  // rotate 90 clockwise: two steps along the ring
  m = allocate_int(NDIRS);
  for (i = 0; i < NDIRS; i++)
    m[i] = (i + 2) % NDIRS;
  return m;
}

static int cmd(string str, object me, string verb)
{
  string * files, * dconst, * dname;
  string s1, s2;
  int mirror_x, mirror_y, i, j, count;
  int * rot, * order;

  if (!str || str == "")
  {
    notify_fail("Syntax: rotate [ -mirrorx | -mirrory ] <file(s)>\n" +
                "        (no flag rotates the zone 90 degrees clockwise)\n");
    return 0;
  }

  mirror_x = mirror_y = 0;
  if (sscanf(str, "%s %s", s1, s2) == 2)
  {
    if (s1 == "-mirrorx")      { mirror_x = 1; str = s2; }
    else if (s1 == "-mirrory") { mirror_y = 1; str = s2; }
  }

  files = get_files(str);
  if (!sizeof(files))
  {
    notify_fail("No files matching '" + str + "'.\n");
    return 0;
  }

  dconst = ({ "DIR_NORTH", "DIR_NORTHEAST", "DIR_EAST", "DIR_SOUTHEAST",
              "DIR_SOUTH", "DIR_SOUTHWEST", "DIR_WEST", "DIR_NORTHWEST" });
  dname  = ({ DIR_NORTH, DIR_NORTHEAST, DIR_EAST, DIR_SOUTHEAST,
              DIR_SOUTH, DIR_SOUTHWEST, DIR_WEST, DIR_NORTHWEST });
  rot = _rotation(mirror_x, mirror_y);
  // diagonals first so DIR_NORTH cannot swallow the DIR_NORTH... prefix of
  // DIR_NORTHEAST / DIR_NORTHWEST (same for DIR_SOUTH).
  order = ({ 1, 3, 5, 7, 0, 2, 4, 6 });

  count = 0;
  for (i = 0; i < sizeof(files); i++)
  {
    string content;

    if (file_size(files[i]) <= 0)
      continue;

    content = read_file(files[i]);
    if (!content)
    {
      write("Could not read '" + files[i] + "'.\n");
      continue;
    }

    // pass 1: source direction -> unique non-direction sentinel
    for (j = 0; j < sizeof(order); j++)
    {
      int d;
      d = order[j];
      if (rot[d] == d)
        continue;
      content = replace(content, dconst[d], "@@RC" + d + "@@");
      content = replace(content, "\"" + dname[d] + "\"", "@@RN" + d + "@@");
    }
    // pass 2: sentinel -> rotated direction token
    for (j = 0; j < sizeof(order); j++)
    {
      int d;
      d = order[j];
      if (rot[d] == d)
        continue;
      content = replace(content, "@@RC" + d + "@@", dconst[rot[d]]);
      content = replace(content, "@@RN" + d + "@@", "\"" + dname[rot[d]] + "\"");
    }

    write_file(files[i], content, 1);
    write("Rotated exits in '" + files[i] + "'.\n");
    count++;
  }

  write("Done: " + count + " file" + (count == 1 ? "" : "s") + " rewritten.\n");
  return 1;
}
