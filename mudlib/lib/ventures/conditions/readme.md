# Open conditions

A venture — a shop or a tavern — opens under a list of **conditions**, and it
serves somebody only while every one of them holds. Each condition is a small
object in this directory (or in a game's own `ventures/conditions/`), and the
venture keeps, for each one, its path and the arguments it was given. The list
is saved with the venture, so it survives reloads.

Every venture starts with one condition, `attended`.

## The contract

A condition object answers:

    mixed check_open(object venture, object location, object who, mapping args)
    string query_description(mapping args)

- `venture` is the shop or pub (a location component, or a legacy room);
  `location` is where it stands; `who` is the customer (may be nil); `args` are
  the condition's own arguments.
- `check_open` returns `1` when the venture may serve, or a string: the reason,
  which is exactly what the customer reads ("There is nobody serving."). A `0`
  closes it with the generic message.
- `query_description` is one line for `build venture`.

The venture checks the conditions in order, every time a customer buys, sells or
asks for a value; the first one that does not hold gives the answer. Listing the
goods and browsing one of them work while it is closed, so a customer can see
whether it is worth coming back: the answer is then added under what they see.

## The conditions here

| condition | arguments | open when |
|---|---|---|
| `attended` | — | somebody whose work is this location is in it and is not fighting anybody here; always, when no job is held at the location |
| `hours` | `from`, `to` (game hours, 0-23) | the game hour is in `[from, to)`, across midnight when `to < from` |
| `date` | `day` (day of the year, 1-365), `days` (default 1) | today is one of the `days` days starting on `day` |

The blacksmith component's forge uses `attended` too: it burns while the smith
is at work.

## Using it

Standing in the shop or tavern, with the builder ring:

    build venture                                    # list, and whether it is open now
    build venture condition add hours from=8 to=14   # mornings only
    build venture condition add date day=200 days=3  # a fair, three days a year
    build venture condition remove attended          # nobody needs to be at the counter
    build venture condition add attended             # put it back

A bare name is looked up in `/games/<game>/ventures/conditions/` first and here
after, so a game can add conditions of its own or replace one of these; a full
path is used as given. Adding a condition that is already there replaces its
arguments.

## Writing a new one

A tavern that only serves the citizens of its own realm:

    // /games/<game>/ventures/conditions/citizens.c
    mixed check_open(object venture, object location, object who, mapping args)
    {
      if (!who || who->query_citizenship() == args["of"])
        return 1;
      return "We do not serve strangers here.\n";
    }

    string query_description(mapping args)
    {
      return "serves only citizens of " + args["of"];
    }

    build venture condition add citizens of=/games/<game>/obj/citizenships/<realm>
