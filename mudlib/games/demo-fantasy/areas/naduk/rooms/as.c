// Created by Lummen 24-7-97

#include "../path.h"

inherit "/std/shop";

void setup()
{
  set_short("Tienda");
  set_long("Te encuentras en una pequenya tienda del pueblo de Naduk. En este"
           " sitio puedes comprar y vender toda clase de objetos que posean algun valor. Los "
           "comandos son:\n\n"
           "  <list>    para listar.\n"
           "  <comprar> para comprar.\n"
           "  <vender>  para vender.\n"
           "  <ojear>   para examinar un objeto y ver cuanto vale.\n"
           "  <valorar> para valorar un objeto antes de venderlo.\n\n");

  set_light(60);
  add_exit(DIR_SOUTH, ROOMS + "a4.c", "door");
  set_store_room(ROOMS + "almacen1.c");
  add_property("no_undead", 1);
}
