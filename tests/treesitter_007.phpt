--TEST--
Tree-sitter: Grammar::PHP vs Grammar::PHP_ONLY
--FILE--
<?php
	foreach ([TreeSitter\Grammar::PHP, TreeSitter\Grammar::PHP_ONLY] as $grammar) {
		$parser = new TreeSitter\Parser($grammar);
		echo $parser->parse('<?php test();')->getRootNode() . PHP_EOL;
		echo $parser->parse('test();')->getRootNode() . PHP_EOL;
	}
?>
--EXPECT--
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
(program (text))
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
(program (expression_statement (function_call_expression function: (name) arguments: (arguments))))
