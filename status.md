#Current status of the project

List of MudOS `efun` functions and their implementation in `ccmudlib`

##Types

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
| `sizeof`                |  |  |  |
| `capitalize`            |  |  |  |
| `explode`               |  |  |  |
| `implode`               |  |  |  |
| `allocate`              |  |  |  |
| `member_array`          |  |  |  |
| `sort_array`            |  |  |  |
| `unique_array`          |  |  |  |
| `lower_case`            |  |  |  |
| `replace_string`        |  |  |  |
| `strsrch`               |  |  |  |
| `values`                |  |  |  |
| `keys`                  |  |  |  |
| `map_delete`            |  |  |  |
| `m_delete`              |  |  |  |
| `m_values`              |  |  |  |
| `m_indices`             |  |  |  |
| `unique_mapping`        |  |  |  |
| `printf`                |  |  |  |
| `sprintf`               |  |  |  |
| `strcmp`                |  |  |  |
| `filter`                |  |  |  |
| `filter_array`          |  |  |  |
| `filter_mapping`        |  |  |  |
| `map`                   |  |  |  |
| `map_mapping`           |  |  |  |
| `map_array`             |  |  |  |


##Basic calls and interactions between objects

The general scheme of how the `this_player` value is being stored in a central object (`/lib/core/mudos.c`,
which is in charge of doing the respective `init` calls) has been taken from the 
`mudos_alike_4_dgd` mudlib, although the code had to be written from scratch as it 
do not have an open source license.

| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `init`                  |  | :white_check_mark: |  |
| `this_player`           |  | :white_check_mark: |  |
| `this_interactive`      |  |  |  |
| `this_user`             |  |  |  |
| `previous_object`       |  | :white_check_mark: |  |
| `all_previous_objects`  |  | :white_check_mark: | alias for `previous_objects` |
| `evaluate`              |  | :white_check_mark: |  |


| Function | Description | Status | Comments |
| -------- | ----------- | :------: | -------- |
| `bind`                  |  |  |  |
| `call_stack`            |  |  |  |
| `destruct`              |  |  |  |
| `file_name`             |  |  |  |
| `call_out`              |  |  |  |
| `input_to`              |  |  |  |
| `random`                |  |  |  |
| `environment`           |  |  |  |
| `all_inventory`         |  |  |  |
| `deep_inventory`        |  |  |  |
| `first_inventory`       |  |  |  |
| `next_inventory`        |  |  |  |
| `say`                   |  |  |  |
| `tell_room`             |  |  |  |
| `present`               |  |  |  |
| `move_object`           |  |  |  |
| `add_action`            |  |  |  |
| `query_verb`            |  |  |  |
| `command`               |  |  |  |
| `remove_action`         |  |  |  |
| `living`                |  |  |  |
| `commands`              |  |  |  |
| `disable_commands`      |  |  |  |
| `enable_commands`       |  |  |  |
| `set_living_name`       |  |  |  |
| `livings`               |  |  |  |
| `find_living`           |  |  |  |
| `find_player`           |  |  |  |
| `notify_fail`           |  |  |  |
| `restore_object`        |  |  |  |
| `save_object`           |  |  |  |
| `save_variable`         |  |  |  |
| `restore_variable`      |  |  |  |
| `users`                 |  |  |  |
| `get_dir`               |  |  |  |
| `write`                 |  |  |  |
| `tell_object`           |  |  |  |
| `shout`                 |  |  |  |
| `receive`               |  |  |  |
| `message`               |  |  |  |
| `find_object`           |  |  |  |
| `load_object`           |  |  |  |
| `find_call_out`         |  |  |  |
| `allocate_mapping`      |  |  |  |
| `match_path`            |  |  |  |
| `inherits`              |  |  |  |
| `replace_program`       |  |  |  |
| `regexp`                |  |  |  |
| `reg_assoc`             |  |  |  |
| `call_out_info`         |  |  |  |
| `crc32`                 |  |  |  |
| `write_file`            |  |  |  |
| `rename`                |  |  |  |
| `write_bytes`           |  |  |  |
| `file_size`             |  |  |  |
| `read_bytes`            |  |  |  |
| `read_file`             |  |  |  |
| `cp`                    |  |  |  |
| `link`                  |  |  |  |
| `mkdir`                 |  |  |  |
| `rm`                    |  |  |  |
| `rmdir`                 |  |  |  |
| `clear_bit`             |  |  |  |
| `test_bit`              |  |  |  |
| `set_bit`               |  |  |  |
| `next_bit`              |  |  |  |
| `crypt`                 |  |  |  |
| `oldcrypt`              |  |  |  |
| `ctime`                 |  |  |  |
| `exec`                  |  |  |  |
| `localtime`             |  |  |  |
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
| `set_heart_beat`        |  |  |  |
| `query_heart_beat`      |  |  |  |
| `set_hide`              |  |  |  |
| `shadow`                |  |  |  |
| `query_shadowing`       |  |  |  |
| `throw`                 |  |  |  |
| `time`                  |  |  |  |
| `deep_inherit_list`     |  |  |  |
| `shallow_inherit_list`  |  |  |  |
| `stat`                  |  |  |  |
| `interactive`           |  |  |  |
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
| `set_eval_limit`        |  |  |  |
| `resolve`               |  |  |  |
| `shutdown`              |  |  |  |
