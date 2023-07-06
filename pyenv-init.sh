#!/bin/bash

pyenv versions | grep 3.10.6 1>/dev/null || pyenv install 3.10.6
echo '-> Creating pyenv'
pyenv virtualenv 3.10.6 icfpc-2023

echo '-> Exporting "icfpc-2023" to .python-version'
echo icfpc-2023 > .python-version

echo '-> Updating pip'
pip install --upgrade pip
