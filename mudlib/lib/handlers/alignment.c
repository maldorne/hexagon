
// new alignment system, neverbot 6/03

// returns if two livings are or aren't enemies
// passing their numeric alignments as parameters
int query_enemies(int attacker, int defender){

  if (attacker == defender)
    return 0;

  // Ambos buenos y/o neutrales
  if ((defender > 0) && (attacker > 0))
    return 0;

  // Ambos malos
  if ((defender < 0) && (attacker < 0))
    return 0;


  // Atacante malo y defensor bueno:
  if ((attacker < 0) && (defender > 100)){
    // Se llevan mas de mil puntos de alineamiento de diferencia
    if (defender - attacker > 1000)
      return 1;
  }

  // Atacante bueno y defensor malo:
  if ((attacker > 100) && (defender < 0)){
    // Se llevan mas de mil puntos de alineamiento de diferencia
    if (attacker - defender > 1000)
      return 1;
   }

  return 0;
}
