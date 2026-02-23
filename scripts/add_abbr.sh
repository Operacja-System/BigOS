#!/usr/bin/env bash

ABBR="$1"
EXP="$2"

if [ "$#" -ne 2 ]; then
	echo "Usage: ./add_abbr.sh ABBR EXP"
	exit 1
fi

GIT_ROOT=$(git rev-parse --show-toplevel 2>/dev/null)

if [ -z "$GIT_ROOT" ]; then
    echo "Not inside a Git repository."
	exit 1
fi

python3 "$GIT_ROOT/scripts/internal/add_abbr.py" "$GIT_ROOT/docs/source/pages/bigos/general/abbreviations.rst" "$ABBR" "$EXP"
