#!/usr/bin/env sh
set -e

export JAVA_TOOL_OPTIONS="$(for f in /app/javaopts/*; do cat $f 2>/dev/null |tr "\n" " "; printf ' '; done)"

exec "$@"
