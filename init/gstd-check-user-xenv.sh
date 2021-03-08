#!/bin/sh

if [ -z "$DISPLAY" ] || [ -z "$XAUTHORITY" ]
then
    exit 1
fi

systemctl --user import-environment DISPLAY XAUTHORITY
