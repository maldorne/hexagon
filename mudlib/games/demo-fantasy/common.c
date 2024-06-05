
#define GAME_NAME "Demo Fantasy"

inherit "/games/hexagon/common.c";

void setup()
{
  set_game_name(slugify(GAME_NAME));
  set_light(100);
  set_short("Common room of " + GAME_NAME);
  set_long("Large relaxing chairs addorn the room. The walls are covered " + 
           "with strange motifs from different lands and the little shelf " + 
           "above the fireplace is chocker block full of strange figurines " + 
           "and bits of cloth. It appears to be the common room of %^BOLD%^" +
           GAME_NAME + "%^RESET%^.\n");

  add_item(({ "chair", "chairs" }), "The chairs are lazing around the room relaxing it looks " + 
                    "like they come here after a hard days working in " +
                    GAME_NAME + ".\n");
  add_item("fireplace", "A nice little fire place with a cheery fire burning " + 
                        "in it keeping every one warm.\n");
  add_item(({ "figurine", "figurines" }), "Small figurines, they look strangely familiar " + 
                         "until you realise they are the members of " +
                         GAME_NAME + ".\n");
  add_item(({ "cloth", "clothes" }), "Strange coloured bits of cloth strewn over the " + 
                    "mantlepice for no readily apparent reason.\n");
  add_item(({ "motif", "motifs" }), "The motifs on close inspection look like stylised " + 
                     "signatures of all the members of " + GAME_NAME +
                     ". Some of them are very strange, in fact there " + 
                     "seem to be more than there are members of the house. " + 
                     "perhaps it is the members of the future.\n");
  add_item("shelf", "A nice normal sort of shelf thing. It is like all " + 
                    "those ones you see in houses all over the place, " + 
                    "execpt... the way it has been burnt... hmm it does " + 
                    "not look like the fire did it.\n");
  add_item("fire", "There is a fire happily burning away in the fireplace " + 
                   "spluttering and crackling to itself. The flames almost " + 
                   "seem afraid of something.\n");  
}
