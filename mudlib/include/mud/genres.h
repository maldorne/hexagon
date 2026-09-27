#ifndef GENRES_H
#define GENRES_H

// Game genres: stable English ids, never translated. A game master declares
// one with set_genre(); what a player reads comes from table("genres"), which
// maps each id to its name in the mud's language.

#define GENRE_FANTASY          "fantasy"
#define GENRE_SCIENCE_FICTION  "science-fiction"
#define GENRE_WESTERN          "western"
#define GENRE_CONTEMPORARY     "contemporary"
#define GENRE_HORROR           "horror"
#define GENRE_POST_APOCALYPTIC "post-apocalyptic"
#define GENRE_HISTORICAL       "historical"

#endif
