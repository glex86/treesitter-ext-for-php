--TEST--
Tree-sitter: strict parse of deeply nested invalid source does not exhaust the C stack
--FILE--
<?php
	$parser = new TreeSitter\Parser(TreeSitter\Grammar::JSON);

	try {
		$parser->parse(str_repeat('[', 100000));
		echo 'no exception', PHP_EOL;
	} catch (TreeSitter\SyntaxError $e) {
		echo 'SyntaxError', PHP_EOL;
	}

	$root = $parser->parseWithErrors(str_repeat('[', 100000))->getRootNode();
	var_dump($root->hasError());

	try {
		new TreeSitter\Range(0, 4294967296, new TreeSitter\Point(0, 0), new TreeSitter\Point(0, 0));
	} catch (ValueError $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
SyntaxError
bool(true)
TreeSitter\Range::__construct(): Argument #2 ($end_byte) must be between 0 and 4294967295
