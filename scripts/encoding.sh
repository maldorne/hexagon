#!/bin/bash

# usage:
# execute "../scripts/encoding.sh" from the mudlib root directory
# 
# this script has been tested and used in MacOS Ventura 13.2
# it needs the command 'sponge' (brew install sponge in macos)
# 
# it will probably work in other flavours of unix/linux, although iconv arguments
# might be different (test them) and installing sponge will be done with
# their proper package managers

# we need sponge to be installed
if ! command -v sponge &> /dev/null
then
    echo "sponge could not be found, must be installed"
    exit
fi

# list all files but .git and assets directories
for file in `find . \( -path ./.git -prune -o -path ./assets -prune \) -o -print`
do
encoding=$(file -b --mime-encoding $file)

case $encoding in
  # directories, empty files
  "binary")
    # echo "$file is binary"
    # do nothing
    ;;

  # already utf-8 :)
  "utf-8")
    # do nothing
    ;;

  # subset of utf-8: the file is alreday utf-8 but only uses the first 128 characters
  "us-ascii")
    # do nothing
    ;;

  # Western ISO-8859-1, convert to UTF-8
  "iso-8859-1")
    iconv -f ISO-8859-1 -t UTF-8 $file | sponge $file
    ;;
  
  # other cases ?
  *)
    echo "$file is $encoding"
    ;;
esac

done
