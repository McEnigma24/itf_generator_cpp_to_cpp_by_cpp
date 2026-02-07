#!/bin/bash
set -euo pipefail

DIRS=(
    _core
    _libs
)

ALL_FILES=()
for dir in "${DIRS[@]}"; do
    mapfile -t FILES < <(find "${dir}" -type f \( -iname '*.c' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.hpp' \) | sort)
    # mapfile -t FILES < <(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|cpp|h|hpp)$')
    if ((${#FILES[@]} > 0)); then
        ALL_FILES+=("${FILES[@]}")
    fi
done

if [ ${#ALL_FILES[@]} -eq 0 ]; then
    echo "Brak plików do formatowania."
    exit 0
fi

# wypłaszczanie tablicy tak aby jeden plik to jeden element i iteracja działa poprawnie #
IFS=$'\n' read -r -d '' -a ALL_FILES < <(printf '%s\n' "${ALL_FILES[@]}" | sort -u && printf '\0')

for FILE in "${ALL_FILES[@]}"; do
    echo "${FILE}"
    clang-format -i -style="file:.clang-format" "${FILE}"
done