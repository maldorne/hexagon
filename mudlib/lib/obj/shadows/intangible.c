// Intangible shadow. Cloned onto a living that no blow can reach: whoever
// attacks it is told why and stops fighting it, and its hit points never go
// down.

private object body;
private string refusal;

/**
 * Puts the shadow over a living.
 *
 * @param  ob       the living that cannot be touched
 * @param  message  what an attacker is told
 */
void setup_shadow(object ob, string message)
{
  body = ob;
  refusal = message;
  shadow(ob, 1);
}

void attack_by(object ob)
{
  if (!ob)
    return;

  if (strlen(refusal))
    tell_object(ob, refusal);
  ob->stop_fight(body, 1);
}

int adjust_hp(int i, varargs object hp_remover)
{
  return body->query_hp();
}
