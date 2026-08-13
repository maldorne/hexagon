// Name generator engine.
//
// Learns the "shape" of a set of example names from a plain wordlist (one
// example per line) and produces new names that resemble them without
// reproducing any single one. It is a character n-gram sampler: it records,
// for every run of `order` characters seen in the examples, which characters
// tend to follow, then walks that table one character at a time -- starting
// from a run of start markers and stopping at an end marker -- picking each
// next character at random, weighted by how often it followed in the source.
// Longer `order` sticks closer to the examples; shorter `order` wanders more.
//
// The engine is culture-agnostic: the same code serves dwarven, elvish or
// orcish names -- swap the wordlist and the style changes. Wordlists belong to
// the games that use the generator, not here.
//
// Callers reach it through the package master, e.g.
//   NAMEGEN_OB->generate_name("/games/<game>/.../elvish.names")

#define NG_DEFAULT_ORDER 2   // characters of context the sampler remembers
#define NG_MIN_LEN       3   // reject shorter results (a stray fragment)
#define NG_MAX_LEN       12  // reject / cut off longer results
#define NG_MAX_TRIES     40  // resamples before giving up the length/copy test

#define NG_START "^"         // padding before a word; never emitted
#define NG_END   "$"         // marks the end of a word; stops the walk

// Where the packaged starter wordlists live (used by generate_for).
#define NG_COLLECTIONS "/packages/namegen/collections/"

// Cache: "path|order" -> ([ "order": n, "table": ngram, "words": examples ]).
// A wordlist is read and modelled once, then reused across calls.
private mapping models;

private void _init()
{
  if (!models)
    models = ([ ]);
}

// Read a wordlist file into an array of lowercased example names, dropping
// blank lines, surrounding spaces, and lines starting with '#' (comments).
private string * load_words(string path)
{
  string raw;
  string * lines, * words;
  int i;

  raw = read_file(path);
  if (!raw)
    return ({ });

  lines = explode(raw, "\n");
  words = ({ });
  for (i = 0; i < sizeof(lines); i++)
  {
    string w;

    w = lines[i];
    while (strlen(w) && w[0] == ' ')
      w = w[1 ..];
    while (strlen(w) && w[strlen(w) - 1] == ' ')
      w = w[0 .. strlen(w) - 2];

    if (!strlen(w) || w[0] == '#')
      continue;

    words += ({ lower_case(w) });
  }

  return words;
}

// Build the n-gram table: for every window of `order` characters in the
// padded examples, collect the character that follows. Characters are stored
// with repetition, so a plain random index over the array is already weighted
// by source frequency.
private mapping build_table(string * words, int order)
{
  mapping table;
  string pad;
  int i, j;

  table = ([ ]);
  pad = "";
  for (j = 0; j < order; j++)
    pad += NG_START;

  for (i = 0; i < sizeof(words); i++)
  {
    string w;

    w = pad + words[i] + NG_END;
    for (j = order; j < strlen(w); j++)
    {
      string ctx, ch;

      ctx = w[j - order .. j - 1];
      ch = w[j .. j];
      if (!table[ctx])
        table[ctx] = ({ });
      table[ctx] += ({ ch });
    }
  }

  return table;
}

// Fetch (or lazily build and cache) the model for a wordlist at `order`.
private mapping model_for(string path, int order)
{
  string key;

  _init();
  key = path + "|" + order;
  if (!models[key])
  {
    string * words;

    words = load_words(path);
    if (!sizeof(words))
      return nil;

    models[key] = ([
      "order": order,
      "table": build_table(words, order),
      "words": words,
    ]);
  }

  return models[key];
}

// Walk the table once to produce a single raw (lowercase) name. Starts from a
// run of start markers, appends one weighted-random next character at a time,
// and stops at the end marker or when `max_len` characters have been emitted.
private string sample(mapping model, int max_len)
{
  mapping table;
  string ctx, out;
  int order, steps;

  order = model["order"];
  table = model["table"];

  ctx = "";
  for (steps = 0; steps < order; steps++)
    ctx += NG_START;

  out = "";
  for (steps = 0; steps < max_len; steps++)
  {
    string * nexts, ch;

    nexts = table[ctx];
    if (!nexts || !sizeof(nexts))
      break;

    ch = nexts[random(sizeof(nexts))];
    if (ch == NG_END)
      break;

    out += ch;
    // slide the window: keep the last `order` characters as the new context
    ctx = (ctx + ch)[1 ..];
  }

  return out;
}

// Generate one name from a per-culture wordlist file. `order`, `min_len` and
// `max_len` are optional (0 = use the defaults). Rejects results that are too
// short, too long, or an exact copy of a source example, resampling up to
// NG_MAX_TRIES; returns nil only if the wordlist is missing or empty. The name
// is lowercase (like a stored id); capitalise it at display time.
string generate_name(string path, varargs int order, int min_len, int max_len)
{
  mapping model;
  string name;
  int tries;

  if (order <= 0)   order = NG_DEFAULT_ORDER;
  if (min_len <= 0) min_len = NG_MIN_LEN;
  if (max_len <= 0) max_len = NG_MAX_LEN;

  model = model_for(path, order);
  if (!model)
    return nil;

  // `name` keeps the most recent sample that is NOT an exact source copy,
  // whatever its length. We never return a copy: a high `order` on a small
  // wordlist reproduces whole examples, and reading the source list back is
  // not generating. If a sample also passes the length filter we return it at
  // once; otherwise the remembered non-copy is the fallback.
  name = "";
  for (tries = 0; tries < NG_MAX_TRIES; tries++)
  {
    string s;

    s = sample(model, max_len);

    // we want new names, not the source list read back
    if (member_array(s, model["words"]) != -1)
      continue;

    name = s;
    if (strlen(s) >= min_len && strlen(s) <= max_len)
      return s;
  }

  // no sample passed the length filter: return the last non-copy we saw, or
  // nil if every single sample reproduced a source name (order too high for
  // this wordlist) so the caller can react rather than get a copy
  return strlen(name) ? name : nil;
}

// Generate `count` names from the same wordlist. Missing/empty wordlist
// yields an empty array.
string * generate_names(string path, int count, varargs int order,
                        int min_len, int max_len)
{
  string * out;
  int i;

  out = ({ });
  for (i = 0; i < count; i++)
  {
    string name;

    name = generate_name(path, order, min_len, max_len);
    if (name)
      out += ({ name });
  }

  return out;
}

// Generate a name for a named collection and gender from the packaged
// wordlists. Resolves the path as collections/<collection>.<gender>.names,
// falling back to collections/<collection>.names when a collection has no
// gendered lists (e.g. an androgynous one). `gender` is "male" / "female" (or
// ""); the other arguments are the same optional order/min/max as
// generate_name. Returns nil when no wordlist is found, so a caller can fall
// back to its own naming.
string generate_for(string collection, string gender, varargs int order,
                    int min_len, int max_len)
{
  string path;

  if (!collection || !strlen(collection))
    return nil;

  path = nil;
  if (gender && strlen(gender))
  {
    string gendered;
    gendered = NG_COLLECTIONS + collection + "." + gender + ".names";
    if (file_size(gendered) > 0)
      path = gendered;
  }
  if (!path)
    path = NG_COLLECTIONS + collection + ".names";
  if (file_size(path) <= 0)
    return nil;

  return generate_name(path, order, min_len, max_len);
}

// Drop every cached model, so an edited wordlist is re-read on next use.
void reset_cache()
{
  models = ([ ]);
}
