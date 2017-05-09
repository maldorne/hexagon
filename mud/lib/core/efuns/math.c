
/* Math Utility Functions */

// extracted from various places to make them common, neverbot 03/09
// new functions
// 	absolute value: abs
// 	class point, class fpoint
//    distance between points: distance, fdistance

// TODO class point and distance
// look here http://lpmuds.net/smf/index.php?topic=942.0

// class point
// {
// 	int x;
// 	int y;
// }

// class fpoint
// {
// 	float x;
// 	float y;
// }

// int distance(class point a, class point b)
// {
// 	float res = sqrt(pow(b->x - a->x, 2) + pow(b->y - a->y, 2));
// 	int res2 = res;
// 	return res2;
// }

// float fdistance(class fpoint a, class fpoint b)
// {
// 	float res = pow(b->x - a->x, 2) + pow(b->y - a->y, 2);
// 	return sqrt(res);
// }

static nomask int random(int low, varargs int max)
{
  if (!max)
  {
    if (low == 0)
      return 0; // changed in dgd v1.6
    return ::random(low);
  }
  
  if (low == max) 
    return low;
  if (low > max) 
    return 0;
  
  return ::random(max-low)+low+1;
}

static nomask int abs(int value)
{
  if (value < 0)
    return -value;
  return value;
}

/* integer exponential function added by dank Feb 23, 93 */
/*
int exp(int x, int y) {
  int i, ret;
  if (y < 0) {
    notify_fail("exp(x,y) doce y<0 es una fraccion; no se permiten fracciones!!\n");
    return 0;
  }
  ret = 1;
  for (i=0; i<y; i++)
    ret *= x;
  return ret;
}
*/

// it would be best to use the roll(int) thingie. 
// less processor intesive, but the string looks better
// Made by Sojan, put in Simul_efun by Baldrick.

// this is a dice function - rolls a dice and gives a result ... 
// can be a simul_efun
// or put wherever it can be accessed 
 
static nomask int roll(int times, int type)
{
  int i,retval;
  retval = 0;

  if (!times || !type) 
    return 0;
  
  for (i = 0; i < times; i++)
  {
    retval += (random(type) + 1);
  }

  return retval;
}

/* input to above function is of form roll(10, 20) if you wanted to roll 
   10 20 sided dice */

// compatibility with old mudos
/*
static nomask string oldcrypt(string str, varargs string salt)
{
  return ::crypt(str, salt);
}
*/

