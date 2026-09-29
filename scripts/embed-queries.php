<?php

/**
 * Embeds the query files (queries/*.scm) of the bundled grammars into
 * lib/parsers/queries.gen.h, so Grammar::getQuery() does not depend on where
 * the extension is installed.
 *
 * Run it after the grammar submodules were updated:
 *
 *     php scripts/embed-queries.php
 *
 * The header is checked in, the build does not need PHP or this script.
 */

$root = dirname(__DIR__);

/* grammar constant (see lib/parsers/parsers.h) => directory of the grammar below lib/parsers */
$grammars = [
	'PHP_TREESITTER_GRAMMAR_PHP'        => 'tree-sitter-php',
	'PHP_TREESITTER_GRAMMAR_PHP_ONLY'   => 'tree-sitter-php',
	'PHP_TREESITTER_GRAMMAR_HTML'       => 'tree-sitter-html',
	'PHP_TREESITTER_GRAMMAR_CSS'        => 'tree-sitter-css',
	'PHP_TREESITTER_GRAMMAR_JAVASCRIPT' => 'tree-sitter-javascript',
	'PHP_TREESITTER_GRAMMAR_TYPESCRIPT' => 'tree-sitter-typescript',
	'PHP_TREESITTER_GRAMMAR_PYTHON'     => 'tree-sitter-python',
	'PHP_TREESITTER_GRAMMAR_JSON'       => 'tree-sitter-json',
];

function c_string_literal(string $text): string
{
	$text = str_replace("\r\n", "\n", $text);
	$lines = explode("\n", $text);
	$last = array_key_last($lines);
	$literal = [];

	foreach ($lines as $index => $line) {
		if ($index === $last && $line === '') {
			break;
		}

		$escaped = addcslashes($line, "\\\"");
		$escaped = str_replace('??', '?\\?', $escaped); // no trigraphs
		$literal[] = "\t\"" . $escaped . ($index === $last ? '' : '\\n') . '"';
	}

	return $literal ? implode("\n", $literal) : "\t\"\"";
}

$arrays = [];
$entries = [];

foreach ($grammars as $constant => $directory) {
	$queries = glob($root . '/lib/parsers/' . $directory . '/queries/*.scm');
	sort($queries);

	foreach ($queries as $file) {
		$name = basename($file, '.scm');

		// the HTML text injections only make sense for the grammar that mixes PHP with text
		if ($constant === 'PHP_TREESITTER_GRAMMAR_PHP_ONLY' && $name === 'injections-text') {
			continue;
		}
		$identifier = 'php_treesitter_query_' . strtolower(str_replace('-', '_', $directory)) . '_' . str_replace('-', '_', $name);

		if (!isset($arrays[$identifier])) {
			$source = file_get_contents($file);

			if ($source === false) {
				fwrite(STDERR, "Cannot read $file\n");
				exit(1);
			}

			$arrays[$identifier] = 'static const char ' . $identifier . "[] =\n" . c_string_literal($source) . ";\n";
		}

		$entries[] = sprintf("\t{ %s, \"%s\", %s, sizeof(%s) - 1 },", $constant, $name, $identifier, $identifier);
	}
}

$header = "/* This is a generated file, run scripts/embed-queries.php instead of editing it. */\n\n"
	. "#ifndef PHP_TREESITTER_QUERIES_GEN_H\n#define PHP_TREESITTER_QUERIES_GEN_H\n\n"
	. "#include \"parsers.h\"\n\n"
	. "typedef struct {\n\tzend_long grammar;\n\tconst char *name;\n\tconst char *source;\n\tsize_t length;\n} php_treesitter_embedded_query;\n\n"
	. implode("\n", $arrays)
	. "\nstatic const php_treesitter_embedded_query php_treesitter_embedded_queries[] = {\n"
	. implode("\n", $entries)
	. "\n};\n\n#endif /* PHP_TREESITTER_QUERIES_GEN_H */\n";

file_put_contents($root . '/lib/parsers/queries.gen.h', $header);

echo count($arrays), " query files embedded, ", count($entries), " grammar/query pairs\n";
