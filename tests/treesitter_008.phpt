--TEST--
Tree-sitter: Parser::parse throws SyntaxError, Parser::parseWithErrors keeps the error tree
--FILE--
<?php
	$parser = new TreeSitter\Parser(TreeSitter\Grammar::PHP_ONLY);

	foreach (['<?php function {', '<?php $a = ;'] as $source) {
		try {
			$parser->parse($source);
			echo 'no exception', PHP_EOL;
		} catch (TreeSitter\SyntaxError $e) {
			echo get_class($e), ': ', $e->getMessage(), PHP_EOL;
			var_dump($e instanceof TreeSitter\Exception, $e instanceof Exception);
		}
	}

	$tree = $parser->parseWithErrors('<?php $a = ;');
	$root = $tree->getRootNode();
	echo $root, PHP_EOL;
	var_dump($root->hasError(), $root->isError());

	$statement = $root->getNamedChild(1);
	$errors = array_filter($statement->getChildren(), fn($node) => $node->isError());
	echo count($errors), ' ', current($errors)->getType(), PHP_EOL;

	// a valid source is parsed the same way by both methods
	echo $parser->parse('<?php f();') == $parser->parseWithErrors('<?php f();') ? 'different objects' : 'not comparable', PHP_EOL;
	echo $parser->parse('<?php f();')->getRootNode(), PHP_EOL;

	// missing nodes are reported as such
	$js = new TreeSitter\Parser(TreeSitter\Grammar::JAVASCRIPT);
	try {
		$js->parse('function f() { return 1;');
	} catch (TreeSitter\SyntaxError $e) {
		echo $e->getMessage(), PHP_EOL;
	}
	$missing = $js->parseWithErrors('function f() { return 1;')->getRootNode()
		->getNamedChild(0)->getNamedChild(0)->getChildByFieldName('body')->getChild(2);
	var_dump($missing->isMissing(), $missing->getType());
?>
--EXPECT--
TreeSitter\SyntaxError: Syntax error at line 1, column 7
bool(true)
bool(true)
TreeSitter\SyntaxError: Syntax error at line 1, column 10
bool(true)
bool(true)
(program (php_tag) (expression_statement (variable_name (name)) (ERROR)))
bool(true)
bool(false)
1 ERROR
not comparable
(program (php_tag) (expression_statement (function_call_expression function: (name) arguments: (arguments))))
Syntax error: missing } at line 1, column 25
bool(true)
string(1) "}"
