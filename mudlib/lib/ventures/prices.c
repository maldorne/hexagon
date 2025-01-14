
#include <room/ventures.h>

/* 
 * This determines how much the shopkeeper will offer for an item, according
 * to its value.  It only gets called if set_no_resell() has not been called
 * and resale_value has not been hand-set to something.
 */
int scaled_value(int n)
{
  int *prates;
  int i, tot;

  tot = 0;
  prates = PAY_RATES;
  for (i = 0; ( (i < sizeof(prates)) && (n > prates[i]) ); i+=2);

  if (i > 0)
    i -= 2;
  // prates[i] is now the rate-increment directly below the value.

  tot = (n - prates[i]) * ((prates[i+2] / prates[i+3]) -
    (prates[i] / prates[i+1]));
  tot /= (prates[i+2] - prates[i]);
  tot += (prates[i] / prates[i+1]);

  // For those curious, we just defined a line segment.  Basically, we
  // used  y - y1 = m(x - x1).  And found the y value for x = n.
  // Read ventures.h for better explanation.
  return tot;
}

int do_cha_adjust(int amt, int cha, int updown)
{
  int temp;

  if (!cha) 
    return amt;
  if (cha > 19) 
    cha = 19;

  if (cha > AVG_CHA)
  {
    temp = 10 * (cha - AVG_CHA) * CHA_AD / (18 - AVG_CHA);
    if (updown) 
    {
      temp = amt * (1000 + temp) / 1000 - 1;
      if (temp < amt) 
        return amt;
    }
    else 
    {
      temp = amt * 1000 / (1000 + temp) + 1;
      if (temp > amt) 
        return amt;
    }
    return temp;
  }

  if (cha < AVG_CHA)
  {
    temp = 10 * (AVG_CHA - cha) * CHA_AD / (AVG_CHA - 3);
    if (updown)
    {
      temp = amt * 1000 / (1000 + temp) + 1;
      if (temp > amt) 
        return amt;
    }
    else
    {
      temp = amt * (1000 + temp) / 1000 - 1;
      if (temp < amt) 
        return amt;
    }

    return temp;
  }

  return amt;
}
