

static nomask string mud_name() 
{
  return handler("config")->config_mud_name(); 
}

static nomask string version() 
{ 
  return status()[0];
}

