/*
 * genres.c
 *
 * Game genres catalogue. Maps the genre ids a game master declares with
 * set_genre() (see include/mud/genres.h) to their name in the mud's language.
 *
 * Callers reach this via the singleton accessor `table("genres")`.
 */

#include <mud/genres.h>
#include <translations/genres.h>

private mapping names;

void create()
{
  names = ([
    GENRE_FANTASY:          _LANG_GENRE_FANTASY,
    GENRE_SCIENCE_FICTION:  _LANG_GENRE_SCIENCE_FICTION,
    GENRE_WESTERN:          _LANG_GENRE_WESTERN,
    GENRE_CONTEMPORARY:     _LANG_GENRE_CONTEMPORARY,
    GENRE_HORROR:           _LANG_GENRE_HORROR,
    GENRE_POST_APOCALYPTIC: _LANG_GENRE_POST_APOCALYPTIC,
    GENRE_HISTORICAL:       _LANG_GENRE_HISTORICAL,
  ]);
}

// Every genre id the catalogue knows.
string * query_genres() { return map_indices(names); }

// The name of a genre as players read it; an unknown id reads as no genre.
string query_genre_name(string id)
{
  if (!id || !names[id])
    return _LANG_GENRE_UNKNOWN;

  return names[id];
}
