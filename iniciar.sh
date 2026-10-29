#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
if [[ ! -x build/vhs || src/main.c -nt build/vhs || compilar.sh -nt build/vhs ]]; then
    ./compilar.sh
fi
if [[ $# -eq 0 ]]; then set -- --windowed; fi
exec ./build/vhs "$@"
