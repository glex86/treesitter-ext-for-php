--TEST--
Tree-sitter: parser timeout and reset
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	var_dump($parser->getTimeoutMicros());

	$big = "<?php\n" . str_repeat("function f() { return [1,2,3,4,5,6,7,8,9,10]; }\n", 20000);

	// a parse that takes longer than the timeout is cancelled
	$parser->setTimeoutMicros(1);
	var_dump($parser->getTimeoutMicros());

	try {
		$parser->parse($big);
		echo 'no timeout', PHP_EOL;
	} catch (TS\Exception $e) {
		echo get_class($e), ': ', $e->getMessage(), PHP_EOL;
	}

	// the cancelled parse does not leak into the next one
	$parser->setTimeoutMicros(0);
	echo $parser->parse('<?php f();')->getRootNode(), PHP_EOL;

	$parser->setTimeoutMicros(60000000);
	echo $parser->parse("<?php\nf();")->getRootNode(), PHP_EOL;

	// works together with an old tree
	$old = "<?php\nf();\n";
	$tree = $parser->parse($old);
	$tree->edit(TS\InputEdit::fromContentChange($old, 1, 0, 1, 0, 'g(); '));
	echo $parser->parse("<?php\ng(); f();\n", $tree)->getRootNode(), PHP_EOL;

	try {
		$parser->setTimeoutMicros(-1);
	} catch (ValueError $e) {
		echo $e->getMessage(), PHP_EOL;
	}

	$parser->reset();
	echo 'reset', PHP_EOL;
?>
--EXPECT--
int(0)
int(1)
TreeSitter\Exception: Parsing was cancelled, it took longer than 1 microseconds
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
TreeSitter\Parser::setTimeoutMicros(): Argument #1 ($timeout) must not be negative
reset
