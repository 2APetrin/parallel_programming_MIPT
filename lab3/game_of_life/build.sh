#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cmake -B $SCRIPT_DIR/build
cd $SCRIPT_DIR/build
make