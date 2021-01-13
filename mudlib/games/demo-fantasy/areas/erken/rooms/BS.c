// Created by Lummen 24-7-97

#define ROOMS ROOMS + ""
inherit "/std/shop";

void setup()
{
  set_short("Tienda");
  set_long("Te encuentras en una pequenya tienda del pueblo de Naduk. En este"
           " comprar y vender toda clase de objetos que posean algun valor. Los "
           "comandos son:\n\n"
           "  <list>    para listar.\n"
           "  <comprar> para comprar.\n"
           "  <vender>  para vender.\n"
           "  <ojear>   para examinar un objeto y ver cuanto vale.\n"
           "  <valorar> para valorar un objeto antes de venderlo.\n\n");

  set_light(60);
  add_exit(DIR_OUT, ROOMS + "B6.c", "door");
  set_store_room(ROOMS + "almacen2.c");
  add_property("no_undead", 1);
}
