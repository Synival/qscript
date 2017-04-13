#!/bin/bash
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"

stderr_color() (
   "$@" 2>&1 | sed -r 's/^([^ ]*\ \([0-9]+,[0-9]+\): .*)/\x1b[0;31;1m\1\x1b[0m/g'
)
stderr_color ../../qscript "$@"
