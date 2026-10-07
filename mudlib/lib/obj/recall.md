# Recall items

A recall item takes its owner back to a place they marked. The mechanics live
in `/lib/obj/recall.c` and nothing of a setting does: what the item is called,
what it looks like and every line the player reads belong to the item that
inherits it. A fantasy game hands out a magic stone, a science fiction one a
beacon, and both behave the same.

`/lib/obj/hearthstone.c` is the one that ships with the mudlib: the fantasy
dress, a purple stone. Any fantasy game can use it as it is.

## What the player does

- **mark** the item, standing in a place that allows it (an inn, by default).
  The item remembers the place.
- **travel** (a bare verb, or the verb on the item): the owner has to stand
  still for a few seconds, and then goes back to the marked place. Moving,
  fighting or dying on the way stops it.
- After travelling, the item cannot be used again for a while.

If nothing is marked, the item takes the owner to their place of origin: the
first room of their citizenship, or else of their race (`set_init_room` in
`/lib/citizenship.c` and `/lib/race.c`), and only when that place belongs to the
game they are playing.

## Rooms and locations

It works the same with legacy rooms and with locations, so a game that was
never converted needs nothing special:

- An inn is a room with `query_pub()` (a room inheriting `/lib/ventures/pub.c`)
  or a location with the `pub` component.
- A room is remembered by its source file; a location by its `.o`.
- A room marked before its area was converted is replaced by its location when
  the owner travels, if the location exists.

## The cooldown

It is kept on the owner, as a timed property (`recall_lock`), not on the item.
It counts the owner's heart beats, so it runs while they play, survives logging
out, and stays when the item is replaced by another one. The default is one
hour of play.

## Which item a player carries

Each game says what its players must always carry, in its master object
(`/games/<game>/master.c`, which inherits `/lib/core/game.c`):

    set_mandatory_items(({ "/lib/obj/diary", "/games/<game>/obj/misc/beacon" }));

A player who lacks one of them is given it when they log in. A game that says
nothing hands out `MUST_HAVE` from `<user/player.h>`, which is just the diary.
If a game changes its recall item, the player gets the new one and the place
the old one had marked travels with it.

## Making your own

Inherit `/lib/obj/recall.c`, set the item's name and description as with any
item, and replace whatever wording you want. Anything you leave alone keeps the
neutral default.

### Messages

`set_message(key, text)`. In the text, `$mcname$` is the owner's name and
`$place$` the marked place's.

| key | when |
|---|---|
| `start_me`, `start_room` | the owner starts travelling (to them, to the room) |
| `moved` | the owner moved and the travel stopped |
| `combat` | the owner is fighting |
| `dead`, `died` | the owner is dead; the owner died while travelling |
| `acting` | the item is already working |
| `cooldown` | it cannot be used again yet |
| `arrive_me`, `arrive_room`, `leave_room` | the owner arrives; what the place they left and the place they reach see |
| `origin`, `forgotten` | nothing marked: going to the place of origin; no origin either |
| `error` | the marked place could not be loaded |
| `mark_what` | the mark verb was used on something else |
| `mark_not_here` | this place cannot be marked |
| `marked` | the place has been marked |
| `marked_info`, `unmarked_info` | added to the item's description |
| `help` | what `help <item>` shows |

### Settings

| call | default |
|---|---|
| `set_travel_verbs(({ ... }))` | `regresar`, `volver` / `recall`, `return` |
| `set_mark_verbs(({ ... }))` | `marcar` / `mark` |
| `set_channel_time(seconds)` | 10 |
| `set_cooldown(seconds)` | 3600 |

### Where it can be marked

Override `can_mark_here(object place)`. It is given the room or location the
owner stands in, and returns 1 if it may be marked.

## Examples

A beacon for a science fiction game, marked in a starport or a hotel. The
strings are inline here for a single-language game; a bilingual one keeps them
in its `.lang.<lang>.h` files, like the hearthstone.

    // /games/<game>/obj/misc/beacon.c
    inherit "/lib/obj/recall.c";

    void create()
    {
      ::create();

      set_name("baliza");
      set_short("baliza de retorno");
      add_alias("baliza");
      set_main_plural("balizas de retorno");
      add_plural("balizas");
      set_long("Un cilindro de metal del tamaño de un pulgar, con una luz " +
               "verde que parpadea despacio.\n");
      set_gender(2);

      set_travel_verbs(({ "teletransportar", "regresar" }));
      set_channel_time(5);
      set_cooldown(1800);

      set_message("start_me", "Pulsas la baliza; el zumbido empieza a subir de tono.\n");
      set_message("start_room", "La baliza de $mcname$ empieza a zumbar.\n");
      set_message("moved", "La baliza pierde la señal al moverte.\n");
      set_message("cooldown", "La baliza aún se está recargando.\n");
      set_message("arrive_me", "Un fogonazo verde y estás de vuelta.\n\n");
      set_message("arrive_room", "$mcname$ se materializa en un fogonazo verde.\n");
      set_message("leave_room", "$mcname$ se desvanece en un fogonazo verde.\n");
      set_message("mark_not_here", "Sólo puedes fijar la baliza en un puerto o un hotel.\n");
      set_message("marked", "Baliza fijada en: $place$.\n");
      set_message("marked_info", "Está fijada en: $place$.\n");
      set_message("unmarked_info", "No está fijada en ningún sitio.\n");
    }

    // a starport or a hotel, both written as locations with their own component
    int can_mark_here(object place)
    {
      return place && place->query_location() &&
             (place->has_component("starport") || place->has_component("hotel"));
    }

The game then hands it out:

    set_mandatory_items(({ "/lib/obj/diary", "/games/<game>/obj/misc/beacon" }));

A fantasy game that only wants the stone to take longer to recharge and to be
markable at any shrine as well as at an inn:

    inherit "/lib/obj/hearthstone.c";

    void create()
    {
      ::create();
      set_cooldown(7200);
      set_message("mark_not_here", "Sólo puedes hacer eso en una taberna o en un santuario.\n");
    }

    int can_mark_here(object place)
    {
      return ::can_mark_here(place) ||
             (place && place->query_location() && place->has_component("temple"));
    }

## Mounts

Summoning the owner's mount, and leaving it behind when travelling, are written
in `recall.c` but commented out until mounts work again.
