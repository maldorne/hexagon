
# Jubal's encrypting tool

Original code in [davesource.com](http://davesource.com/Projects/LPMuds/bin/x.c).

## Info

This program reads in a file and:

  - decrypts the file if it ends in EXTENSION (#defined below)
  - encrypts the file otherwise

The encryption is done by rotating with a key that the user selects
The first lines of the file are the header
Then, the next line is the key encrypted with crypt() (for checking keys)
After that is the data.

This is a bin/ type command.  To invoke it, call the function main()

> Enjoy, Jubal (Dave Ljung, DaveSource.com/GetDave.com)

This program has been successfully ported to the following drivers:

2.4.5, 3.1.2, 3.1.2 -DR, 3.1.2 -CD and 3.2 (amylaar)

This code is copyright 1993, David Ljung Madison (DaveSource.com/GetDave.com)

It may be freely redistributed, but it may not be sold in any form.

consideration:  make the write_file a diff. function
and then do a call_out to help take care of too long eval error (big files)
This won't work on most systems, because call_outs often can't do file I/O

## Modifications

Ported to Hexagon, neverbot 05/2024
