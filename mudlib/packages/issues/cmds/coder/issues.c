
#include <mud/cmd.h>
#include "../../src/issues.h"

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "issues" }));
  set_usage("issues [<number>] [remove | close | open]");
  set_help("Shows every issue in the issue tracker.");
}

string print_issue(mixed issue, varargs int extended_info)
{
  string where, ret, title_color, where_color;

  ret = "";
  where = "";
  title_color = "";
  where_color = "";


  if (sizeof(issue[ISSUE_ORIGINAL_PATHS]) == 1)
    where = issue[ISSUE_ORIGINAL_PATHS][0];
  else
  {
    if (!extended_info)
      where = issue[ISSUE_ORIGINAL_PATHS][0] + " (...)";
    else
    {
      int i;
      where += "\n";

      for (i = 0; i < sizeof(issue[ISSUE_ORIGINAL_PATHS]); i++)
      {
        where += "       - " +issue[ISSUE_ORIGINAL_PATHS][i] + "\n";
      }      
    }
  }

  if (!extended_info)
    ret += "   " + issue[ISSUE_ID] + ") ";
  else
    ret += "\n      ";

  if (issue[ISSUE_STATUS] == ISSUE_STATUS_OPEN)
  {
    title_color = "%^GREEN%^";
    where_color = "%^CYAN%^";
  }

  ret += title_color + issue[ISSUE_TITLE] + "%^RESET%^" +
         " [%^ORANGE%^" + (issue[ISSUE_STATUS] == ISSUE_STATUS_OPEN ? "open" : "closed") + "%^RESET%^]\n" +
         "      in " + where_color + where + "%^RESET%^\n";

  if (issue[ISSUE_TIMES] > 1)
  {
    ret += "      " + issue[ISSUE_TIMES] + " times from " + ctime(issue[ISSUE_CREATION_TIME], 4) + 
           " to " + ctime(issue[ISSUE_LAST_TIME], 4) + "\n";
  }
  else
  {
    ret += "      " + ctime(issue[ISSUE_CREATION_TIME], 4) + "\n";
  }

  // if (extended_info)
    ret += "\n";

  return ret;
}

static int cmd(string str, object me, string verb)
{
  mapping issues;
  string * keys_issues;
  string ret, action;
  int i, index;

  issues = package("issues")->query_issues();
  keys_issues = keys(issues);

  // action on an issue
  if (sscanf(str, "%d %s", index, action) == 2)
  {
    mixed issue;
    issue = package("issues")->query_issue_by_id(index);

    if (!issue)
    {
      write("No issue found with id: " + index + ".\n");
      return 1;
    }

    if (action == "remove")
    {
      package("issues")->remove_issue(issue[ISSUE_TITLE]);
      write("Issue %^GREEN%^" + issue[ISSUE_TITLE] + "%^RESET%^ removed.\n");
    }
    else if (action == "close")
    {
      package("issues")->close_issue(issue[ISSUE_TITLE]);
      write("Issue %^GREEN%^" + issue[ISSUE_TITLE] + "%^RESET%^ closed.\n");
    }
    else if (action == "open")
    {
      package("issues")->open_issue(issue[ISSUE_TITLE]);
      write("Issue %^GREEN%^" + issue[ISSUE_TITLE] + "%^RESET%^ opened.\n");
    }
    else
    {
      write("Invalid option.\n");
    }

    return 1;
  }

  // ask for info about a specific issue
  if (sscanf(str, "%d", index) == 1)
  {
    mixed issue;
    issue = package("issues")->query_issue_by_id(index);

    if (!issue)
    {
      write("No issue found with id: " + index + ".\n");
      return 1;
    }

    // 1 flag == extended info
    write(print_issue(issue, 1));

    return 1;
  }

  ret = "\n\n" +
        "   Pending issues in the issue tracker: " + sizeof(keys_issues) + "\n" +
        "   -----------------------------------------\n";

  for (i = 0; i < sizeof(keys_issues); i++)
    ret += print_issue(issues[keys_issues[i]]);

  ret += "\n";

  // write(ret);
  me->user()->more_string(ret, "Issues");
  return 1;
}
