# Current status: Commands

List of commands available to every player, depending of its user type/role (`player`, `coder`, `admin`, and the common type `login` always available, even in the login screen before choosing a player character).

**Be aware**: most of the commands listed here are already implemented and working (might not be translated), but as I've just started working in this document recently, I'll update every entry as I test each command.

## Login

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| characters | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| finger     | :white_check_mark: | :white_check_mark: | :x: |    |
| help       | :white_check_mark: | :x: | :white_check_mark: |    |
| who        | :white_check_mark: | :white_check_mark: |   | The weather time (depends of the game) has to be translated yet. Help is half done. |

## Admin

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| ban        |          |          |    |    |
| clonein    |          |          |    |    |
| dirs       |          |          |    |    |
| domain     |          |          |    |    |
| domainwho  |          |          |    |    |
| donde      |          |          |    |    |
| echoall    |          |          |    |    |
| fds        |          |          |    |    |
| findfile   |          |          |    |    |
| fsearch    |          |          |    |    |
| guildwho   |          |          |    |    |
| ident      |          |          |    |    |
| in         |          |          |    |    |
| ll         |          |          |    |    |
| memory     |          |          |    |    |
| memtree    |          |          |    |    |
| money_info |          |          |    |    |
| netstat    |          |          |    |    |
| people     |          |          |    |    |
| qatt       |          |          |    |    |
| qc         |          |          |    |    |
| qpeople    |          |          |    |    |
| register   |          |          |    |    |
| resolve    |          |          |    |    |
| rotate     |          |          |    |    |
| shutdown   |          |          |    |    |
| singletons |          |          |    |    |
| snetstat   |          |          |    |    |
| snoopers   |          |          |    |    |
| sockets    |          |          |    |    |
| stats      |          |          |    |    |
| status     |          |          |    |    |
| su         |          |          |    |    |
| suspend    |          |          |    |    |
| terms      |          |          |    |    |
| unicos     |          |          |    |    |
| whois      |          |          |    |    |
| xdate      |          |          |    |    |
| xfinger    |          |          |    |    |

## Coder

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| armaduras    |          |          |    |    |
| backup       |          |          |    |    |
| cat          |          |          |    |    |
| clone        |          |          |    |    |
| compare      |          |          |    |    |
| cp           |          |          |    |    |
| cplines      |          |          |    |    |
| cprange      |          |          |    |    |
| discard      |          |          |    |    |
| du           |          |          |    |    |
| exec         |          |          |    |    |
| exits        |          |          |    |    |
| ftpwho       |          |          |    |    |
| goto         |          |          |    |    |
| grep         |          |          |    |    |
| head         |          |          |    |    |
| home         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| invisible    |          |          |    |    |
| ioulist      |          |          |    |    |
| load         |          |          |    |    |
| ls           |          |          |    |    |
| mail         |          |          |    |    |
| malloc       |          |          |    |    |
| man          |          |          |    |    |
| mkdir        |          |          |    |    |
| more         |          |          |    |    |
| mudlist      |          |          |    |    |
| multi        |          |          |    |    |
| mv           |          |          |    |    |
| nls          |          |          |    |    |
| peace        |          |          |    |    |
| resistencias |          |          |    |    |
| restore      |          |          |    |    |
| rm           |          |          |    |    |
| rmdir        |          |          |    |    |
| sar          |          |          |    |    |
| tail         |          |          |    |    |
| uptime       |          |          |    |    |
| visible      |          |          |    |    |
| wget         |          |          |    |    |

## Player

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| apuntador     |          |          |    |    |
| ausente       |          |          |    |    |
| buscar        |          |          |    |    |
| cabalgar      |          |          |    |    |
| calendar      |          |          |    |    |
| carga         |          |          |    |    |
| cobardia      |          |          |    |    |
| comandos      |          |          |    |    |
| combates      |          |          |    |    |
| configuracion |          |          |    |    |
| considerar    |          |          |    |    |
| coste         |          |          |    |    |
| detener       |          |          |    |    |
| dinero        |          |          |    |    |
| diplomacia    |          |          |    |    |
| enterrar      |          |          |    |    |
| equipo        |          |          |    |    |
| estado        |          |          |    |    |
| glance        |          |          |    |    |
| grope         |          |          |    |    |
| grupo         |          |          |    |    |
| inventory     |          |          |    |    |
| kill          |          |          |    |    |
| look          |          |          |    |    |
| map           |          |          |    |    |
| modo          |          |          |    |    |
| monitor       |          |          |    |    |
| past          |          |          |    |    |
| prompt        |          |          |    |    |
| quests        |          |          |    |    |
| ranking       |          |          |    |    |
| retirar       |          |          |    |    |
| score         |          |          |    |    |
| sheet         |          |          |    |    |
| time          |          |          |    |    |
| whoami        |          |          |    |    |

## Legend

| Symbol | Meaning |
| :----: | ------- |
| :white_check_mark:            | Already tested. |
| :negative_squared_cross_mark: | Not included/necessary. |
| :x:                           | Not at the moment. |
