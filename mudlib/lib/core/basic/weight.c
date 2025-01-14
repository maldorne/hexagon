
// weight inheritance, neverbot 4/03

static int weight;

void create()
{
	weight = 0;
}

// fixed these so there are no negative weights... Anirudh
void adjust_weight(int w) 
{
  if (weight+w < 0)
    w = -weight;
  if (environment())
    environment()->add_weight(w);
  weight += w;
}

void set_weight(int w) 
{
  if (w < 0)
    w = 0;
  if (environment())
    environment()->add_weight(w-weight);
  weight = w;
}

int query_weight() 
{
  if (weight < 0)
    weight = 0;
  return weight;
}

mixed * stats()
{
	return ({
		({"Weight (nosave)", weight, }),
	});
}