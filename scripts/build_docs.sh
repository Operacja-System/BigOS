#!/usr/bin/env bash

cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GIT_ROOT=$(git rev-parse --show-toplevel 2>/dev/null)

if [ -z "$GIT_ROOT" ]; then
    echo "Not inside a Git repository."
	exit 1
fi

cd "$GIT_ROOT"

doxygen docs/Doxyfile && make -C docs html
