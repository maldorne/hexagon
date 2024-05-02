
# uuid.c

Found in:

[LPCsnippets Github Repository](https://github.com/atari2600tim/LPCsnippets/blob/master/uuid.c).

## Info

This is a "version 4" uuid.  This is based on random numbers, the others use time.
https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
supposed to use lower case but accept upper case (so run lower_case when comparing incoming stuff)

Author unknown.

Example:

```
> exec return "/packages/uuid/src/uuid.c"->uuid()
Result:
 "be97d609-4d5d-4e35-9b17-00a2f85d353f"
```
