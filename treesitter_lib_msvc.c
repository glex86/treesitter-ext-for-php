/*
 * tree-sitter as one translation unit for Visual Studio.
 *
 * tree-sitter uses _Static_assert, which the C compiler of Visual Studio only knows in C11 mode
 * (/std:c11). The headers of PHP 8.4 do not compile in that mode (zend_max_align_t), so the mode cannot
 * be switched on for the extension. This file supplies _Static_assert for the tree-sitter sources only,
 * with the typedef of an array whose size is negative when the condition is false.
 *
 * Other compilers include lib/tree-sitter/lib/src/lib.c directly (see config.m4), this file is for
 * config.w32.
 */

#ifdef _MSC_VER

#define TREESITTER_CONCAT_(a, b) a##b
#define TREESITTER_CONCAT(a, b) TREESITTER_CONCAT_(a, b)

#ifndef _Static_assert
# define _Static_assert(condition, message) \
    typedef char TREESITTER_CONCAT(treesitter_static_assert_, __COUNTER__)[(condition) ? 1 : -1]
#endif

#endif

#include "lib/tree-sitter/lib/src/lib.c"
