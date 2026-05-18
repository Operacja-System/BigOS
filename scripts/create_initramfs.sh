#!/usr/bin/env bash

# usage: ./create_initramfs.sh file1 file2 file3 ... -o outfile.img
# input file names must be shorter then 255 chars
# input files must be smaller then 4GiB
# input files combined size must be smaller then 4GiB

GIT_ROOT=$(git rev-parse --show-toplevel 2>/dev/null)

if [ -z "$GIT_ROOT" ]; then
    echo "Not inside a Git repository."
	exit 1
fi

python3 "$GIT_ROOT/scripts/internal/create_initramfs.py" "$@"
