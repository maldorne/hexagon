# namegen

A small, game-agnostic name generator. Give it a plain wordlist of example
names and it produces new names that resemble them without copying any one of
them.

## How it works

It is a character n-gram sampler. From the examples it records, for every run
of `order` characters, which characters tend to come next; then it builds a
name one character at a time, starting from a run of start markers and stopping
at an end marker, choosing each next character at random weighted by how often
it followed in the source. A higher `order` stays closer to the examples; a
lower one wanders further. Results that are too short, too long, or an exact
copy of a source name are rejected and resampled.

The engine knows nothing about any particular language or culture — the style
comes entirely from the wordlist you feed it. Swap the file and the flavour
changes.

## Wordlists

A wordlist is a plain text file, one example name per line. Blank lines and
lines starting with `#` are ignored.

Output quality scales with list size. On a small list (a few dozen names) use
`order` 2: higher orders memorise and just read the source back, which the
generator then rejects as copies, so results repeat. A few hundred names give a
higher `order` enough distinct runs to produce varied, realistic names without
copying. The shipped `collections/` are seed lists — grow them for better
results.

Names are often gendered, so where it matters a culture is split into separate
lists per gender and the caller picks the file matching the NPC it is naming;
androgynous cultures can stay in one list. The `collections/` folder ships a
starter set, named `<culture>[.<origin>].<gender>.names`:

- `elvish.names` — unisex, invented.
- `humans.fantasy.male.names`, `humans.fantasy.female.names` — invented,
  medieval-fantasy flavour.
- `humans.latin.male.names`, `humans.latin.female.names` — ordinary Spanish /
  Romance given names.
- `humans.english.male.names`, `humans.english.female.names` — ordinary English
  given names.

Real-name lists are kept free of diacritics so the byte-level sampler never
splits a multibyte character.

Games may add their own lists (their own cultures) and point the generator at
them; the engine is indifferent to where a wordlist lives.

## Use

```c
#include <namegen.h>

string name  = NAMEGEN_OB->generate_name(path);            // defaults
string fancy = NAMEGEN_OB->generate_name(path, 3, 4, 10);  // order 3, len 4..10
string * many = NAMEGEN_OB->generate_names(path, 8);       // eight at once
NAMEGEN_OB->reset_cache();                                 // after editing a list
```

`generate_name` returns a lowercase name (capitalise it yourself when you
display it), or `nil` if the wordlist is missing
or empty. `order`, `min_len` and `max_len` are optional (0 = default). Each
wordlist is read and modelled once, then cached per `order`.

## Layout

- `master.c` — the loadable object (`NAMEGEN_OB`); inherits the engine.
- `src/namegen.c` — the engine.
- `collections/` — starter wordlists per culture/gender.
