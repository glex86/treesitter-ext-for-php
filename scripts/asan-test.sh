#!/bin/bash
#
# Builds a copy of the extension with AddressSanitizer and LeakSanitizer and runs the tests with it.
# Catches use-after-free, buffer overflows and leaks that the plain tests do not show.
#
#     bash scripts/asan-test.sh
#
# Run it on Linux (or WSL) from anywhere. It works in $HOME/ts-asan, the source tree is not touched,
# and it takes a few minutes because the bundled grammars are big. Exit code 0 means clean.

set -u

SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DST="${TS_ASAN_DIR:-$HOME/ts-asan}"

rm -rf "$DST"
mkdir -p "$DST"

cd "$SRC"
tar --exclude=./modules --exclude=./.libs --exclude='*.lo' --exclude='*.o' --exclude='*.la' \
    --exclude=./autom4te.cache --exclude=./.claude --exclude=./.codegraph --exclude=./example \
    -cf - . | tar -xf - -C "$DST"

cd "$DST"
rm -f Makefile Makefile.objects Makefile.fragments configure config.status config.log config.h libtool

phpize > /dev/null 2>&1

./configure CFLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer" LDFLAGS="-fsanitize=address" > configure.log 2>&1 \
    || { echo "configure failed:"; tail -20 configure.log; exit 2; }

make -j"$(nproc)" > make.log 2>&1 \
    || { echo "make failed:"; grep -E "error" make.log | head; exit 2; }

echo "built $DST/modules/treesitter.so"

# php itself is not built with the sanitizer, so the runtime is preloaded, only into the test run
# (a leak in a helper like head or grep would be reported as well otherwise)
ASAN_LIB="$(gcc -print-file-name=libasan.so)"

ASAN_OPTIONS=detect_leaks=1:abort_on_error=0 \
LD_PRELOAD="$ASAN_LIB" \
USE_ZEND_ALLOC=0 \
NO_INTERACTION=1 \
TEST_PHP_ARGS="-n" \
    make test TESTS=tests/ > test.log 2>&1

grep -E "^(FAIL|LEAK)|Tests (passed|failed|leaked)" test.log

if grep -q "FAIL\|Tests failed *: *[1-9]" test.log || grep -lq "AddressSanitizer\|LeakSanitizer" tests/*.out tests/*.log 2>/dev/null; then
    echo "sanitizer or test failures, see $DST/test.log and $DST/tests/*.out"
    exit 1
fi

echo "clean: no test failed and the sanitizers reported nothing"
