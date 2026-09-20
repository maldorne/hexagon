
#define _LANG_BASEOBS_ROPE "rope"
#define _LANG_BASEOBS_ROPE_LONG "A sturdy rope, long enough to hold your weight " + \
      "if you use it to climb, and useful for tying anything up.\n"

#define _LANG_BASEOBS_TORCH "torch"
#define _LANG_BASEOBS_TORCH_LONG "A small torch to light the way, little more than a stick " + \
      "of old wood with strips of cloth wound around one end. Use \"light torch\" and " + \
      "\"extinguish torch\" to work it.\n"

#define _LANG_BASEOBS_TORCH_AS_NEW "It is as good as new.\n"
#define _LANG_BASEOBS_TORCH_BURNT_OUT "It is burnt out.\n"
#define _LANG_BASEOBS_TORCH_LEFT(percentage) ("It has " + percentage + "% of its use left.\n")
#define _LANG_BASEOBS_TORCH_LIT " (%^YELLOW%^lit%^RESET%^)"
#define _LANG_BASEOBS_TORCH_LIT_PLURAL " (%^YELLOW%^lit%^RESET%^)"

#define _LANG_BASEOBS_TORCH_VERBS_LIGHT ({ "light" })
#define _LANG_BASEOBS_TORCH_VERBS_EXTINGUISH ({ "extinguish", "quench" })

#define _LANG_BASEOBS_TORCH_WHAT (capitalize(query_verb()) + " what?\n")
#define _LANG_BASEOBS_TORCH_HOLD_FIRST "You have to hold the torch first.\n"
#define _LANG_BASEOBS_TORCH_ALREADY_LIT "The torch is already lit.\n"
#define _LANG_BASEOBS_TORCH_ALREADY_OUT "The torch is not lit.\n"

#define _LANG_BASEOBS_TORCH_YOU_LIGHT "%^YELLOW%^You light the torch.%^RESET%^\n"
#define _LANG_BASEOBS_TORCH_SOMEBODY_LIGHTS(who) ("%^YELLOW%^" + who->query_cap_name() + \
      " lights a torch.%^RESET%^\n")
#define _LANG_BASEOBS_TORCH_YOU_EXTINGUISH "You put the torch out.\n"
#define _LANG_BASEOBS_TORCH_SOMEBODY_EXTINGUISHES(who) (who->query_cap_name() + \
      " puts a torch out.\n")
#define _LANG_BASEOBS_TORCH_GOES_OUT "Your torch goes out.\n"
#define _LANG_BASEOBS_TORCH_SOMEBODYS_GOES_OUT(who) ("The torch of " + who->query_cap_name() + \
      " goes out.\n")
