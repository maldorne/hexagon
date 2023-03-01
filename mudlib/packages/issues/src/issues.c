/* 
 * This is the handler for the issue tracker. 
 */

inherit "/lib/core/object.c";

#include "issues.h"

#define HOME "/room/weather_room.c"
#define SAVE_FILE "/save/issues/issues"

// issues[title] = [array of data]
// every issue in the mapping has an array of data:
//     issue[title][ISSUE_ID]             = int as issue id
//     issue[title][ISSUE_TITLE]          = same as mapping key
//     issue[title][ISSUE_STATUS]         = int to define status (open, closed, etc)
//     issue[title][ISSUE_CREATION_TIME]  = creation time
//     issue[title][ISSUE_LAST_TIME]      = last time the issue happen
//     issue[title][ISSUE_TIMES]          = times put in the issue tracker
//     issue[title][ISSUE_ORIGINAL_PATHS] = list of strings with the paths of the 
//                                     objects where the issue happened
mapping issues;

// total num of issues since the beginning (used as issue id)
int num_issues;

mapping query_issues() { return issues; }

void create()
{
  issues = ([ ]);
  num_issues = 0;

  // TO DO:
  // if /save/issues directory does not exist, create it

  restore_object(SAVE_FILE, 1);

  ::create();
}

void setup()
{
  // anticloning like obj/shut, neverbot 6/03
  if (file_name(this_object()) != base_name(this_object())) 
  {
    
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }
  
  reset_get();
  set_name("issue tracker");
  set_short("Issue Tracker");
  set_long("This is the issue tracker!\n");
  add_alias("tracker");
  add_plural("trackers");
  
  // move masked, always move the object to HOME
  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  ::move(HOME, messin, messout);
}

void save_handler() 
{ 
  save_object(SAVE_FILE, 1); 
}

void dest_me() 
{
  save_object(SAVE_FILE, 1);
  ::dest_me();
}

// return an specific issue from the list of issues
mixed query_issue_by_id(int id)
{
  string * keys;
  int i;

  keys = keys(issues);

  for (i = 0; i < sizeof(keys); i++)
    if (issues[keys[i]][ISSUE_ID] == id)
      return issues[keys[i]];

  return nil;
}

int add_issue(string title, object ob)
{
  title = trim(title);
  
  if (undefinedp(issues[title]))
  {
    num_issues++;
    issues[title] = ({ num_issues, title, ISSUE_STATUS_OPEN, time(), time(), 1, ({ base_name(ob) }) });

    save_handler();
    return 1;
  }
  
  issues[title][ISSUE_LAST_TIME] = time();
  issues[title][ISSUE_TIMES]++;
  issues[title][ISSUE_STATUS] = ISSUE_STATUS_OPEN;

  if (member_array(base_name(ob), issues[title][ISSUE_ORIGINAL_PATHS]) == -1)
    issues[title][ISSUE_ORIGINAL_PATHS] += ({ base_name(ob) });

  save_handler();
  return 1;
}

int remove_issue(string title)
{
  if (undefinedp(issues[title]))
    return 0;

  map_delete(issues, title);
  save_handler();
  return 1;
}

int open_issue(string title)
{
  if (undefinedp(issues[title]))
    return 0;

  issues[title][ISSUE_STATUS] = ISSUE_STATUS_OPEN;
  save_handler();
  return 1;
}

int close_issue(string title)
{
  if (undefinedp(issues[title]))
    return 0;

  issues[title][ISSUE_STATUS] = ISSUE_STATUS_CLOSED;
  save_handler();
  return 1;
}
