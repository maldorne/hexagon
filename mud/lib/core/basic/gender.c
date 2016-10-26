/* 
 * Retocado por neverbot@Cc, Abril 04
 *
 * Dejado gender 0 unicamente para los objetos (y unicamente cuando no haya
 *  sido definido su genero).
 */

int gender;

// Añadidos create y stats, neverbot 4/03
void create()
{
  gender = 1;
}

int query_gender() { return gender; }

int query_female() { return gender == 2; }
int query_male() { return gender == 1; }

/* The next function is soley for compatibility with old code. */
/* It should not be used in new source files. */
int query_neuter() { return !gender; }

string query_gender_string() 
{
  switch (gender) 
  {
    // case 0: return "asexual";
    case 1: return "varón";
    case 2: return "mujer";
    // case 3: return "bisexual";
    default: return "varón";
  }
}

string query_gender_title() 
{
  switch (gender) 
  {
    case 1: return "señor";
    case 2: return "señora";
    default: return "ente";
  }
}
 
string query_objective() 
{
  switch (gender) 
  {
    case 1: return "el";
    case 2: return "ella";
    default: return "el";
  }
}
 
string query_possessive() 
{
  return "su";
  /*
  switch (gender) {
    case 1: return "su";
    case 2: return "su";
    default: return "su";
  }
  */
}
 
string query_pronoun() 
{
  switch (gender) 
  {
    case 1: return "él";
    case 2: return "ella";
    default: return "él";
  }
}

// Bugfix by Aragorn 22/1/94
// Cambiado por neverbot, solo puede recibir int.
void set_gender(int arg) 
{
  // Añadido para el genero de los objetos (espero que no
  // se use en mas sitios!!)
  if (arg == 0)
  {
    // if (interactive(this_object())){
    if (living(this_object()))
    {
      write("No se puede hacer eso (género ilegal).\n");
      return;
    }
      
    gender = 0;
    return;
  }

  if (arg < 1)
    gender = 1;
  else if (arg > 2)
    gender = 2;
  else
    gender = arg;
}

string query_article()
{
  if (gender == 2) return "la";
  return "el";
}

string query_numeral()
{
   if (gender == 1) return "un";
   return "una";
}

string query_vocal()
{
   if (gender == 1) return "o";
   return "a";
} 

// neverbot, 7/03
string query_demonstrative()
{
   if (gender == 1) return "este";
   return "esta";
}

mixed stats()
{
  return ({ ({"Gender", gender, }), });
}
