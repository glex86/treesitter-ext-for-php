--TEST--
Tree-sitter: Grammar enum
--FILE--
<?php
	echo implode(PHP_EOL, array_map(fn($case) => hex2bin(dechex($case->value)), $grammars = TreeSitter\Grammar::cases())) . PHP_EOL;

	try {
		new TreeSitter\Grammar;
		echo '__SENTINEL__';
	} catch (Error $e) {
		echo $e->getMessage(), PHP_EOL;
	}

	foreach ($grammars as $grammar) {
		new TreeSitter\Parser($grammar);
	}

	var_dump(TreeSitter\Grammar::from(0x6a73) === TreeSitter\Grammar::JAVASCRIPT);
	var_dump(TreeSitter\Grammar::tryFrom(1));
	var_dump(TreeSitter\Grammar::PHP_ONLY->name, TreeSitter\Grammar::PHP_ONLY->value === 0x7068705f6f6e6c79);

	try {
		new TreeSitter\Parser(0x6a73);
	} catch (TypeError $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
php
php_only
phpdoc
html
css
js
ts
python
json
Cannot instantiate enum TreeSitter\Grammar
bool(true)
NULL
string(8) "PHP_ONLY"
bool(true)
TreeSitter\Parser::__construct(): Argument #1 ($grammar) must be of type TreeSitter\Grammar, int given