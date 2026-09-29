--TEST--
Tree-sitter: Node navigation (children, parent, siblings)
--FILE--
<?php
	$parser = new TreeSitter\Parser(TreeSitter\Grammar::JSON);
	$tree = $parser->parse("[1, null]");
	$root = $tree->getRootNode();
	$array = $root->getChild(0);

	echo $array->getType(), ' ', $array->countChildren(), ' ', $array->countNamedChildren(), PHP_EOL;

	foreach ($array->getChildren() as $i => $child) {
		echo $i, ':', $child->getType(), ($child->isNamed() ? ' (named)' : ''), PHP_EOL;
	}

	echo implode(',', array_map(fn($node) => $node->getType(), $array->getNamedChildren())), PHP_EOL;

	// Out of range indexes yield null
	var_dump($root->getChild(5), $root->getChild(-1), $root->getNamedChild(5));

	// The root has no parent and no siblings
	var_dump($root->getParent(), $root->getNextSibling(), $root->getPrevSibling());

	// Nodes are comparable
	var_dump($array->getParent() == $root);
	var_dump($array->getChild(1) == $array->getNamedChild(0));
	var_dump($array->getChild(1) == $array->getChild(2));

	$number = $array->getNamedChild(0);
	echo $number->getNextSibling()->getType(), PHP_EOL;
	echo $number->getNextNamedSibling()->getType(), PHP_EOL;
	echo $number->getPrevSibling()->getType(), PHP_EOL;
	var_dump($number->getPrevNamedSibling());

	try {
		new TreeSitter\Node;
	} catch (Error $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
array 5 2
0:[
1:number (named)
2:,
3:null (named)
4:]
number,null
NULL
NULL
NULL
NULL
NULL
NULL
bool(true)
bool(true)
bool(false)
,
null
[
NULL
TreeSitter\Node class is non-instantiable
