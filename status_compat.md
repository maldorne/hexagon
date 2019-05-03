# Current status: MudOS compatibility

List of MudOS `efun` functions and their implementation in `Hexagon Mudlib`.

Be aware: most of the functions
listed here are already implemented and working, but as I've just started working in this
document recently, I'll update every entry as I test each function.

## Basic Types

Common calls and operations over the basic types.

| Function | Status | Comments |
| -------- | :------: | -------- |
| `clonep`                | :white_check_mark: |  |
| `intp`                  | :white_check_mark: |  |
| `undefinedp`            | :white_check_mark: |  |
| `nullp`                 | :white_check_mark: | alias for `undefinedp` |
| `floatp`                | :white_check_mark: |  |
| `stringp`               | :white_check_mark: |  |
| `virtualp`              |  |  |
| `functionp`             | - | nonexistant in dgd |
| `pointerp`              | :white_check_mark: | alias for `arrayp` |
| `arrayp`                | :white_check_mark: |  |
| `objectp`               | :white_check_mark: |  |
| `classp`                | - | nonexistant in dgd |
| `mapp`                  | :white_check_mark: | alias for `mappingp` |
| `userp`                 | :white_check_mark: |  |
| **strings**             |  |  |
| `capitalize`            | :white_check_mark: |  |
| `lower_case`            | :white_check_mark: |  |
| `replace_string`        | :white_check_mark: |  |
| `sprintf`               | :white_check_mark: | included from the [sprintf lpc package](https://github.com/dworkin/gurbalib/tree/master/contrib/Sprintf) by Haijo Schipper (GPL license). Used in the form `static string sprintf (string format, mixed args...)` as there are no way to modify strings passed as arguments. Expanded to accept strings with multiple lines. |
| `printf`                | :white_check_mark: | alias for `write(sprintf(...));` |
| `strlen`                | :white_check_mark: | provided by dgd, added `visible_strlen` to count only visible/printable characters |
| `strcmp`                | :white_check_mark: |  |
| `strsrch`               | :white_check_mark: |  |
| **arrays**              |  |  |
| `sizeof`                | :white_check_mark: | provided by dgd |
| `explode`               | :white_check_mark: | provided by dgd, also added `full_explode` |
| `implode`               | :white_check_mark: | provided by dgd |
| `allocate`              | :white_check_mark: | note: all array elements are initialized to `nil` by default. To initialize the array elements to zero, use `allocate_int`. |
| `member_array`          | :white_check_mark: |  |
| `index`                 | :white_check_mark: | alias for `member_array` |
| `sort_array`            | :white_check_mark: |  |
| `unique_array`          | :white_check_mark: |  |
| `filter`                | :white_check_mark: |  |
| `filter_array`          | :white_check_mark: |  |
| **mappings**            |  |  |
| `values`                | :white_check_mark: | alias for `map_values` |
| `keys`                  | :white_check_mark: | alias for `map_indices` |
| `map_delete`            | :white_check_mark: |  |
| `m_delete`              | :white_check_mark: | alias for `map_delete` |
| `m_values`              | :white_check_mark: | alias for `map_values` |
| `m_indices`             | :white_check_mark: | alias for `map_indices` |
| `allocate_mapping`      |  |  |
| `unique_mapping`        |  |  |
| `map`                   |  |  |
| `map_mapping`           |  |  |
| `map_array`             | :white_check_mark: |  |
| `filter_mapping`        | :white_check_mark: |  |
| **functions**           |  |  |
| `function_exists`       | :white_check_mark: | alias for `function_object` |


## Objects

| Function | Status | Comments |
| -------- | :------: | -------- |
| `clone_object`          | :white_check_mark: | provided by dgd, extended |
| `new`                   | - | provided by dgd, different behaviour, use `clone_object` instead |
| `destruct`              | :white_check_mark: | alias for `destruct_object` |
| `find_object`           | :white_check_mark: | provided by dgd |
| `load_object`           | :white_check_mark: | extended alias for `compile_object` |


## Basic calls and interactions between objects

The general idea of how the `this_player` value is being stored in a central object (`/lib/core/mudos.c`, which is in charge of doing the respective `init` calls and stores current `query_verb`, `notify_fail`, etc) has been taken from the `mudos_alike_4_dgd` mudlib, although the code had to be written from scratch as it do not have an open source license.

| Function | Status | Comments |
| -------- | :------: | -------- |
| `init`                  | :white_check_mark: | it's not a function provided by MudOS per se, it's the function that will be called inside every object when they move (if it exists). Listed here for clarity, as `Hexagon Mudlib` makes the same calls. |
| `this_player`           | :white_check_mark: |  |
| `this_user`             | :white_check_mark: | provided by dgd |
| `this_interactive`      | :white_check_mark: | alias for `this_user` |
| `previous_object`       | :white_check_mark: |  |
| `all_previous_objects`  | :white_check_mark: | alias for `previous_objects` |
| `evaluate`              | :white_check_mark: |  |
| `add_action`            | :white_check_mark: |  |
| `remove_action`         |  |  |
| `query_verb`            | :white_check_mark: |  |
| `command`               | :white_check_mark: |  |
| `commands`              | :white_check_mark: |  |
| `notify_fail`           | :white_check_mark: |  |
| `enable_commands`       | :white_check_mark: |  |
| `disable_commands`      |  |  |
| `interactive`           | :white_check_mark: |  |
| `set_living_name`       | :white_check_mark: |  |
| `living`                | :white_check_mark: |  |
| `find_living`           | :white_check_mark: |  |
| `find_player`           | :white_check_mark: |  |
| `livings`               | :white_check_mark: |  |
| `set_heart_beat`        | :white_check_mark: |  |
| `query_heart_beat`      | :white_check_mark: |  |
| `users`                 | :white_check_mark: | provided by dgd, masked to use user handler. `players` added to obtain the list of player objects. |
| `players`               | :white_check_mark: | needed after separating users in two different objects: `user` manages connections, input and output, and `player` manages the in-game avatar |
| `objects`               |  |  |
| `children`              |  |  |

## Functions
| Function | Status | Comments |
| -------- | :------: | -------- |
| `call_out`              | :white_check_mark: | provided by dgd, expanded to store list of pending call outs |
| `remove_call_out`       | :white_check_mark: | provided by dgd, expanded to store list of pending call outs |
| `find_call_out`         | :white_check_mark: |  |
| `call_out_info`         | :white_check_mark: |  |

## Time & Date

| Function | Status | Comments |
| -------- | :------: | -------- |
| `time`                  | :white_check_mark: | provided by dgd |
| `ctime`                 | :white_check_mark: | provided by dgd, masked as efun to translate and add functionality |
| `localtime`             | :white_check_mark: | done using the `sprintf` time functions, would be reimplemented |

## Movement, Inventory & Environment

| Function | Status | Comments |
| -------- | :------: | -------- |
| `init`                  | :white_check_mark: | see above |
| `environment`           | :white_check_mark: |  |
| `all_inventory`         | :white_check_mark: |  |
| `deep_inventory`        | :white_check_mark: |  |
| `first_inventory`       | :white_check_mark: |  |
| `next_inventory`        | :white_check_mark: |  |
| `id`                    | :white_check_mark: |  |
| `present`               | :white_check_mark: |  |
| `move`                  | :white_check_mark: |  |
| `move_object`           | :white_check_mark: | alias for `move` |

## Files and paths
| Function | Status | Comments |
| -------- | :------: | -------- |
| `file_name`             | :white_check_mark: | alias for `object_name` |
| `restore_object`        | :white_check_mark: |  |
| `save_object`           | :white_check_mark: |  |
| `get_dir`               | :white_check_mark: | masked to mimic MudOS behaviour |
| `match_path`            |  |  |
| `read_file`             | :white_check_mark: | The DGD version reads one character at a time. The MudOS version reads one line. Use `read_file_line` for this last behaviour. |
| `read_bytes`            | :white_check_mark: | alias for `read_file` |
| `write_file`            | :white_check_mark: | provided by dgd, masked for permissions |
| `write_bytes`           |  |  |
| `rename`                | :white_check_mark: | alias for `rename_file`, masked for permissions |
| `file_size`             | :white_check_mark: | Mikael Lind's example code [from Phantasmal docs](http://phantasmal.sourceforge.net/DGD/external/file_size.c) |
| `cp`                    | :white_check_mark: |  |
| `link`                  | - | nonexistant in dgd |
| `mkdir`                 | :white_check_mark: |  |
| `rm`                    | :white_check_mark: |  |
| `rmdir`                 | :white_check_mark: |  |

## Interactions with users
| Function | Status | Comments |
| -------- | :------: | -------- |
| `write`                 | :white_check_mark: |  |
| `say`                   | :white_check_mark: |  |
| `tell_object`           | :white_check_mark: |  |
| `tell_room`             | :white_check_mark: |  |
| `shout`                 |  |  |
| `receive`               |  |  |
| `message`               | :white_check_mark: |  |
| `input_to`              | :white_check_mark: | accepts optional parameters `input_to(func, flag, args...)` |


## Driver configuration
| Function | Status | Comments |
| -------- | :------: | -------- |
| `eval_cost`             | :white_check_mark: | uses `status`  |
| `reset_eval_cost`       | :white_check_mark: | dummy function |
| `set_eval_limit`        | :white_check_mark: | dummy function |

## Others
| Function | Status | Comments |
| -------- | :------: | -------- |
| `crypt`                 | :white_check_mark: | provided by dgd |
| `oldcrypt`              | :white_check_mark: | alias for `crypt`, commented to have less functions in the auto ob |
| `debug_info`            | :white_check_mark: | not same info as in mudos, returns `status(ob)` converted to a string |
| `snoop`                 |  | dummy function for now |
| `query_snoop`           |  |  |
| `query_snooping`        |  |  |
| `query_ip_number`       | :white_check_mark: | provided by dgd |
| `query_ip_name`         | :white_check_mark: | provided by dgd |
| `query_host_name`       |  |  |
| `resolve`               |  |  |
| `query_idle`            | :white_check_mark: | moved to local function in the user object |
| `random`                | :white_check_mark: | provided by dgd |
| `shadow`                | :white_check_mark: |  |
| `query_shadowing`       | :white_check_mark: |  |
| `bind`                  |  |  |
| `save_variable`         |  |  |
| `restore_variable`      |  |  |
| `call_stack`            |  |  |
| `inherits`              |  |  |
| `replace_program`       |  |  |
| `regexp`                |  |  |
| `reg_assoc`             |  |  |
| `crc32`                 |  |  |
| `clear_bit`             |  |  |
| `test_bit`              |  |  |
| `set_bit`               |  |  |
| `next_bit`              |  |  |
| `exec`                  | - | dummy function |
| `set_hide`              |  |  |
| `throw`                 |  |  |
| `error`                 |  |  |
| `deep_inherit_list`     |  |  |
| `shallow_inherit_list`  |  |  |
| `stat`                  |  |  |
| `in_edit`               |  |  |
| `in_input`              |  |  |
| `master`                |  |  |
| `memory_info`           | :white_check_mark: |  |
| `cache_stats`           | - | nonexistant in dgd |
| `malloc_status`         |  |  |
| `query_load_average`    |  |  |
| `mud_status`            |  |  |
| `get_config`            |  |  |
| `get_char`              |  |  |
| `reload_object`         |  |  |
| `flush_messages`        |  |  |
| `ed`                    |  |  |
| `ed_start`              |  |  |
| `ed_cmd`                |  |  |
| `query_ed_mode`         |  |  |
| `dumpallobj`            |  |  |
| `dump_file_descriptors` |  |  |
| `set_light`             |  |  |
| `origin`                |  |  |
| `reclaim_objects`       |  |  |
| `uptime`                | :white_check_mark: | uses `status` |
| `shutdown`              | :white_check_mark: |  |

## Other optional MudOS packages
| Function | Package | Status | Comments |
| -------- | :------: | :------: | -------- |
| `domain_stats`         | PACKAGE_MUDLIB_STATS | :heavy_multiplication_x: | Not used |
| `author_stats`         | PACKAGE_MUDLIB_STATS | :heavy_multiplication_x: | Not used |
