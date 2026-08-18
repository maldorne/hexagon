// Generic message personalization.
//
// Substitute pronoun / name / gender-agreement tokens in a template against an
// actor (and an optional target), so one authored line reads correctly for any
// gender instead of a fixed, possibly mis-gendered string. Same token vocabulary
// the social commands use, but a plain string utility: no dependency on the
// souls handler, callable anywhere a living can be named.
//
//   actor tokens:  $mcname$ $mname$ $mpronoun$ $mobj$ $mposs$ $mvocal$ $mnumeral$
//   target tokens: $hcname$ $hname$ $hpronoun$ $hobj$ $hposs$ $hvocal$ $hnumeral$
//   $lastarg$              the caller's `arg`, verbatim
//   $ifarg:A$else$B~$      A when arg is non-empty, else B (either half optional)
//
// With no target the $h*$ tokens are left untouched (there is nobody to resolve
// them against). Any other unknown $...$ token is likewise left intact, so a
// caller layering its own tokens on top (e.g. the souls handler's $force#/$arg:)
// can post-process the result.

static nomask string personalize_string(string template, object actor,
                                        varargs object target, string arg)
{
  string s1, s2, s3, s4, str;

  if (!template || !actor)
    return template;
  if (!arg)
    arg = "";

  str = template;
  s4 = "";

  // pronoun / name / agreement pass. On a token we do not handle (an unknown
  // one, or an $h*$ with no target), fold it -- delimiters and all -- into s4
  // and put the closing $ back at the front of the remainder, so a caller's own
  // $token$ survives the pass intact.
  while (sscanf(str, "%s$%s$%s", s1, s2, s3) == 3)
    switch (s2)
    {
    case "mcname":   str = s1 + actor->query_cap_name() + s3;   break;
    case "mname":    str = s1 + actor->query_name() + s3;       break;
    case "mpronoun": str = s1 + actor->query_pronoun() + s3;    break;
    case "mobj":     str = s1 + actor->query_objective() + s3;  break;
    case "mposs":    str = s1 + actor->query_possessive() + s3; break;
    case "mvocal":   str = s1 + actor->query_vowel() + s3;      break;
    case "mnumeral": str = s1 + actor->query_numeral() + s3;    break;
    case "lastarg":  str = s1 + arg + s3;                       break;

    // target tokens only resolve when a target was given; otherwise they fall
    // through (past each other) to the default and pass through untouched
    case "hcname":   if (target) { str = s1 + target->query_cap_name() + s3;   break; }
    case "hname":    if (target) { str = s1 + target->query_name() + s3;       break; }
    case "hpronoun": if (target) { str = s1 + target->query_pronoun() + s3;    break; }
    case "hobj":     if (target) { str = s1 + target->query_objective() + s3;  break; }
    case "hposs":    if (target) { str = s1 + target->query_possessive() + s3; break; }
    case "hvocal":   if (target) { str = s1 + target->query_vowel() + s3;      break; }
    case "hnumeral": if (target) { str = s1 + target->query_numeral() + s3;    break; }

    default:
      s4 += s1 + "$" + s2;
      str = s3 ? "$" + s3 : "$";
      break;
    }

  str = s4 + str;

  // $ifarg:A$else$B~$ -- A when the caller passed an arg, else B; either half
  // may be omitted. Each branch is personalized in turn.
  while (sscanf(str, "%s$ifarg:%s~$%s", s1, s2, s3) == 3)
  {
    string estr;

    estr = nil;
    sscanf(s2, "%s$else$%s", s2, estr);
    if (arg && arg != "")
      str = s1 + personalize_string(s2, actor, target, arg) + s3;
    else if (estr)
      str = s1 + personalize_string(estr, actor, target, arg) + s3;
    else
      str = s1 + s3;
  }

  return str;
}
