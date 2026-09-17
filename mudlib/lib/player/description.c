// The description a player writes of their own character, shown when somebody
// looks at them instead of the one their race gives.

#define MAX_DESCRIPTION_LENGTH 600

string description;

void create()
{
  description = "";
}

string query_description() { return description ? description : ""; }
int query_max_description_length() { return MAX_DESCRIPTION_LENGTH; }

// An empty text goes back to the default description.
int set_description(string text)
{
  if (!text)
    text = "";

  text = trim(replace_string(text, "\n", " "));

  if (strlen(text) > MAX_DESCRIPTION_LENGTH)
    return 0;

  description = text;
  return 1;
}
