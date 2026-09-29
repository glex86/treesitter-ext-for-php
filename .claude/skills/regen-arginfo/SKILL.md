---
name: regen-arginfo
description: Regenerate treesitter_arginfo.h from treesitter.stub.php and verify the extension still builds. Use after any change to treesitter.stub.php.
disable-model-invocation: false
---

# Regenerate arginfo

`treesitter_arginfo.h` is generated; never edit it by hand (a hook blocks it).

1. Run inside WSL from the repo root:
   ```bash
   wsl -d Ubuntu-26.04 --cd /mnt/p/LLM/est-treesitter-fresh -- bash -lc 'php build/gen_stub.php treesitter.stub.php'
   ```
   (`build/gen_stub.php` needs the `nikic/php-parser` package; if it fails on that, report it rather than guessing. Always pass `-d Ubuntu-26.04`: the default WSL distro is a different one.)
2. Review the diff of `treesitter_arginfo.h`: the `Stub hash` line must change, and new/changed methods must appear as `ZEND_BEGIN_ARG_*` blocks.
3. Make sure `treesitter.c` implements every `ZEND_METHOD`/`PHP_METHOD` listed in the header's method table, with matching signatures and return types.
4. Rebuild and test with the `/build-and-test` skill (WSL).
5. Report which methods were added/changed/removed.
