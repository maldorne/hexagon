
/* I was hoping I never had to touch this code..
 * But it seems like i have to.
 * Adding a "loading" property, so that players won't be able to quit before 
 * all equipment are loaded.
 * Baldrick, nov '94
 * Taniwha 1995, 1996 (ad nauseum)
 * Wonderflug 1996, first real work in months, time to get this working
 * well. (and ohhhhh the bugs)
 *
 * changed for hexagon, using a mapping of arrays instead of an array of mappings
 *  auto_load[file_name] = ({ attribute_map_item_1, ... attribute_map_item_n })
 * neverbot 10/2021
 */

#include <language.h>

void make_iou(string filename, mixed bad_info, object dest);

/*
 * Returns a mixed array which can be converted back to objects using
 * load_auto_load
 */
mapping create_auto_load(object * obs)
{
  mapping  attribute_map; 
  string   obj_filename;
  string * fname;
  mapping  auto_load;
  int      i;

  if (!obs) 
    return ([ ]);

  obs -= ({ nil });
  auto_load = ([ ]);

  for (i = 0; i < sizeof(obs); i++)
  {
    // avoid_auto_load returns nonzero if the object should not save.
    // formerly queyr_auto_load, this was poorly named, so I changed it, neverbot 10/2021
    if (obs[i]->avoid_auto_load())
      continue;

    attribute_map = obs[i]->query_auto_load_attributes();

    // Perhaps this is too harsh
    // if (undefinedp(attribute_map) || !map_sizeof(attribute_map))
    // if attribute_map is empty, it just means the item is not custom, 
    // all its values are default... but the item exists!!
    if (undefinedp(attribute_map))
      continue;

    // obs[i] may've become null, it may not have a filename, or
    // it may have a filename "", this protects any of that from adding
    // it to the auto load.
    if (obs[i] && (obj_filename = file_name(obs[i])) && 
      (fname = explode(obj_filename, "#")) &&
      sizeof(fname) > 0 )
    {
      obj_filename = fname[0];
      if (undefinedp(auto_load[obj_filename]))
        auto_load[obj_filename] = ({ attribute_map });
      else 
        auto_load[obj_filename] += ({ attribute_map });
    }
  }

  return auto_load;
} /* create_auto_load() */

/*
 * Loads all the items and their values in auto_load into dest.
 * Returns all those objects that loaded.
 */
object * load_auto_load(mapping auto_load, object dest) 
{
  object ob, * obs;
  string * files;
  int i, j;

  if (undefinedp(auto_load) || !map_sizeof(auto_load))
    return ({ });

  obs = ({ });
  files = keys(auto_load);
  
  for (i = 0; i < sizeof(files); i++)
  {
    for (j = 0; j < sizeof(auto_load[files[i]]); j++)
    {
      // Try to clone one
      catch( ob = clone_object(files[i]) );

      if (ob)
      {
        mapping attribute_map;
        int move_ret;

        attribute_map = auto_load[files[i]][j];

        ob->init_auto_load_attributes(attribute_map);

        // ob might've bit it during init, or dest might not exist
        if (!ob || !dest || (move_ret = ob->move(dest)) )
        {
          // ob or dest don't exist, or couldn't move ob into dest
          if (ob)
          {
            log_file("loader", "[" + ctime(time(), 4) + "] " + 
              (string)this_player()->query_name() + " dropped a [" +
              (string)ob->query_name() + "].\n"); //, move_ret="+move_ret+".\n");

            // if we can't move to environment, well, too bad
            ob->move(environment(this_player()));
          }

          tell_object(this_player(), _LANG_AUTO_LOAD_DROP_SOMETHING);
        }

        if (ob)
          obs += ({ ob });
      }
      else
      {
        // And if it didn't work , give em an IOU
        mapping iou_info;

        tell_object(this_player(), _LANG_AUTO_LOAD_SOMETHING_WRONG);

        // build mapping with the info of ONLY this item
        iou_info[files[i]] = auto_load[files[i]][j];
        make_iou(files[i], iou_info, dest);
      }
    }
  }

  obs -= ({ nil });
  return obs;
} /* load_auto_load() */

/*
 * Creates an IOU for objects that failed to load.
 */
void make_iou(string filename, mixed bad_info, object dest)
{
  object iou;

  log_file("loader", "[" + ctime(time(), 4) + "] " + 
    (string)dest->query_cap_name() + " had [" + filename + 
    "] refuse to load.\n");

  catch( iou = clone_object("/lib/obj/iou.c") );

  if (iou)
  {
    iou->add_auto_load_info(filename, bad_info);
    catch(iou->move(dest));
  }
}
