
object my_player;
static object corpse; // Para tener informacion a la hora de resucitar

object query_my_corpse() { return corpse; }
void set_my_corpse(object body) { corpse = body; }


void create()
{
  corpse = nil;
}

// int test_add() { return 0; }

void attack() {}
void adjust_hp() {}
void set_hp() {}
void do_death() { /* He`s allready dead...  sheeze.  */ }
 
void setup_shadow(object ob) 
{
  shadow(ob,1);
  my_player = ob;
}

int attack_by(object ob) 
{
  tell_object(ob, "Esta persona es un espíritu, no puedes hacerle nada.\n");
  ob->stop_fight(my_player);
  return 1;
}

int attack_ob(object ob) 
{
  tell_object(my_player, "Tu mano no tiene efecto en el mundo material, ¡eres un espíritu!\n");
  return 0;
}

string short() 
{
  if (!my_player)
    return "Sombra de muerte";
  return "Espíritu de "+my_player->short();
}

string query_short() 
{
  if ( !my_player)
    return "Sombra de muerte";
  return "Espíritu de "+my_player->query_short();
}

void dest_death_shadow() 
{
  destruct(this_object());
}

int cast() 
{
  tell_object(my_player, "¿Piensas formular hechizos siendo un espíritu?\n");
  return 1;
}

int do_shout() 
{
  tell_object(my_player, "Eres un espíritu, eso no es posible.\n");
  return 1;
}

int do_echo() 
{
  tell_object(my_player, "Eres un espíritu, eso no es posible.\n");
  // tell_object(my_player, "You are dead...\n");
  return 1;
}

int do_guild_command() 
{
  tell_object(my_player, "Eres un espíritu, eso no es posible.\n");
  // tell_object(my_player, "You are dead!\n");
  return 1;
}

int do_race_command() 
{
  tell_object(my_player, "Eres un espíritu, eso no es posible.\n");
  // tell_object(my_player, "You are dead!\n");
  return 1;
}

/*
int score()
{
  tell_object(my_player, "You are just a disembodied spirit. What use has a wispy thing like " +
    "you have for a collection of meaningless numbers?\n");
  return 1;
}
*/
 
void second_life() 
{
} 

