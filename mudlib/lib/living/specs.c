
/*
 * Añadido nuevo sistema de especialidades para cada uno de los grupos sociales
 *  neverbot 20/10/2012
 */

#include <mud/secure.h>
#include <living/social.h>

int * spec_object_list;

void create()
{
  spec_object_list = allocate_int(NUM_SOCIAL_OBJECTS);
}

void specs_commands()
{

} 

int * query_spec_object_list() { return spec_object_list; }
void set_spec_object_list(int * list)
{
  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;  

  spec_object_list = list;
}

// Para el sistema de especialidades
int query_guild_spec() { return spec_object_list[GUILD_OB]; }
int query_job_spec() { return spec_object_list[JOB_OB]; }

void set_guild_spec(int value) { spec_object_list[GUILD_OB] = value; }
void set_job_spec(int value) { spec_object_list[JOB_OB] = value; }

void reset_guild_spec() { spec_object_list[GUILD_OB] = 0; }
void reset_job_spec() { spec_object_list[JOB_OB] = 0; }

mixed * stats() 
{
  return ({ 
    ({"Spec Object List", spec_object_list, }),
  });
}
