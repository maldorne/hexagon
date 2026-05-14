
# json

JSON serializer and parser for Hexagon, in LPC.

## Layout

```
/packages/json/
    master.c          public entry point: encode(value) / decode(text)
    src/
        encode.c      serializer (entry: encode)
        decode.c      parser (entry: decode)
```

## Usage

The mudlib exposes two simulated kfuns inherited from `auto`:

```c
string json_encode(mixed value);
mixed  json_decode(string text);
```

These are thin wrappers defined in `/lib/core/efuns/json.c` that forward to `"/packages/json/master"->encode(...)` and `->decode(...)`. The wrappers exist so that only **two** functions land in the auto object, regardless of how many private helpers the implementation grows. Direct callers can still invoke `"/packages/json/master"->encode(v)` if they want to skip the sefun layer.

## Type mapping

| LPC value             | JSON                                |
|:----------------------|:------------------------------------|
| `nil`                 | `null`                              |
| `int`                 | number (integer)                    |
| `float`               | number                              |
| `string`              | string (JSON escape rules)          |
| `mixed *`             | array                               |
| `mapping` (string keys) | object                            |
| `mapping` (other keys)  | error                             |
| `object`              | error (see notes below)             |

LPC has no native boolean: `0` and `1` encode as JSON numbers, not as `false`/`true`. Conversely, `json_decode` maps `true` → `1`, `false` → `0`, `null` → `nil`.

## Why LPC objects raise

The word "object" is overloaded here: a JSON _object_ is a key-value map (which we encode from LPC mappings), while an LPC _object_ is a reference to a mud entity (a player, a room, a clone). The encoder handles the first perfectly; the second is rejected.

Concretely, if `json_encode` finds a value of LPC type `object` anywhere in the structure — at the top level, inside an array, or as a value in a mapping — it raises. There is no generic answer to "how should a player object be turned into JSON?" — its name? its id? a snapshot of its properties? — and that decision belongs to the caller, not to the encoder. Flatten objects to a stringifiable identifier (name, id, file path…) before calling.

This mirrors the limitation documented at `notes/lpc/dgd/writing-dgd-extensions.md §7.2`: even a C kfun built against the DGD extension API could not solve this on its own, because there is no `lpc_object_getval` to read object internals from outside.

## Tests

`/packages/test/src/efuns/json.c` exercises both functions against ~50 test groups. Run with the `test` admin command.

## See also

- `/packages/json-old/` — historical port of Lost Souls MUD's `grimoire_json`. Unused; kept as a reference implementation.
