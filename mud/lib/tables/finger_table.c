// Table to translate domain and area names to something more readable
// neverbot@CcMud 01/10


string get_nice_name(string domain)
{
  switch(domain)
  {
    case "ciudad_capital":
      return "Ciudad Capital";
  }

  return capitalize(domain);
}


string get_domain_name(string what)
{
  switch(what)
  {
    case "Ciudad Capital":
    case "ciudad capital":
    case "Ciudad capital":
      return "ciudad_capital";
  }

  return what;  
}
