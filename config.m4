AC_DEFINE(HAVE_TREESITTER, 1, [ Have treesitter ])

dnl Versions of the bundled grammars for the module info, from the tree-sitter.json of each of them.
dnl Keep the names in sync with PHP_MINFO_FUNCTION in treesitter.c.
ts_grammar_version() {
  grep -m1 '"version"' "$srcdir/lib/parsers/$1/tree-sitter.json" | cut -d'"' -f4
}
AC_DEFINE_UNQUOTED(TS_VERSION_PHP, ["`ts_grammar_version tree-sitter-php`"], [Version of the PHP grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_HTML, ["`ts_grammar_version tree-sitter-html`"], [Version of the HTML grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_CSS, ["`ts_grammar_version tree-sitter-css`"], [Version of the CSS grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_JAVASCRIPT, ["`ts_grammar_version tree-sitter-javascript`"], [Version of the JavaScript grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_TYPESCRIPT, ["`ts_grammar_version tree-sitter-typescript`"], [Version of the TypeScript grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_PYTHON, ["`ts_grammar_version tree-sitter-python`"], [Version of the Python grammar])
AC_DEFINE_UNQUOTED(TS_VERSION_JSON, ["`ts_grammar_version tree-sitter-json`"], [Version of the JSON grammar])

TS_CFLAGS="$TS_CFLAGS -Ilib/tree-sitter/lib/include/ -Ilib/tree-sitter/lib/src/"

TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-php/php/src/parser.c lib/parsers/tree-sitter-php/php/src/scanner.c"
TS_CFLAGS="$TS_CFLAGS -Ilib/parsers/tree-sitter-php/php/src/"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-php/php_only/src/parser.c lib/parsers/tree-sitter-php/php_only/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-html/src/parser.c lib/parsers/tree-sitter-html/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-css/src/parser.c lib/parsers/tree-sitter-css/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-javascript/src/parser.c lib/parsers/tree-sitter-javascript/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-typescript/typescript/src/parser.c lib/parsers/tree-sitter-typescript/typescript/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-typescript/tsx/src/parser.c lib/parsers/tree-sitter-typescript/tsx/src/scanner.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-json/src/parser.c"
TS_PARSERS="$TS_PARSERS lib/parsers/tree-sitter-python/src/parser.c lib/parsers/tree-sitter-python/src/scanner.c"

PHP_NEW_EXTENSION(treesitter, treesitter.c lib/tree-sitter/lib/src/lib.c $TS_PARSERS, yes,, [$TS_CFLAGS])
