#include <light.h>

static int light, int_light;

void create(){
	light = 0;
	int_light = 0;
}

int query_light() {
  return light + int_light;
}

int query_my_light() { 
  return light;
}

int query_int_light() {
  return int_light;
}

int set_opaque() {
  if (!this_object()->query_property(OPAQUE) && environment())
    environment()->adjust_light(-int_light);
  this_object()->add_property(OPAQUE,1);
  return 1;
}

int reset_opaque() {
  if (this_object()->query_property(OPAQUE) && environment())
    environment()->adjust_light(int_light);
  this_object()->remove_property(OPAQUE);
}

/* this is used by objects being added to the inventory etc. */
int adjust_light(int i) {
  if (i) {
    if (environment() && !this_object()->query_property(OPAQUE))
      environment()->adjust_light(i);
    event(this_object(), "light_change", i);
  }
  return (int_light += i);
}

int set_light(int i) {
  if (light - i) {
    if (environment())
      environment()->adjust_light(i-light);
    event(this_object(), "light_change", i-light);
  }
  return (light = i);
}

mixed *stats() {
  return ({
    ({ "Light (nosave)", light }),
    ({ "Int Light (nosave)", int_light }),
    // ({ "opaco", this_object()->query_property(OPAQUE) }),
     });
}

/*
mixed *query_init_data() {
   return ({ "light", light, "set_light/p/" });
} 
*/
/* query_init_data() */
