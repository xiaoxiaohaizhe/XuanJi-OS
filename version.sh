#!/bin/bash

COUNT=$(git rev-list --count HEAD 2>/dev/null || echo "0")
N=$((COUNT + 1))
X=$((N / 100 + 1))
Y=$(printf "%02d" $((N % 100)))
VERSION="v${X}.${Y}.0"

cat > kernel/include/version.h << EOF2
#ifndef VERSION_H
#define VERSION_H
#define VERSION "$VERSION"
#endif
EOF2

echo "✓ Version: $VERSION"
