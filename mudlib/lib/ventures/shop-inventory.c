
// items the shop _must_ have
static mapping permanent_goods;

// prototypes
// backwards compatibility
int add_thing(string file, int amount);
int add_permanent_goods(string file, int amount);
mapping query_permanent_goods(){ return permanent_goods; }

void create() 
{
  permanent_goods = ([ ]);
}

// backwards compatibility
int add_thing(string file, int amount) { return add_permanent_goods(file, amount); }

//  with the new shop system, the items bought and sold are _always_ saved, so
//  we need a way to restock some basic objects if we want them to always be
//  available
int add_permanent_goods(string file, int amount)
{
  object ob;
  ob = load_object(file);
  
  if (!ob)
  {
    debug("shops", " invalid add_permanent_goods in " + file_name(this_object()) + " : " + file + "\n");
  
    // create new issue, same message as npcs equipment
    if (package("issues"))
      package("issues")->add_issue("Cannot clone " + file, this_object());
  
    return 0;
  }
  
  permanent_goods[base_name(ob)] = amount;
}

mixed * stats()
{
  return ({
    ({ "Permanent Goods", permanent_goods }),
  });
}