---
name: build-and-test
description: Build the treesitter PHP extension and run the .phpt tests inside WSL Ubuntu-26.04 (phpize/configure/make), all tests or a single one. Use when asked to build, compile, or run tests.
---

# Build and test (WSL Ubuntu-26.04)

All builds and tests run in the `Ubuntu-26.04` WSL distro, never in Windows/MSYS. The project is at `/mnt/p/LLM/est-treesitter-fresh`. Run commands from Windows via:

```bash
wsl -d Ubuntu-26.04 --cd /mnt/p/LLM/est-treesitter-fresh -- bash -lc '<commands>'
```
(`wsl -l -v` shows the default distro is a different one, so always pass `-d Ubuntu-26.04`.)

Sources: `treesitter.c`, `lib/tree-sitter/lib/src/lib.c`, and the parsers listed in `config.m4`.

## Prerequisites (once, inside the distro)
```bash
sudo apt install php-dev build-essential autoconf libtool pkg-config
```
Submodules must be present (`lib/parsers/*`): `git submodule update --init --recursive`.

## Full build
```bash
phpize --clean && phpize   # required if configure/Makefile were generated elsewhere (e.g. MSYS) or config.m4 changed
./configure
make -j"$(nproc)"
make test
```
The existing `configure`/`Makefile` in the tree may come from a previous environment; if `./configure` or `make` complains about paths or a different PHP, run `phpize --clean` first.

## Single test
```bash
make test TESTS=tests/treesitter_003.phpt
```
Tests are `tests/treesitter_001..006.phpt`. Failures leave `.out/.diff/.exp` files next to the test; read the `.diff` first.

## Quick manual check
```bash
php -d extension=$PWD/modules/treesitter.so example/t1.php
```

## Notes
- Building on `/mnt/p` (NTFS) is slower than in the WSL filesystem; if it's too slow, rsync the tree to `~/est-treesitter` and build there, then sync sources back only if you edited there.
- Line endings: files must be LF; CRLF in `configure`, `config.m4`, or `*.phpt` breaks the build or the tests.

## Reporting
State compiler errors/warnings verbatim, which tests passed/failed, and the `.diff` for failures. If the toolchain is missing, say so rather than guessing.
