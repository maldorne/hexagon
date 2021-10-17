
/* I was hoping I never had to touch this code..
 * But it seems like i have to.
 * Adding a "loading" property, so that players won't be able to quit before 
 * all equipment are loaded.
 * Baldrick, nov '94
 * Taniwha 1995, 1996 (ad nauseum)
 * Wonderflug 1996, first real work in months, time to get this working
 * well. (and ohhhhh the bugs)
 */

#include <language.h>

/* elements of a strob - saveable (string but not really) version
 * of an object, which is stored as a mixed array.  These are the indices.
 */
#define FILENAME 0
#define VARMAP   1

void auto_clone(object ob, mapping attribute_map, object dest);

/*
 * Returns a mixed array which can be converted back to objects using
 * load_auto_load
 */
mixed * create_auto_load(object * obs)
{
  mapping  attribute_map; 
  string   obj_filename;
  string * fname;
  mixed  * auto_load;
  int      i;

  if (!obs) 
    return ({ });

  /* instead of growing the auto_load, we allocate and then subtract
   * zeros after we're done.
   */
  obs -= ({ nil });
  auto_load = allocate(sizeof(obs));

  for (i = 0; i < sizeof(obs); i++)
  {
    /* query_auto_load returns nonzero if the object should not save.
     * this is poorly named.
     */
    if (obs[i]->query_auto_load())
      continue;

    attribute_map = obs[i]->query_auto_load_attributes();

    /* Perhaps this is too harsh */
    if (!attribute_map)
      continue;

    /* obs[i] may've become null, it may not have a filename, or
     * it may have a filename "", this protects any of that from adding
     * it to the auto load.
     */
    if (obs[i] && (obj_filename = file_name(obs[i])) && 
      (fname = explode(obj_filename, "#")) &&
      sizeof(fname) > 0 )
    {
      obj_filename = fname[0];
      auto_load[i] = ({ obj_filename, attribute_map });
    }

    /* what is the first field used for ? */
  }

  auto_load -= ({ nil });
  return auto_load;
} /* create_auto_load() */

/*
 * Creates an IOU for objects that fail to load.
 */

void make_iou(mixed bad_strob, object dest)
{
  object iou;

  log_file("loader", "["+ctime(time(), 4)+"] "+ 
    (string)dest->query_cap_name()+" had ["+(string)bad_strob[FILENAME]+
    "] refuse to load.\n");

  catch( iou = clone_object("/lib/obj/iou.c") );

  if (iou)
  {
    iou->add_auto_string(bad_strob);
    catch(iou->move(dest));
  }
}

/*
 * Loads all the strobs in auto_load into dest.
 * Returns all those objects that loaded.
 */
object * load_auto_load(mixed * auto_load, object dest) 
{
  object ob, * obs;
  int i;

  if (!auto_load || !sizeof(auto_load))
    return ({ });

  auto_load -= ({ nil });
  obs = allocate(sizeof(auto_load));
  
  for (i = 0; i < sizeof(auto_load); i++)
  {
    mixed * strob;
    strob = auto_load[i];

    if (sizeof(strob) != 2)
      continue;

    if (!stringp(strob[FILENAME]))
      continue;

    // Try to clone one
    catch( ob = clone_object(strob[FILENAME]) );

    if (ob)
    {
      auto_clone(ob, strob[VARMAP], dest);
      if (ob)
        obs += ({ ob });
    }
    else
    {
      // And if it didn't work , give em an IOU
      make_iou(strob, dest);
    }
  }

  obs -= ({ nil });
  return obs;
} /* load_auto_load() */

void auto_clone(object ob, mapping attribute_map, object dest)
{
  int move_ret;

  if (!ob)
  {
    tell_object(this_player(), _LANG_AUTO_LOAD_SOMETHING_WRONG);
    return;
  }

  ob->init_auto_load_attributes(attribute_map);

  // ob might've bit it during init, or dest might not exist
  if (!ob || !dest || (move_ret = ob->move(dest)) )
  {
    /* ob or dest don't exist, or couldn't move ob into dest */
    if (ob)
    {
      log_file("loader", "["+ctime(time(), 4)+"] "+ 
        (string)this_player()->query_name()+" dropped a ["+
        (string)ob->query_name()+"].\n"); //, move_ret="+move_ret+".\n");

      /* if we can't move to environment, well, too bad */
      ob->move(environment(this_player()));
    }

    tell_object(this_player(), _LANG_AUTO_LOAD_DROP_SOMETHING);
  }
} /* auto_clone() */

/* Hamlet added me -- I'm used for the 'update' command. */
/* I changed this to create_object_auto_load, it is after all quite
 * a generally useful function, so create_update_auto_load was silly
 * --wf.
 */
mixed * create_object_auto_load(object ob)
{
  mapping attribute_map;
  mixed * strob;
  string  obj_filename;
  int     j;

  if (!ob) 
    return ({ });

  attribute_map = ob->query_auto_load_attributes();

  if (!attribute_map)
    return ({ });

  if (sscanf(file_name(ob), "%s#%d", obj_filename, j) == 2)
    strob = ({ obj_filename, attribute_map });
  else
    strob = ({ file_name(ob), attribute_map });

  return strob;
} /* create_update_auto_load() */
