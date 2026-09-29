--TEST--
Tree-sitter: Language and UTF-16 column conversion
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$language = $parser->getLanguage();

	var_dump($language instanceof TS\Language);
	var_dump($language->getAbiVersion() > 0, $language->getSymbolCount() > 100);
	var_dump($language->getSymbolName(999999), $language->getFieldName(0));

	$function = $language->getSymbolForName('function_definition', true);
	echo $language->getSymbolName($function), PHP_EOL;

	$root = $parser->parse('<?php function f() {}')->getRootNode();
	var_dump($root->getNamedChild(1)->getKindId() === $function);

	echo get_class(TS\Language::fromGrammar(TS\Grammar::JSON)), PHP_EOL;

	foreach ([fn() => new TS\Language, fn() => TS\Language::fromGrammar(1)] as $check) {
		try {
			$check();
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	// UTF-16 <-> byte columns: "á" is 2 bytes/1 unit, the emoji is 4 bytes/2 units
	$tree = $parser->parse("<?php\n\$s = 'á😀b'; // ő\n");

	foreach ([0, 6, 8, 12, 13, 14, 21] as $byteColumn) {
		echo $byteColumn, '=>', $tree->byteToUtf16Column(1, $byteColumn), ' ';
	}
	echo PHP_EOL;

	foreach ([0, 6, 7, 8, 9, 10, 11] as $unitColumn) {
		echo $unitColumn, '=>', $tree->utf16ToByteColumn(1, $unitColumn), ' ';
	}
	echo PHP_EOL;

	echo $tree->byteToUtf16Column(2, 0), PHP_EOL;

	$checks = [
		fn() => $tree->byteToUtf16Column(9, 0),
		fn() => $tree->byteToUtf16Column(1, 999),
		fn() => $tree->utf16ToByteColumn(1, 999),
		fn() => $tree->utf16ToByteColumn(-1, 0),
	];

	foreach ($checks as $check) {
		try {
			$check();
		} catch (ValueError $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
NULL
NULL
function_definition
bool(true)
TreeSitter\Language
TreeSitter\Language class is non-instantiable
Unknown grammar
0=>0 6=>6 8=>7 12=>9 13=>10 14=>11 21=>17 
0=>0 6=>6 7=>8 8=>12 9=>12 10=>13 11=>14 
0
TreeSitter\Tree::byteToUtf16Column(): Argument #1 ($row) must be an existing row of the source
TreeSitter\Tree::byteToUtf16Column(): Argument #2 ($byte_column) must be within the row
TreeSitter\Tree::utf16ToByteColumn(): Argument #2 ($utf16_column) must be within the row
TreeSitter\Tree::utf16ToByteColumn(): Argument #1 ($row) must be an existing row of the source
