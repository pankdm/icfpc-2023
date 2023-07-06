#!/bin/bash
set +ex

if ! type "brew" 1>/dev/null; then
    echo "go get Homebrew first:"
    echo " -> https://brew.sh/"
    exit 1
fi

function maybe_brew_install() {
    type $1 1>/dev/null || brew install $1
}

function ask_and_install_deps() {
    read -n1 -p "Install $1 deps? [y,n]" doit
    echo
    case $doit in
    y|Y) $2 ;;
    n|N) ;;
    *) ;;
    esac
}

function install_python_deps() {
    echo ">>>>>> Installing Python deps"
    maybe_brew_install python3
    maybe_brew_install pyenv
    pyenv virtualenv --help 1>/dev/null || brew install pyenv-virtualenv
    bash ./pyenv-init.sh
    echo installed Python deps.
}

function install_cpp_deps() {
    echo ">>>>>> Installing C++ deps"
    maybe_brew_install gcc
    maybe_brew_install g++
    echo installed C++ deps.
}

# UI deps:
function install_ui_deps() {
    echo ">>>>>> Installing UI deps"
    maybe_brew_install node
    maybe_brew_install nvm
    maybe_brew_install pnpm
    cd ./ui
    pnpm i
    cd ..
    echo installed UI deps.
}

ask_and_install_deps Python install_python_deps
ask_and_install_deps C++ install_cpp_deps
ask_and_install_deps UI install_ui_deps
