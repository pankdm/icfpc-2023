#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
    # if macos
    exec bash ./install_macos.sh $1
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Get out of here"
    exit 1337
else
    echo "Who are you?"
    exit 1
fi
