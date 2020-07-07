#!/usr/bin/env bash

# Created by RidgeRun, 2020
#
# The software contained in this file is free and unencumbered software
# released into the public domain. Anyone is free to use the software contained
# in this file as they choose, including incorporating it into proprietary
# software.

# Default values
ADDRESS="0.0.0.0"
PORT="8000"

function usage {
  echo 'execute:'
  echo './setup_http_server.sh -a $ADDRESS -p $PORT'
}

while [ "$1" != "" ]; do
  case $1 in 
    -a|--address )  shift
                    ADDRESS="${1}"
                    ;;
    -p|--port )     shift
                    PORT="${1}"
                    ;;
    -h|--help )     usage
                    exit 0
                    ;;
    * )
                    echo 'Unknown option'
                    usage
                    exit 1
  esac
  shift
done

if command -v python3 >/dev/null 2>&1;
then
  python3 -m http.server ${PORT} --bind ${ADDRESS}
elif command -v python2 >/dev/null 2>&1;
then
  python -m SimpleHTTPServer ${PORT}
else
  echo Could not detect any Python installation
fi

