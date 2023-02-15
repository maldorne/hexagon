#!/bin/bash

# list all files but .git and assets directories
# find . \( -path ./.git -prune -o -path ./assets -prune \) -o -print -execdir file -b --mime-encoding {} \;

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
