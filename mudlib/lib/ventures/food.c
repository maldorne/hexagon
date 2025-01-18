
#include <living/food.h>

int get_min_per_heal(string type, int volume) 
{
  int min_per_heal;

  min_per_heal = NORMAL_WCOST;

  if (type == ALCOHOL)
    min_per_heal = (1000 - volume) * min_per_heal / 1000;
  else if (type == DRINK) 
    min_per_heal = (1000 + volume) * min_per_heal / 1000;

  if (min_per_heal < 1) 
    min_per_heal = 1;
  return min_per_heal;
}
