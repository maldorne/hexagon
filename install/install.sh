#!/bin/bash

os=${OSTYPE//[0-9.]/}

cd driver/dgd/src

if [[ $os == darwin ]]
then 
  sed -i '' '968,974 s/^/\/\//' ./kfun/file.cpp
  sed -i '' -e 's/# -DSLASHSLASH/-DSLASHSLASH #/g' Makefile
else
  sed -i '968,974 s/^/\/\//' ./kfun/file.cpp
  sed -i -e 's/# -DSLASHSLASH/-DSLASHSLASH #/g' Makefile
fi

make
make install
cd ../../..

if [ ! -f driver/config.dgd ]; 
then
  cp driver/config.example.dgd driver/config.dgd
fi

if [ ! -d driver/dgd/tmp ]; 
then
  mkdir driver/dgd/tmp
fi
