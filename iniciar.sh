#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
if [[ ! -x build/vhs ]] || [[ compilar.sh -nt build/vhs ]] || \
   [[ -n "$(find src -type f \( -name '*.c' -o -name '*.h' \) -newer build/vhs -print -quit 2>/dev/null)" ]]; then
    ./compilar.sh
fi
if [[ $# -eq 0 ]]; then set -- --windowed; fi
exec ./build/vhs "$@"
