
static nomask object * previous_objects()
{
  object prev;
  object * result;
  int step;

  result = ({ });
  step = 0;

  prev = previous_object(step);

  while (prev != nil)
  {
    result += ({ prev });
    step++;
    prev = previous_object(step);
  }

  return result;
}
