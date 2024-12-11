
// intersection
// returns elements found in both arrays

static mixed * intersection(mixed * arr_a, mixed * arr_b)
{
  int i, j;
  mixed * result;

  result = ({ });

  if (!sizeof(arr_a) || !sizeof(arr_b))
    return result;

  for (i = 0; i < sizeof(arr_a); i++)
  {
    for (j = 0; j < sizeof(arr_b); j++)
    {
      if (arr_a[i] == arr_b[j])
      {
        result += ({ arr_a[i] });
        break;
      }
    }
  }

  return result;
}
