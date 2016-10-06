#Current status: MudOS compatibility

List of MudOS `efun` functions and their implementation in `ccmudlib`. 

Be aware: most of the functions 
listed here are already implemented and working, but as I've just started working in this 
document recently, I'll update every entry as I test each function.

##Basic Types

Common calls and operations over the basic types.

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `clonep`                |  |  |  |
| `intp`                  |  | :white_check_mark: |  |
| `undefinedp`            |  | :white_check_mark: |  |
| `nullp`                 |  | :white_check_mark: | alias for `undefinedp` |
| `floatp`                |  | :white_check_mark: |  |
| `stringp`               |  | :white_check_mark: |  |
| `virtualp`              |  |  |  |
| `functionp`             |  | - | nonexistant in dgd |
| `pointerp`              |  | :white_check_mark: | alias for `arrayp` |
| `arrayp`                |  | :white_check_mark: |  |
| `objectp`               |  | :white_check_mark: |  |
| `classp`                |  | - | nonexistant in dgd |
| `mapp`                  |  | :white_check_mark: | alias for `mappingp` |
| `userp`                 |  | :white_check_mark: |  |
| **strings**             |  |  |  |
| `capitalize`            |  |  |  |
| `lower_case`            |  |  |  |
| `replace_string`        |  | :white_check_mark: |  |
| `sprintf`               |  | :white_check_mark: | included from the [sprintf lpc package](https://github.com/dworkin/gurbalib/tree/master/contrib/Sprintf) by Haijo Schipper (GPL license). Used in the form `static string sprintf (string format, mixed args...)` as there are no way to modify strings passed as arguments. |
| `printf`                |  | :white_check_mark: | alias for `sprintf` |
| `strlen`                |  | :white_check_mark: | provided by dgd |
| `strcmp`                |  |  |  |
| `strsrch`               |  | :white_check_mark: |  |
| **arrays**              |  |  |  |
| `sizeof`                |  | :white_check_mark: | provided by dgd |
| `explode`               |  |  |  |
| `implode`               |  |  |  |
| `allocate`              |  | :white_check_mark: | note: all array elements are initialized to `nil` |
| `member_array`          |  | :white_check_mark: |  |
| `index`                 |  | :white_check_mark: | alias for `member_array` |
| `sort_array`            |  |  |  |
| `unique_array`          |  |  |  |
| `filter`                |  |  |  |
| `filter_array`          |  |  |  |
| **mappings**            |  |  |  |
| `values`                |  |  |  |
| `keys`                  |  |  |  |
| `map_delete`            |  |  |  |
| `m_delete`              |  |  |  |
| `m_values`              |  |  |  |
| `m_indices`             |  |  |  |
| `allocate_mapping`      |  |  |  |
| `unique_mapping`        |  |  |  |
| `map`                   |  |  |  |
| `map_mapping`           |  |  |  |
| `map_array`             |  |  |  |
| `filter_mapping`        |  |  |  |


##Objects

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `destruct`              |  | :white_check_mark: | alias for `destruct_object` |
| `find_object`           |  | :white_check_mark: | provided by dgd |
| `load_object`           |  | :white_check_mark: | extended alias for `compile_object` |


##Basic calls and interactions between objects

The general scheme of how the `this_player` value is being stored in a central object (`/lib/core/mudos.c`,
which is in charge of doing the respective `init` calls) has been taken from the 
`mudos_alike_4_dgd` mudlib, although the code had to be written from scratch as it 
do not have an open source license.

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `init`                  |  | :white_check_mark: | it's not a function provided by MudOS per se, it's the function that will be called inside every object when they move (if it exists). Listed here for clarity, as `ccmudlib` makes the same calls. |
| `this_player`           |  | :white_check_mark: |  |
| `this_interactive`      |  |  |  |
| `this_user`             |  |  |  |
| `previous_object`       |  | :white_check_mark: |  |
| `all_previous_objects`  |  | :white_check_mark: | alias for `previous_objects` |
| `evaluate`              |  | :white_check_mark: |  |
| `add_action`            |  |  |  |
| `query_verb`            |  |  |  |
| `command`               |  |  |  |
| `remove_action`         |  |  |  |
| `commands`              |  |  |  |
| `disable_commands`      |  |  |  |
| `enable_commands`       |  |  |  |
| `interactive`           |  |  |  |
| `set_heart_beat`        |  | :white_check_mark: |  |
| `query_heart_beat`      |  | :white_check_mark: |  |

##Time & Date

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `time`                  |  | :white_check_mark: | provided by dgd |
| `ctime`                 |  | :white_check_mark: | provided by dgd, masked as efun to translate and add functionality |
| `localtime`             |  | :white_check_mark: | done using the `sprintf` time functions, would be reimplemented |

##Movement, Inventory & Environment

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `init`                  |  | :white_check_mark: | see above |
| `environment`           |  | :white_check_mark: |  |
| `all_inventory`         |  | :white_check_mark: |  |
| `deep_inventory`        |  | :white_check_mark: |  |
| `first_inventory`       |  |  |  |
| `next_inventory`        |  |  |  |
| `present`               |  |  |  |
| `move`                  |  | :white_check_mark: |  |
| `move_object`           |  | :white_check_mark: | alias for `move` |

##Files and paths
| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `file_name`             |  | :white_check_mark: | alias for `object_name` |
| `restore_object`        |  |  |  |
| `save_object`           |  |  |  |
| `get_dir`               |  | :white_check_mark: | masked to mimic MudOS behaviour |
| `match_path`            |  |  |  |
| `write_file`            |  |  |  |
| `rename`                |  |  |  |
| `write_bytes`           |  |  |  |
| `file_size`             |  | :white_check_mark: | Mikael Lind's example code [from Phantasmal docs](http://phantasmal.sourceforge.net/DGD/external/file_size.c) |
| `read_bytes`            |  |  |  |
| `read_file`             |  |  |  |
| `cp`                    |  |  |  |
| `link`                  |  |  |  |
| `mkdir`                 |  |  |  |
| `rm`                    |  |  |  |
| `rmdir`                 |  |  |  |

##Interactions with users
| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `write`                 |  | :white_check_mark: |  |
| `tell_object`           |  |  |  |
| `shout`                 |  |  |  |
| `receive`               |  |  |  |
| `message`               |  |  |  |


##Driver configuration
| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `eval_cost`             |  | :white_check_mark: | uses `status`  |
| `reset_eval_cost`       |  | :white_check_mark: | dummy function |
| `set_eval_limit`        |  | :white_check_mark: | dummy function |


| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `bind`                  |  |  |  |
| `input_to`              |  |  |  |
| `random`                |  |  |  |
| `say`                   |  |  |  |
| `tell_room`             |  |  |  |
| `living`                |  | :white_check_mark: |  |
| `set_living_name`       |  |  |  |
| `livings`               |  |  |  |
| `find_living`           |  |  |  |
| `find_player`           |  |  |  |
| `notify_fail`           |  |  |  |
| `save_variable`         |  |  |  |
| `restore_variable`      |  |  |  |
| `users`                 |  |  |  |
| `call_stack`            |  |  |  |
| `call_out`              |  |  |  |
| `find_call_out`         |  |  |  |
| `call_out_info`         |  |  |  |
| `inherits`              |  |  |  |
| `replace_program`       |  |  |  |
| `regexp`                |  |  |  |
| `reg_assoc`             |  |  |  |
| `crc32`                 |  |  |  |
| `clear_bit`             |  |  |  |
| `test_bit`              |  |  |  |
| `set_bit`               |  |  |  |
| `next_bit`              |  |  |  |
| `crypt`                 |  |  |  |
| `oldcrypt`              |  |  |  |
| `exec`                  |  |  |  |
| `function_exists`       |  |  |  |
| `objects`               |  |  |  |
| `query_host_name`       |  |  |  |
| `query_idle`            |  |  |  |
| `query_ip_name`         |  |  |  |
| `query_ip_number`       |  |  |  |
| `snoop`                 |  |  |  |
| `query_snoop`           |  |  |  |
| `query_snooping`        |  |  |  |
| `remove_call_out`       |  |  |  |
| `set_hide`              |  |  |  |
| `shadow`                |  |  |  |
| `query_shadowing`       |  |  |  |
| `throw`                 |  |  |  |
| `deep_inherit_list`     |  |  |  |
| `shallow_inherit_list`  |  |  |  |
| `stat`                  |  |  |  |
| `in_edit`               |  |  |  |
| `in_input`              |  |  |  |
| `master`                |  |  |  |
| `memory_info`           |  |  |  |
| `get_config`            |  |  |  |
| `get_char`              |  |  |  |
| `children`              |  |  |  |
| `reload_object`         |  |  |  |
| `error`                 |  |  |  |
| `uptime`                |  |  |  |
| `flush_messages`        |  |  |  |
| `ed`                    |  |  |  |
| `ed_start`              |  |  |  |
| `ed_cmd`                |  |  |  |
| `query_ed_mode`         |  |  |  |
| `cache_stats`           |  |  |  |
| `malloc_status`         |  |  |  |
| `mud_status`            |  |  |  |
| `dumpallobj`            |  |  |  |
| `dump_file_descriptors` |  |  |  |
| `query_load_average`    |  |  |  |
| `set_light`             |  |  |  |
| `origin`                |  |  |  |
| `reclaim_objects`       |  |  |  |
| `resolve`               |  |  |  |
| `shutdown`              |  |  |  |
