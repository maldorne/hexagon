

int monitor;

void create()
{
  monitor = 1;
}

void set_monitor(int m) { monitor = m; }
int query_monitor() { return monitor; }

// Cambiamos la funcion para mostrar la vida igual que con el
// score (colores), neverbot 12/05

void display_monitor(int use_colors) 
{
  string COLOR, RESET, color2, color3;

  COLOR = "%^GREEN%^";

  if (use_colors)
  {
    RESET = "%^RESET%^GREEN%^";
    color2 = "";
    color3 = "";

    if (!monitor)
      return;

    if ( this_object()->query_hp() >= (this_object()->query_max_hp()/2) )
      color2 += "%^GREEN%^";
    else if ( this_object()->query_hp() >= (this_object()->query_max_hp()/4) )
      color2 += "%^BOLD%^MAGENTA%^";
    else
      color2 += "%^BOLD%^RED%^";  

    if ( this_object()->query_hp() >= (this_object()->query_max_gp()/2) )
      color3 += "%^GREEN%^";
    else if ( this_object()->query_hp() >= (this_object()->query_max_gp()/4) )
      color3 += "%^BOLD%^MAGENTA%^";
    else
      color3 += "%^BOLD%^RED%^";  
    
    write(COLOR + 
      "Pv: " + color2 + this_object()->query_hp() + RESET + 
      " (" + this_object()->query_max_hp() + RESET + 
      ") Pe: " + color3 + this_object()->query_hp() + RESET + 
      " (" + this_object()->query_max_gp() + RESET + 
      ")%^RESET%^\n");
  }
  else
  {
    tell_object(this_object(), COLOR + sprintf("Pv: %d (%d) Pe: %d (%d)\n", 
                                              this_object()->query_hp(), 
                                              this_object()->query_max_hp(), 
                                              this_object()->query_hp(), 
                                              this_object()->query_max_gp()) + "%^RESET%^\n");
  }
}

mixed * stats() 
{
  return ({ });
}
