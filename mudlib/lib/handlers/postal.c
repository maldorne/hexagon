// /adm/daemon/postal_d.c
// from the Nightmare Mudlib
// the postal daemon... handles all mail sending and receiving
//    "radix",
// Added retire_user and age_mail : Radix - March 6, 1997
//
// Rewritten for Hexagon, neverbot 09/2026: every character keeps their
// mailbox (letters and personal groups) in their own save directory, and
// every recipient gets their own copy of a letter.

#include <mud/secure.h>
#include <files/postal.h>
#include <user/player.h>
#include <language.h>

// The mailbox loaded right now, and whose it is. Every call loads the
// mailbox it needs, so these only live for the length of one call.
mapping * letters;
mapping groups;
static string owner;

private void load_mailbox(string who);
private void save_mailbox();
private int find_letter(string id);
private string * without_repeats(string * names);

void create()
{
  seteuid(ROOT);
  letters = ({ });
  groups = ([ ]);
  owner = "";
}

int valid_access(string func)
{
  if (geteuid(previous_object()) == ROOT)
    return 1;

  if (member_array(base_name(previous_object()), TRUSTED_MAILERS) != -1)
    return 1;

  log_file("illegal", "attempt to access postal daemon function " + func + " by " +
    (this_player() ? this_player()->query_name() : getuid(previous_object())) +
    " from file " + file_name(previous_object()) + " [" + ctime(time(), 4) + "]\n");

  return 0;
}

private void load_mailbox(string who)
{
  owner = lower_case(who);
  letters = ({ });
  groups = ([ ]);
  restore_object(player_save_dir(owner) + MAILBOX_SAVE, 1);
}

private void save_mailbox()
{
  if (!sizeof(letters) && !map_sizeof(groups))
  {
    if (file_exists(player_save_dir(owner) + MAILBOX_SAVE + ".o"))
      rm(player_save_dir(owner) + MAILBOX_SAVE + ".o");
    return;
  }

  mkdir(player_save_dir(owner));
  save_object(player_save_dir(owner) + MAILBOX_SAVE, 1);
}

private int find_letter(string id)
{
  int i;

  for (i = 0; i < sizeof(letters); i++)
    if (letters[i]["id"] == id)
      return i;

  return -1;
}

private string * without_repeats(string * names)
{
  string * result;
  int i;

  result = ({ });

  for (i = 0; i < sizeof(names); i++)
    if (member_array(names[i], result) == -1)
      result += ({ names[i] });

  return result;
}

// A character that can receive mail: one with a save file. Guests have none,
// but one could be online under a name nobody saved yet, so ask them too.
int valid_recipient(string who)
{
  object ob;

  if (!strlen(who))
    return 0;

  who = lower_case(who);

  if ((ob = find_player(who)) && ob->query_property(GUEST_PROP))
    return 0;

  return player_exists(who);
}

// Letters in the mailbox, oldest first. Each one is a mapping with
// id, from, to, cc, subject, date, read and body.
mapping * query_letters(string who)
{
  int i;
  mapping * result;

  load_mailbox(who);
  result = ({ });

  for (i = 0; i < sizeof(letters); i++)
    result += ({ ([ ]) + letters[i] });

  return result;
}

mapping mail_status(string who)
{
  int unread, i;

  load_mailbox(who);

  for (i = 0; i < sizeof(letters); i++)
    if (!letters[i]["read"])
      unread++;

  return ([ "unread" : unread, "total" : sizeof(letters) ]);
}

// Resolve a list of names and personal groups of the sender into the
// characters that will get the letter. Names that are neither go to unknown.
mapping expand_recipients(string from, string * names)
{
  string * found, * unknown, * members;
  string name;
  int i, j;

  load_mailbox(from);
  found = ({ });
  unknown = ({ });

  for (i = 0; i < sizeof(names); i++)
  {
    name = lower_case(names[i]);

    if (!strlen(name))
      continue;

    if (groups[name])
    {
      members = groups[name];
      for (j = 0; j < sizeof(members); j++)
        if (valid_recipient(members[j]))
          found += ({ members[j] });
        else
          unknown += ({ members[j] });
    }
    else if (valid_recipient(name))
      found += ({ name });
    else
      unknown += ({ name });
  }

  return ([ "found" : without_repeats(found), "unknown" : without_repeats(unknown) ]);
}

// Deliver a copy of the letter to every recipient in to and cc, which must
// already be expanded. Returns the characters that got it.
string * post_mail(string from, string * to, string * cc, string subject, string body)
{
  string * recipients, * delivered;
  mapping letter;
  object ob;
  int i;

  if (!valid_access("post mail"))
    return ({ });

  from = lower_case(from);

  if (!valid_recipient(from))
    return ({ });

  to = without_repeats(to);
  cc = without_repeats((cc ? cc : ({ })) - to);
  recipients = to + cc;
  delivered = ({ });

  letter = ([
    "id"      : "" + time() + "-" + from + "-" + random(100000),
    "from"    : from,
    "to"      : to,
    "cc"      : cc,
    "subject" : subject,
    "date"    : time(),
    "read"    : 0,
    "body"    : body,
  ]);

  for (i = 0; i < sizeof(recipients); i++)
  {
    if (!valid_recipient(recipients[i]))
      continue;

    load_mailbox(recipients[i]);
    letters += ({ ([ ]) + letter });
    save_mailbox();
    delivered += ({ recipients[i] });

    if (ob = find_player(recipients[i]))
      tell_object(ob, _LANG_POSTAL_NEW_MAIL);
  }

  return delivered;
}

void mark_read(string who, string id)
{
  int i;

  if (!valid_access("mark read"))
    return;

  load_mailbox(who);

  if ((i = find_letter(id)) == -1 || letters[i]["read"])
    return;

  letters[i]["read"] = 1;
  save_mailbox();
}

int remove_letters(string who, string * ids)
{
  int i, removed;

  if (!valid_access("remove letters"))
    return 0;

  load_mailbox(who);

  for (i = sizeof(letters) - 1; i >= 0; i--)
    if (member_array(letters[i]["id"], ids) != -1)
    {
      letters = letters[0..i-1] + letters[i+1..];
      removed++;
    }

  save_mailbox();
  return removed;
}

// Personal groups: a name standing for a list of characters, only usable by
// the character who made it.
mapping query_groups(string who)
{
  load_mailbox(who);
  return ([ ]) + groups;
}

string * add_to_group(string who, string group, string * names)
{
  string * added;
  int i;

  if (!valid_access("add group"))
    return ({ });

  load_mailbox(who);
  group = lower_case(group);
  added = ({ });

  if (!groups[group])
    groups[group] = ({ });

  for (i = 0; i < sizeof(names); i++)
    if (valid_recipient(names[i]) &&
        member_array(lower_case(names[i]), groups[group]) == -1)
      added += ({ lower_case(names[i]) });

  added = without_repeats(added);
  groups[group] += added;

  if (!sizeof(groups[group]))
    map_delete(groups, group);

  save_mailbox();
  return added;
}

string * remove_from_group(string who, string group, string * names)
{
  string * removed;
  int i;

  if (!valid_access("remove group"))
    return ({ });

  load_mailbox(who);
  group = lower_case(group);

  if (!groups[group])
    return ({ });

  for (i = 0; i < sizeof(names); i++)
    names[i] = lower_case(names[i]);

  removed = groups[group] & names;
  groups[group] -= removed;

  if (!sizeof(groups[group]))
    map_delete(groups, group);

  save_mailbox();
  return removed;
}

// Radix : March 7, 1997
// Remove the letters older than MAIL_AGED. Returns how many went.
int age_mail(string who)
{
  string * old;
  int i;

  if (!valid_access("age mail"))
    return 0;

  load_mailbox(who);
  old = ({ });

  for (i = 0; i < sizeof(letters); i++)
    if (letters[i]["date"] + MAIL_AGED < time())
      old += ({ letters[i]["id"] });

  if (!sizeof(old))
    return 0;

  return remove_letters(who, old);
}

// Radix : March 5, 1997
void retire_user(string who)
{
  if (!who || !valid_access("retire user"))
    return;

  who = lower_case(who);

  //  if (this_player() && !this_player()->query_admin() &&
  //  this_player()->query_name() != who)
  //  return;

  // if (find_player(who))
  //  tell_object(find_player(who), "Borrando tu mail.\n");

  if (file_exists(player_save_dir(who) + MAILBOX_SAVE + ".o"))
    rm(player_save_dir(who) + MAILBOX_SAVE + ".o");
}
