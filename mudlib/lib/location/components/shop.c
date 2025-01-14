inherit component "/lib/location/component.c";

inherit sign      "/lib/ventures/shop-sign.c";


void create()
{
  component::create();
  set_type("shop");
}

// will be called after creation + init_auto_load_attributes,
// providing de parent location object
void initialize(object location)
{
  object sign;

  component::initialize(location);

  sign = create_sign(location);
  sign->move(location);
}