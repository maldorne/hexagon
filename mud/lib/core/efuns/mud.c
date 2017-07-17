

static nomask string mud_name() 
{
  return handler("config")->config_mud_name(); 
}

static nomask string version() 
{ 
  return handler("config")->config_driver_version(); 
}
