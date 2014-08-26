
/* Math Utility Functions */

// Extraidas de varios lugares para hacerlas comunes, Folken 03/09
// Funciones nuevas 
// 	valor absoluto: abs
// 	representacion de puntos: class point, class fpoint
//    distancia entre puntos: distance, fdistance

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

int random(int low, varargs int max)
{
  if (!max) 
    return ::random(low);
  if (low == max) 
    return low;
  if (low > max) 
    return 0;
  
  return ::random(max-low)+low+1;
}

int rand_num(int low, int max)
{
	return random(low, max);
}

int abs(int value)
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


/* it would be best to use the roll(int) thingie. 
 * less processor intesive, but the string looks better */
/* Made by Sojan, put in Simul_efun by Baldrick. */

/* this is a dice function - rolls a dice and gives a result ... 
 * can be a simul_efun
 * or put wherever it can be accessed 
 */

int roll(int times, int type)
{
  int i,retval;
  retval=0;
  if(!times||!type) return 0;
  for(i=0;i<times;i++)
  {
    retval+=(random(type)+1);
  }
  return retval;
} /* roll */

/* input to above function is of form roll(10, 20) if you wanted to roll 
   10 20 sided dice */
