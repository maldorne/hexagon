// The text editor each user writes with, saved with the account.

#include <user/editor.h>
#include <language.h>

string editor_mode;

void create()
{
  editor_mode = EDITOR_MODE_MENU;
}

void editor_commands()
{
  add_action("set_editor", _LANG_EDITOR_VERBS);
}

string query_editor_mode()
{
  if (member_array(editor_mode, EDITOR_MODES) == -1)
    return EDITOR_MODE_MENU;

  return editor_mode;
}

// The name of the mode in the user's language.
string query_editor_name()
{
  return _LANG_EDITOR_MODE_NAMES[query_editor_mode()];
}

// Takes the mode by any of its translated names. ed is only for coders.
int set_editor_name(string name)
{
  mapping aliases;
  string * modes;
  int i;

  aliases = _LANG_EDITOR_MODE_ALIASES;
  modes = map_indices(aliases);
  name = lower_case(trim(name ? name : ""));

  for (i = 0; i < sizeof(modes); i++)
    if (member_array(name, aliases[modes[i]]) != -1)
    {
      if (modes[i] == EDITOR_MODE_ED && !this_object()->query_coder())
        return 0;

      editor_mode = modes[i];
      return 1;
    }

  return 0;
}

int set_editor(string str)
{
  if (!strlen(str))
  {
    write(_LANG_EDITOR_CURRENT + _LANG_EDITOR_OPTIONS);
    return 1;
  }

  if (!set_editor_name(str))
  {
    notify_fail(_LANG_EDITOR_UNKNOWN + _LANG_EDITOR_OPTIONS);
    return 0;
  }

  write(_LANG_EDITOR_SELECTED);
  return 1;
}

// Edit a text with the user's editor; end_ob->end_f gets the text, or nil.
void do_edit(string text, string end_f, varargs object end_ob)
{
  if (!end_ob)
    end_ob = previous_object();

  clone_object(EDITOR_OB)->start(this_object()->player(), text, end_f, end_ob,
                                 query_editor_mode());
}

mixed * stats()
{
  return ({
    ({ "Editor", query_editor_mode() }),
  });
}
