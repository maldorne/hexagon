// soul_data.c Translated for RlMud by Elannon (1999)

// Removed most of the souls in spanish, fixed some mistakes,
// removed a lot of parameters, neverbot 02/2006

// Recovered the original soul data in english from frmudlib,
// both languages moved to translations/souls for hexagon
//     neverbot 01/2025

#include <translations/soul-data.h>

string query_global_adj()
{
  return GLOBAL_ADJ;
}

mapping query_soul_data()
{
  return SOUL_DATA_A + SOUL_DATA_B + SOUL_DATA_C +
          SOUL_DATA_D + SOUL_DATA_E + SOUL_DATA_F +
          SOUL_DATA_G + SOUL_DATA_H + SOUL_DATA_I +
          SOUL_DATA_J + SOUL_DATA_K + SOUL_DATA_L +
          SOUL_DATA_M + SOUL_DATA_N + SOUL_DATA_O +
          SOUL_DATA_P + SOUL_DATA_Q + SOUL_DATA_R +
          SOUL_DATA_S + SOUL_DATA_T + SOUL_DATA_U +
          SOUL_DATA_V + SOUL_DATA_W + SOUL_DATA_X +
          SOUL_DATA_Y + SOUL_DATA_Z;
}
