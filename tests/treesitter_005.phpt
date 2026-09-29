--TEST--
Tree-sitter: Node text, fields, descendants and Tree lifetime
--FILE--
<?php
	// A Node must keep its Tree alive
	$root = (new TreeSitter\Parser(TreeSitter\Grammar::JSON))->parse("[1]")->getRootNode();
	gc_collect_cycles();
	echo $root->getType(), ' ', $root->getText(), PHP_EOL;

	$parser = new TreeSitter\Parser(TreeSitter\Grammar::PHP);
	$tree = $parser->parse('<?php function foo($a) { return "é"; }');
	echo $tree->getSource(), PHP_EOL;

	$function = $tree->getRootNode()->getChild(1);
	echo $function->getType(), PHP_EOL;

	echo $function->getChildByFieldName('name')->getText(), PHP_EOL;
	echo $function->getChildByFieldName('body')->getType(), PHP_EOL;
	var_dump($function->getChildByFieldName('nope'));

	$fields = [];
	foreach ($function->getChildren() as $i => $child) {
		$fields[] = $i . '=' . var_export($function->getFieldNameForChild($i), true);
	}
	echo implode(' ', $fields), PHP_EOL;
	var_dump($function->getFieldNameForChild(99));

	// Byte based, not character based: "é" is two bytes
	$body = $function->getChildByFieldName('body');
	echo $body->getText(), ' ', strlen($body->getText()), PHP_EOL;

	echo $tree->getRootNode()->descendantForByteRange(15, 18)->getType(), PHP_EOL;
	echo $tree->getRootNode()->namedDescendantForByteRange(15, 18)->getType(), PHP_EOL;

	try {
		$tree->getRootNode()->descendantForByteRange(5, 1);
	} catch (ValueError $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
document [1]
<?php function foo($a) { return "é"; }
function_definition
foo
compound_statement
NULL
0=NULL 1='name' 2='parameters' 3='body'
NULL
{ return "é"; } 16
name
name
TreeSitter\Node::descendantForByteRange(): Argument #2 ($end_byte) must be greater than or equal to argument #1 ($start_byte)
