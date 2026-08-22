#!/usr/bin/env bash
set -e

if ! command -v g++ >/dev/null 2>&1 || ! command -v make >/dev/null 2>&1; then
    echo "C++ build tools not found. Installing them..."
    sudo apt update
    sudo apt install -y build-essential g++ make
fi

echo "Building MiniFS..."
make

echo "Starting MiniFS..."
./minifs
