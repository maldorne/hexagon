/* Excluding this from the soul.c, just to make things a little cleaner */
/* Wonderflug, 1996 */

#include <translations/souls.h>

#define GLOBAL_ADJ ",slowly,happily,sadly,nervously,wryly,broadly,crookedly," + \
  "stupidly,idiotically,condescendingly,ironically,patiently,brightly," + \
  "slightly,nastily,excitedly,strangely,with forced patience,plastically," + \
  "carefully,sadistically,lopsidedly,fishily,catishly,faintly," + \
  "like a duck,angrily,paranoicaly,innocently,frogily,slyly," + \
  "weakly,humbly,paradoxically,sarcastically,viciously,garishly," + \
  "sweetly,innocently,lovingly,dreamily,radiantly,enthusiastically," + \
  "insolently,quirkily,completely,unconsciously,drunkenly," + \
  "insanely,cutely,maliciously,maybe,fleetingly," + \
  "absentmindedly,curtly,disturbingly,unbelievably,quietly," + \
  "loudly,differently,winningly,in triumph,seductively," + \
  "tightly,because of all the arguments,softly,positively," + \
  "lividly,demonically,warmly,coldly,drunkenly,negatively," + \
  "maniacally,warily,sardonically,lazily,serenely,disappointedly"

// old ping
/* "$mcname$ te hace '¡¡¡PING!!!'$force#alias ping_alias $mname$#2$", */   

#define SOUL_DATA_A ([  \
  SOUL_CMD_ACCUSE : ({ ({ 0, "%s", 2, "%s of %s", 2, "%s %s", 3, "%s" }), ({ \
      "$ifarg:You accuse $arg:#$$else$The butler did it!~$",  \
      "$mcname$ $ifarg:accuses $lastarg$$else$accuses the butler~$", \
      "You accuse $hcname$$ifarg: of $arg:#$~$", \
      "$mcname$ accuses you$ifarg: of $lastarg$~$", \
      "$mcname$ accuses $hcname$$ifarg: of $lastarg$~$" }) }), \
  SOUL_CMD_ACK : ({ ({ 0, "%s", 0, "at %s" }), \
    ({ "You ack", \
      "$mcname$ ack's", \
      "You ack at $hcname$", \
      "$mcname$ ack's at you", \
      "$mcname$ ack's at $hcname$", }) }), \
  SOUL_CMD_ADMIRE : ({ ({ 0, "%s", 2, "%s %s", 3, "%s" }), ({ \
      "$ifarg:You admire $arg:#$$else$You admire this wonderful MUD!~$", \
      "$mcname$ $ifarg:admires $lastarg$$else$admires this wonderful MUD~$", \
      "You admire $hcname$$ifarg: for being $arg:#$~$", \
      "$mcname$ admires you$ifarg: for being $lastarg$~$", \
      "$mcname$ admires $hcname$$ifarg: for being $lastarg$~$" }) }), \
  SOUL_CMD_APPLAUDE : ({ \
    ({ 0, "%s", 1, "%s %s", 2, "%s %s", 3, "%s" }), ({ \
      "You applaud $arg:wholeheartedly,loudly,softly,vaguely,briefly$", \
      "$mcname$ applauds $lastarg$", \
      "You applaud $hcname$ $arg:wholeheartedly,loudly,softly,vaguely,briefly$", \
      "$mcname$ applauds you $lastarg$", \
      "$mcname$ applauds $hcname$ $lastarg$" }) }), \
  SOUL_CMD_AMAZE : ({ 0, ({ \
      "You amaze everybody with your existence", \
      "Isn't $mcname$ amazing?" }) }), \
  SOUL_CMD_AGREE : ({ ({ 0, "%s", 0, "with %s", 1, "%s with %s", 1, "%s %s", \
      2, "with %s %s", 3, "%s" }), ({ \
      "You agree $arg:wholeheartedly,halfheartedly,partially,wearily,"+ \
      "to some degree$", \
      "$mcname$ agrees $lastarg$", \
      "You agree $arg:wholeheartedly,halfheartedly,partially,wearily,"+ \
      "to some degree$ with $hcname$", \
      "$mcname$ agrees $lastarg$ with you", \
      "$mcname$ agrees $lastarg$ with $hcname$" }) }), \
  SOUL_CMD_APOLOGIZE : ({ \
    ({ 3, "%s", 1, "%s to %s", 1, "%s %s", 2, "%s %s", }), ({ \
      "You apologize $arg:profusely,happily,sadly,reservedly,"+ \
      "carefully,slowly,perfunctorily,curtly,viciously,"+ \
      "sulkily,drunkenly,unhappily,professionally,pompously,"+ \
      "wholeheartedly,partially,wearily,fishily,facetiously,"+ \
      "unnecessarily,spontaneously,twice,thoroughly,humbly$", \
      "$mcname$ apologizes $lastarg$", \
      "You apologize $arg:profusely,happily,sadly,reservedly,"+ \
      "carefully,slowly,perfunctorily,curtly,viciously,"+ \
      "sulkily,drunkenly,unhappily,professionally,pompously,"+ \
      "wholeheartedly,partially,wearily,fishily,facetiously,"+ \
      "unnecessarily,spontaneously,twice,thoroughly,humbly$"+ \
      " to $hcname$", \
      "$mcname$ apologizes $lastarg$ to you", \
      "$mcname$ apologizes $lastarg$ to $hcname$" }) }), \
  SOUL_CMD_ACKNOWLEDGE : ({ ({ 0, "%s" }), ({ \
      "You acknowledge $hcname$", \
      "$mcname$ acknowledges you", \
      "$mcname$ acknowledges $hcname$" }) }), \
  SOUL_CMD_ANNOY : ({ ({ 0, "%s", }), ({ \
      "You annoy $hcname$ to death", \
      "$mcname$ annoys you to death", \
      "$mcname$ annoys $hcname$ to death", }) }), \
])

#define SOUL_DATA_B ([  \
  SOUL_CMD_BURP : ({ ({ 3, "%s" }), ({ \
      "You burp$ifarg: $arg:disgustingly,faintly,loudly,rudely$~$", \
      "$mcname$ burps$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_BARK : ({ ({ 3, "%s" }), ({ \
      "You bark like a $arg:dog,cat,tree,duck,wolf,hunting dog,labrador,"+ \
      "german shepard,something else that barks,"+ \
      "bull terrier,train$", \
      "$mcname$ barks like a $lastarg$", }) }), \
  SOUL_CMD_BLINK : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "You blink$ifarg: $arg:owlishly,slowly,carefully$~$", \
      "$mcname$ blinks$ifarg: $lastarg$~$", \
      "You blink at $hcname$", \
      "$mcname$ blinks at you", \
      "$mcname$ blinks at $hcname$" }) }), \
  SOUL_CMD_BREATHE : ({ ({ 3, "%s", }), ({ \
      "You breathe $arg:quickly,seductively,painfully,nastily,slowly,once,"+ \
      "heavily,carefully,heatedly$", \
      "You breathe $lastarg$", }) }), \
  SOUL_CMD_BOW : ({ ({ 0, "%s", 0, "to %s", 1, "%s to %s", 1, "%s %s", 3, "%s" }), ({ \
      "You bow$ifarg: $arg:solemnly,deeply,formally,hastily,slightly,"+ \
      "respectfully,"+ \
      "insolently,clumsily,gracefully,dexterously,wildly,colorfully$~$", \
      "$mcname$ bows$ifarg: $lastarg$~$", \
      "You bow$ifarg: $arg:solemnly,deeply,formally,hastily,slightly,"+ \
      "gracefully,respectfully,"+ \
      "insolently,clumsily,dexterously,wildly,colorfully$~$ to $hcname$", \
      "$mcname$ bows$ifarg: $lastarg$~$ to you", \
      "$mcname$ bows$ifarg: $lastarg$~$ to $hcname$" }) }), \
  SOUL_CMD_BLUSH : ({ ({ 3, "%s", 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You blush$ifarg: $arg:in embarrassment,redly,slightly,"+ \
      "worriedly$~$", \
      "$mcname$ blushes$ifarg: $lastarg$~$", \
      "You blush$ifarg: $arg:in embarrassment,redly,slightly,"+ \
      "worriedly$~$ at $hcname$", \
      "$mcname$ blushes$ifarg: $lastarg$~$ at you", \
      "$mcname$ blushes$ifarg: $lastarg$~$ at you" }) }), \
])

#define SOUL_DATA_C ([  \
  SOUL_CMD_CUDDLE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You cuddle $hcname$", \
      "$mcname$ cuddles you", \
      "$mcname$ cuddles $hcname$" }) }), \
  SOUL_CMD_COMFORT : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You comfort $hcname$$ifarg: $arg:sincerely,wholeheartly,mischievously,"+ \
      "halfheartedly,vaguely,eagerly,completely,lovingly,insincerely,gently,"+ \
      "tenderly,sadly,in earnest,with your wings,in the ear,in the leg,"+ \
      "twice,anxiously,carefully,delicately,like someone else$~$", \
      "$mcname$ comforts you$ifarg: $lastarg$~$", \
      "$mcname$ comforts $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_CONFESS : ({ ({ 3, "%s" }), ({ \
      "I confess $ifarg:$arg:#$$else$to being a witch~$", \
      "$mcname$ confess's $ifarg:$lastarg$$else$to being a witch, burn "+ \
      "$mposs$~$", }), }), \
  SOUL_CMD_CROSS : ({ ({ 3, "%s", 3, "my %s" }), ({ \
      "You cross your $arg:fingers,legs,eyes$", \
      "$mcname$ crosses $mposs$ $lastarg$", }) }), \
  SOUL_CMD_CRY : ({ 0, ({ "Wahhhhhh", "$mcname$ crys loudly" }) }), \
  SOUL_CMD_COUGH : ({ 0, ({ "You cough", "$mcname$ coughs" }) }), \
])

#define SOUL_DATA_D ([  \
  SOUL_CMD_DANCE : ({ ({ 0, "%s", 0, "with %s" }), ({ \
      "You do the disco duck.  Feels great doesn't it ?", \
      "$mcname$ dances the disco duck", \
      "You sweep $hcname$ across the dance floor", \
      "$mcname$ sweeps you across the dance floor", \
      "$mcname$ sweeps $hcname$ across the dance floor" }) }), \
  SOUL_CMD_DROOL : ({ ({ 0, "%s", 0, "at %s" }), ({ \
      "You start to drool", \
      "$mcname$ starts to drool", \
      "You drool all over $hcname$", \
      "$mcname$ drools all over you", \
      "$mcname$ drools all over $hcname$" }) }), \
  SOUL_CMD_DENY : ({ ({ 0, "%s" }), ({ \
      "You deny it all", \
      "$mcname$ denies it all", \
      "You deny $hcname$'s every word", \
      "$mcname$ denies each and every of your words", \
      "$mcname$ denies $hcname$'s every word" }) }), \
])

#define SOUL_DATA_E ([ ])

#define SOUL_DATA_F ([  \
  SOUL_CMD_FLIRT : ({ ({ 0, "%s", 1, "%s with %s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You flirt$ifarg: $arg:seductively,shamelessly,badly,"+ \
      "adorably,winningly,disgustingly,daringly$~$ with $hcname$", \
      "$mcname$ flirts$ifarg: $lastarg$~$ with you", \
      "$mcname$ flirts$ifarg: $lastarg$~$ with $hcname$" }) }), \
  SOUL_CMD_FEAT : ({ ({ 0, "%s" }), ({ \
      "You scream at $hcname$ in fear", \
      "$mcname$ screams at you in fear", \
      "$mcname$ screams at $hcname$ in fear" }) }), \
  SOUL_CMD_FORGIVE : ({ ({ 0, "%s", }), ({ \
      "You forgive $hcname$", \
      "$mcname$ forgives you", \
      "$mcname$ forgives $hcname$", }), }), \
])

#define SOUL_DATA_G ([  \
  SOUL_CMD_GESTICULATE : ({ 0, ({ \
      "You gesticulate wildly", \
      "$mcname$ gesticulates wildly" }) }), \
  SOUL_CMD_GREET : ({ ({ 0, "%s" }), ({ \
      "You greet $hcname$ $arg:happily,sadly,with open arms,"+ \
      "ecstatically,carefully,courteously,hysterically,"+ \
      "reservedly,warmly,coldly,insolently,curtly,"+ \
      "absentmindedly$", \
      "$mcname$ greets you $lastarg$", \
      "$mcname$ greets $hcname$ $lastarg$", }) }), \
])

#define SOUL_DATA_H ([  \
  SOUL_CMD_HALLUCINATE : ({ ({ 0, "%s" }), ({ \
      "You hallucinate", \
      "$mcname$ hallucinates" }) }), \
  SOUL_CMD_HUG : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You hug $hcname$$ifarg: $arg:close,gently,warmly,"+ \
      "tightly,vigorously,desperately,lovingly,happily,like a bear$~$", \
      "$mcname$ hugs you$ifarg: $lastarg$~$", \
      "$mcname$ hugs $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_HOWL : ({ ({ 3, "%s" }), ({ \
      "You howl$ifarg: $arg:at the moon$~$: HOOOOOWWWWWWWWWLLLLLL!!", \
      "$mcname$ howls$ifarg: $arg:at the moon$~$" }) }), \
  SOUL_CMD_HICCUP : ({ 0, ({ \
      "You hiccup", \
      "$mcname$ hiccups" }) }), \
  SOUL_CMD_HUM : ({ ({ 3, "%s" }), ({ \
      "You hum $arg:a merry tune,tunelessly,annoyingly,vaguely,absentmindedly,"+ \
      "whilst you work,distractedly,pointlessly,silently,loudly,"+ \
      "rudely$", \
      "$mcname$ hums $lastarg$" }) }), \
])

#define SOUL_DATA_I ([ ]) 

#define SOUL_DATA_J ([ ]) 

#define SOUL_DATA_K ([ \
  SOUL_CMD_KISS : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You kiss $hcname$$ifarg: $arg:passionately,deeply,tenderly,gently,goodbye," + \
      "lovingly,lengthily,fishily,carefully,demandingly,sensuously,#$~$", \
      "$mcname$ kisses you$ifarg: $lastarg$~$", \
      "$mcname$ kisses $hcname$$ifarg: $lastarg$~$" }) }), \
])

#define SOUL_DATA_L ([ \
  SOUL_CMD_LICK : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You lick your lips", \
      "$mcname$ licks $mposs$ lips", \
      "You lick $hcname$$ifarg: $arg:softly,gently,professionally,"+ \
      "thoroughly$~$", \
      "$mcname$ licks you$ifarg: $lastarg$~$", \
      "$mcname$ licks $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_LAUGH : ({ ({ 0, "%s", 0, "at %s", 1, "%s %s", 1, "%s at %s", 2, "at %s %s", 3, "%s"}), ({ \
      "$ifarg:You laugh $arg:nastily,nicely,quietly,uncontrollably,politely,"+ \
      "happily,maniacally,ecstatically,demonically$"+ \
      "$else$You laugh~$", \
      "$ifarg:$mcname$ laughs $lastarg$$else$$mcname$ laughs~$", \
      "You laugh$ifarg: $arg:nastily,nicely,quietly,uncontrollably,politely,"+ \
      "happily,maniacally,ecstatically,demonically$~$ at $hcname$", \
      "$mcname$ laughs$ifarg: $lastarg$~$ at you", \
      "$mcname$ laughs$ifarg: $lastarg$~$ at $hcname$" }) }), \
  SOUL_CMD_LUCK : ({ ({ 0, "%s" }), ({ \
      "You wish $hcname$ good luck", \
      "$mcname$ wishes you good luck", \
      "$mcname$ wishes $hcname$ good luck" }) }), \
])

#define SOUL_DATA_M ([ \
  SOUL_CMD_MOAN : ({ ({ 3, "%s" }), ({ \
      "You moan$ifarg: $arg:gently,deeply,sensually,agonizingly,in terror$~$", \
      "$mcname$ moans$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_MEOW : ({ ({ 3, "%s", 3, "like a %s" }), ({ \
      "You meow like a $arg:cat,kitten,"+ \
      "cat in heat$", \
      "$mcname$ meows like a $lastarg$", }) }), \
])

#define SOUL_DATA_N ([ ])

#define SOUL_DATA_O ([ \
  SOUL_CMD_OATH : ({ ({ 0, "%s", }), ({ \
      "You swear your eternal love for $hcname$", \
      "$mcname$ swears $mposs$ eternal love for you", \
      "$mcname$ swears $mposs$ eternal love for $hcname$" }) }), \
])

#define SOUL_DATA_P ([ \
  SOUL_CMD_PAMPER : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You pamper $hcname$$ifarg: $arg:lovingly,with a lot of care,"+ \
      "with a lot of attention,with a lot of love,"+ \
      "with affection,with tenderness$~$", \
      "$mcname$ pampers you$ifarg: $lastarg$~$", \
      "$mcname$ pampers $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_PROTEST : ({ ({ 3, "%s" }), ({ \
      "You protest$ifarg: $arg:vehemently,loudly,strongly,weakly,"+ \
      "against duck-shooting,noisily,pointlessly,annoyingly,#$~$", \
      "$mcname$ protests$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_PING : ({ ({ 0, "%s" }), ({ \
      "You ping $hcname$", \
      "$mcname$ pings you$force#ack $mname$#2$", \
      "$mcname$ pings $hcname$" })  }), \
  SOUL_CMD_PONDER : ({ 0, ({ \
      "You ponder the situation", \
      "$mcname$ ponders the situation" }) }), \
  SOUL_CMD_PROMISE : ({ ({ 0, "%s" }), ({ \
      "You promise $hcname$", \
      "$mcname$ promises you", \
      "$mcname$ promises $hcname$" }) }), \
  SOUL_CMD_POINT : ({ ({ 0, "%s", 0, "at %s", 1, "%s at %s", 2, "at %s %s" }), ({ \
      "You point$ifarg: $arg:accusingly,straight$~$ at $hcname$", \
      "$mcname$ points$ifarg: $lastarg$~$ at you", \
      "$mcname$ points$ifarg: $lastarg$~$ at $hcname$" }) }), \
  SOUL_CMD_PUKE : ({ ({ 0, "on %s", 0, "%s", 0, "at %s"  }), ({ \
      "You puke a colourful rainbow of vomit", \
      "$mcname$ pukes a colourful rainbow of vomit", \
      "You puke on $hcname$", "$mcname$ pukes on you", \
      "$mcname$ pukes on $hcname$" }) }), \
])

#define SOUL_DATA_Q ([ ])

#define SOUL_DATA_R ([ \
  SOUL_CMD_ROLL : ({ ({ 2, "%s %s", 1, "%s %s", 3, "%s", 0, "%s" }), ({ \
      "You roll $arg:your eyes,around on the floor laughing,"+ \
      "over and play dead,up into a ball,your own cigarette,"+ \
      "sideways,backwards,professionally,like a spy through an "+ \
      "imaginary door,fearfully,tightly,slowly$", \
      "$mcname$ rolls $lastarg$", \
      "You roll $ifarg:$arg:quickly,slowly,painfully,excitededly,lovingly,"+ \
      "carefully$ ~$over $hcname$", \
      "$mcname$ rolls $ifarg:$lastarg$ ~$over you", \
      "$mcname$ rolls $ifarg:$lastarg$ ~$over $hcname$" }) }), \
  SOUL_CMD_RAVE : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "You rave$ifarg: $arg:incoherently,like a madman,like a lunatic,"+ \
      "like a loon,like a duck$~$", \
      "$mcname$ raves$ifarg: $lastarg$~$" }) }), \
])

#define SOUL_DATA_S ([ \
  SOUL_CMD_STROKE : ({ ({ 0, "%s" }), ({ \
      "You stroke $hcname$", \
      "$mcname$ strokes you", \
      "$mcname$ strokes $hcname$", }) }), \
  SOUL_CMD_SHAKE : ({ ({ 0, "%s", 1, "%s with %s", 1, "%s %s", 1, "%s %s", 3, "%s", \
      1, "%s at %s", 3, "my %s", }), ({ \
      "You shake your head$ifarg: $arg:sorrowfully,carefully,slowly,"+ \
      "once,quickly,curtly,in disagreement,vigously,completely,fishily,"+ \
      "tentatively,in agreement,in disbelief$~$", \
      "$mcname$ shakes $mposs$ head$ifarg: $lastarg$~$", \
      "You shake$ifarg: $arg:hands with,flippers with,fingers with,legs with,"+ \
      "wings with,tentacles with,your fist at$~$ $hcname$", \
      "$mcname$ shakes$ifarg: $lastarg$~$ you", \
      "$mcname$ shakes$ifarg: $lastarg$~$ $hcname$" }) }), \
  SOUL_CMD_SCARE : ({ ({ 0, "%s" }), ({ \
      "You scare the hell out of $hcname$", \
      "$mcname$ scares the hell out of you$force#fear $mname$#2$", \
      "$mcname$ scares the hell out of $hcname$" }) }), \
  SOUL_CMD_SCREAM : ({ ({ 3, "%s", 3, "in %s" }), ({ \
      "$ifarg:You scream in $arg:terror,fright,fear,pain,surprise,frustration,"+ \
      "despair,anger$$else$Arrggghhhhh!!!!~$", \
      "$mcname$ screams$ifarg: in $lastarg$$else$ loudly~$" }) }), \
  SOUL_CMD_SNIFF : ({ ({ 3, "%s" }), ({ \
      "You sniff$ifarg: $arg:dejectedly,noisily,silently,loudly,in disdain$~$", \
      "$mcname$ sniffs$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SNEEZE : ({ ({ 0, "at %s", 2, "%s at %s", 3, "%s" }), ({ \
      "You sneeze $arg:violently,loudly,silently,quietly,suddenly,unexpectedly$", \
      "$mcname$ sneezes $lastarg$", \
      "You sneeze $arg:violently,loudly,silently,quietly,suddenly,unexpectedly$"+ \
      " at $hcname$", \
      "$mcname$ sneezes $lastarg$ at you", \
      "$mcname$ sneezes $lastarg$ at $hcname$" }) }), \
  SOUL_CMD_SPIT : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "You spit$ifarg: $arg:dexterously,like a camel$~$", \
      "$mcname$ spits$ifarg: $lastarg$~$", \
      "You spit in $mhcname$ face", \
      "$mcname$ spits in your face", \
      "$mcname$ spits in $mhcname$ face" }) }), \
  SOUL_CMD_SNARL : ({ ({ 0, "%s", 1, "%s at %s", 1, "%s %s", 2, "at %s %s" }), ({ \
      "You snarl $arg:viciously,nastily,demonicaly,happily,sadly,"+ \
      "unexpectedly,suddenly$ at $hcname$", \
      "$mcname$ snarls $lastarg$ at you", \
      "$mcname$ snarls $lastarg$ at $hcname$", }) }), \
  SOUL_CMD_SNORE : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "$ifarg:You snore $arg:loudly,like a train,carelessly$$"+ \
      "else$...zzzZZZ...~$", \
      "$mcname$ snores$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SMILE : ({ ({ \
      0, "at %s", 0, "%s", 1, "%s at %s", 1, "%s %s", 2, "at %s %s", \
      2, "%s %s", 3, "%s", }), ({ \
      "You smile$ifarg: $arg:happily,sadly,nervously,wryly,broadly,crookedly,"+ \
      "stupidly,idiotically,condescendingly,ironically,patiently,brightly,"+ \
      "slightly,nastily,excitedly,strangely,with forced patience,plastically,"+ \
      "carefully,sadistically,lopsidedly,fishily,catishly,faintly,wickedly,"+ \
      "like a train,angrily,paranoicaly,innocently,frogily,slyly,"+ \
      "weakly,humbly,paradoxically,sarcastically,viciously,garishly,"+ \
      "sweetly,innocently,lovingly,dreamily,radiantly,enthusiastically,"+ \
      "insolently,quirkily,completely,unconsciously,drunkenly,hopefully,"+ \
      "insanely,cutely,maliciously,"+ \
      "absentmindedly,curtly,disturbingly,unbelievably,quietly,"+ \
      "loudly,differently,winningly,in triumph,seductively,"+ \
      "tightly,gosh there are lots of arguments,softly,proudly,"+ \
      "lividly,demonically,warmly,spasmodically,sheepishly,darkly,"+ \
      "maniacally,warily,sardonically,lazily,serenely,disappointedly$~$", \
      "$mcname$ smiles$ifarg: $lastarg$~$", \
      "You smile$ifarg: $arg:happily,sadly,nervously,wryly,broadly,crookedly,"+ \
      "stupidly,idiotically,condescendingly,ironically,patiently,brightly,"+ \
      "slightly,nastily,excitedly,strangely,with forced patience,plastically,"+ \
      "maniacally,carefully,sadistically,lopsidedly,fishily,catishly,faintly,"+ \
      "weakly,humbly,paradoxically,sarcastically,viciously,garishly,wickedly,"+ \
      "insolently,quirkily,completely,unconsciously,drunkenly,spasmodically,"+ \
      "absentmindedly,curtly,disturbingly,unbelievably,quietly,sheepishly,"+ \
      "loudly,differently,winningly,in triumph,seductively,darkly,"+ \
      "sweetly,innocently,lovingly,dreamily,radiantly,enthusiastically,"+ \
      "like a train,angrily,paranoicaly,innocently,frogily,slyly,proudly,"+ \
      "macabrely,ruefully,tentatively,like a dog,hopefully,"+ \
      "tightly,gosh there are lots of arguments,softly,"+ \
      "insanely,cutely,maliciously,"+ \
      "lividly,demonicaly,"+ \
      "sardonically,warily,lazily,serenely,disappointedly$~$ at $hcname$", \
      "$mcname$ smiles$ifarg: $lastarg$~$ at you", \
      "$mcname$ smiles$ifarg: $lastarg$~$ at $hcname$" }) }), \
  SOUL_CMD_SEDUCE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You seduce $hcname$$ifarg: $arg:daringly,disgustingly,"+ \
      "professionally,sweetly,waringly,adorably,sadistically$~$", \
      "$mcname$ seduces you$ifarg: $lastarg$~$", \
      "$mcname$ seduces $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SIGH : ({ ({ 3, "%s", 2, "%s %s", 1, "%s %s" }), ({ \
      "You sigh$ifarg: $arg:deeply,silently,desperately,tiredly,sadly,slowly,"+ \
      "dreamily,softly,happily,"+ \
      "heavily,suggestively,sarcastically,in relief,with relief$~$", \
      "$mcname$ sighs$ifarg: $lastarg$~$", \
      "You sigh$ifarg: $arg:deeply,lovingly,desperately,tiredly,sadly,"+ \
      "slowly,dreamily,softly,happily,heavily,suggestively,sarcastically,"+ \
      "in relief,with relief$~$ at $hcname$", \
      "$mcname$ sighs$ifarg: $lastarg$~$ at you", \
      "$mcname$ sighs$ifarg: $lastarg$~$ at $hcname$" }) }), \
  SOUL_CMD_STAGGER : ({ ({ 3, "%s" }), ({ \
      "You stagger around$ifarg: $arg:#$~$", \
      "$mcname$ staggers around$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SHIVER : ({ ({ 3, "with %s", 3, "in %s" }), ({ \
      "You shiver$ifarg: $arg:fear,with cold,slightly,#$~$", \
      "$mcname$ shivers$ifarg: $lastarg$~$" }) }), \
])

#define SOUL_DATA_T ([ \
  SOUL_CMD_THANK : ({ ({ 0, "%s", 2, "%s %s" }), ({ \
      "You thank $hcname$$ifarg: $arg:profusely,gratuitously,lots,slightly,"+ \
      "reservedly,solemnly,with all your heart$~$", \
      "$mcname$ thanks you$ifarg: $lastarg$~$", \
      "$mcname$ thanks $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_THINK : ({ ({ 3, "%s", }), ({ \
      "You think $ifarg:$arg:#$$else$carefully~$", \
      "$mcname$ thinks $ifarg:$lastarg$$else$carefully~$" }) }), \
  SOUL_CMD_TREMBLE : ({ ({ 3, "%s" }), ({ \
      "You tremble $arg:in fear,in nervousness,in pain,slightly,violently,"+ \
      "insistently,carefully,in a pink fit,in anticip.......pation$", \
      "$mcname$ trembles $lastarg$", }), }), \
  SOUL_CMD_TEMPT : ({ ({ 0, "%s", 2, "%s with %s" }), ({ \
      "You tempt $hcname$$ifarg: with $arg:berries,"+ \
      "money,waffels$~$", \
      "$mcname$ tempts you$ifarg: with $lastarg$~$", \
      "$mcname$ tempts $hcname$$ifarg: with $lastarg$~$" }) }), \
  SOUL_CMD_TRIP : ({ ({ 0, "%s" }), ({ \
      "You trip up", \
      "$mcname$ trips up", \
      "You trip $hcname$", \
      "$mcname$ trips you", \
      "$mcname$ trips $hcname$" }) }), \
])

#define SOUL_DATA_U ([ \
  SOUL_CMD_UNDERSTAND : ({ ({ 0, "%s" }), ({ \
      "You understand $hcname$", \
      "$mcname$ understands you", \
      "$mcname$ understands $hcname$" }) }), \
])

#define SOUL_DATA_V ([ ])

#define SOUL_DATA_W ([ \
  SOUL_CMD_WORSHIP : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "You worship $ifarg:$arg:#$$else$Ducky~$", \
      "$mcname$ worships $ifarg:$lastarg$$else$pinkfish~$", \
      "You worship $hcname$", \
      "$mcname$ worships you", "$mcname$ worships $hcname$" }) }), \
  SOUL_CMD_WALK : ({ ({ 0, "%s", 3, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "You walk around$ifarg: $arg:nervously,thinking,anxiously,"+ \
      "impatiently,slowly,quickly,angrily,waiting for an answer,"+ \
      "patiently,pondering the issue,irritatingly,"+ \
      "boredly,on one leg$~$", \
      "$mcname$ walks around$ifarg: $lastarg$~$", \
      "You walk $ifarg:$lastarg$~$around $hcname$ "+ \
      "$arg:nervously,thinking,anxiously,impatiently,slowly,quickly,"+ \
      "angrily,waiting for an answer,patiently,pondering the issue,"+ \
      "irritatingly,boredly,on one leg$", \
      "$mcname$ walks$ifarg: $lastarg$~$ around you", \
      "$mcname$ walks$ifarg: $lastarg$~$ around $hcname$", \
       }) }), \
  SOUL_CMD_WAVE : ({ ({ \
      0, "%s", 0, "at %s", 0, "to %s", 2, "at %s %s", 1, "%s %s",  \
      1, "%s to %s", 2, "to %s %s", 0, "at %s", 1, "%s at %s", \
      2, "%s %s", 3, "%s" }), ({ \
      "You wave$ifarg: $arg:tiredly,sleepily,profunctialy,lazily,sadly,"+ \
      "happily,redundantly,forlornly,excitedly,longingly,pointlessly,"+ \
      "your wings,your feathers,your tentacles,your roots$~$", \
      "$mcname$ waves$ifarg: $lastarg$~$", \
      "You wave$ifarg: $arg:sleepily,tiredly,profunctialy,lazily,sadly,"+ \
      "happily,forlornly,excitedly,longingly,redundantly,pointlessly,"+ \
      "passionately,greenly,your wings,your feathers,"+ \
      "your tentacles$~$ to $hcname$", \
      "$mcname$ waves$ifarg: $lastarg$~$ to you", \
      "$mcname$ waves$ifarg: $lastarg$~$ to $hcname$" }) }), \
  SOUL_CMD_WAIT : ({ ({ \
      0, "%s", 0, "for %s", 0, "for %s %s", 1, "%s %s", \
      0, "for %s", 1, "%s for %s", \
      2, "%s %s", 3, "%s" }), ({ \
      "You wait$ifarg: $arg:patiently,impatiently,sleepily,lazily,sadly,"+ \
      "happily,tiredly,forlornly,excitedly,longingly$~$", \
      "$mcname$ waits$ifarg: $lastarg$~$", \
      "You wait$ifarg: $arg:patiently,impatiently,sleepily,lazily,sadly,"+ \
      "happily,tiredly,forlornly,excitedly,longingly$~$ for $hcname$", \
      "$mcname$ waits$ifarg: $lastarg$~$ for you", \
      "$mcname$ waits$ifarg: $lastarg$~$ for $hcname$" }) }), \
  SOUL_CMD_WINK : ({ ({ 0, "%s", 0, "at %s", 1, "%s at %s", 1, "%s %s", \
      2, "at %s %s", 2, "%s %s", 3, "%s" }), ({ \
      "You wink$ifarg: $arg:suggestively,broadly,slowly,loudly,grumpily,"+ \
      "despairingly,coyly,desperately,insolently,curtly,lovingly,"+ \
      "patiently,sadistically,warily,seductive,lazily$~$", \
      "$mcname$ winks$ifarg: $lastarg$~$", \
      "You wink$ifarg: $arg:suggestively,broadly,slowly,loudly,grumpily,"+ \
      "despairingly,coyly,desperately,somberly,insolently,curtly,lovingly,"+ \
      "patiently,sadistically,warily,seductive,lazily$~$ at $hcname$", \
      "$mcname$ winks$ifarg: $lastarg$~$ at you", \
      "$mcname$ winks$ifarg: $lastarg$~$ at $hcname$" }) }), \
  SOUL_CMD_WONDER : ({ ({ 3, "%s" }), ({ \
      "You wonder $ifarg:$arg:#$$else$about reality and how "+ \
      "boring it can be~$", \
      "$mcname$ wonder $ifarg:$arg:#$$else$about reality and how "+ \
      "boring it can be~$", }) }), \
  SOUL_CMD_WHISTLE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s", 3, "%s" }), ({ \
      "You whistle $arg:innocently,appreciatively,loudly,musically,pleasantly,"+ \
      "discordantly$", \
      "$mcname$ whistles $lastarg$", \
      "You whistle $arg:appreciatively,innocently,loudly,musically,pleasantly,"+ \
      "discordantly,daringly,seductively$ at $hcname$", \
      "$mcname$ whistles $lastarg$ at you", \
      "$mcname$ whistles $lastarg$ at $hcname$" }) }), \
 ])

#define SOUL_DATA_X ([ ])

#define SOUL_DATA_Y ([ \
  SOUL_CMD_YAWN : ({ ({ 3, "%s" }), ({ \
      "You yawn $arg:tiredly,boredly,sleepily$", \
      "$mcname$ yawns $lastarg$" }) }) , \
])

#define SOUL_DATA_Z ([ ])
