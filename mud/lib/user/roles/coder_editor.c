
static int ed_setup;

void create()
{
}

int query_ed_setup() { return ed_setup; }
void set_ed_setup(int i) { ed_setup = i; }

mixed stats() 
{
  return ({ 
            ({"Ed setup", ed_setup, }),
          });
}
            
