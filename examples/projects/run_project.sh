#!/bin/bash
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"/$1
if [ ! -f "$1" ]; then
   make
   output=$?
   if [ $output -ne 0 ]; then
      exit 1
   fi
fi
"./$@"
