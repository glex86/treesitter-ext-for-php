--TEST--
Tree-sitter: TreeCursor traversal
--FILE--
<?php
	$parser = new TreeSitter\Parser(TreeSitter\Grammar::JSON);
	$tree = $parser->parse('{"a": [1, null]}');

	// Pre-order depth-first walk with the cursor
	function walkCursor(TreeSitter\TreeCursor $cursor): array {
		$types = [];

		while (true) {
			$types[] = $cursor->getCurrentNode()->getType();

			if ($cursor->gotoFirstChild()) {
				continue;
			}

			while (!$cursor->gotoNextSibling()) {
				if (!$cursor->gotoParent()) {
					return $types;
				}
			}
		}
	}

	// Same walk using recursion over getChildren()
	function walkRecursive(TreeSitter\Node $node): array {
		$types = [$node->getType()];

		foreach ($node->getChildren() as $child) {
			array_push($types, ...walkRecursive($child));
		}

		return $types;
	}

	$cursorTypes = walkCursor($tree->walk());
	var_dump($cursorTypes === walkRecursive($tree->getRootNode()));
	var_dump($cursorTypes === walkCursor($tree->getRootNode()->walk()));
	echo implode(' ', $cursorTypes), PHP_EOL;

	$cursor = $tree->walk();
	echo $cursor->getDepth(), ' ', var_export($cursor->getCurrentFieldName(), true), PHP_EOL;

	var_dump($cursor->gotoFirstChild()); // object
	var_dump($cursor->gotoFirstChild()); // {
	var_dump($cursor->gotoNextSibling()); // pair
	echo $cursor->getCurrentNode()->getType(), ' ', $cursor->getDepth(), PHP_EOL;

	var_dump($cursor->gotoFirstChild()); // key
	echo var_export($cursor->getCurrentFieldName(), true), PHP_EOL;

	var_dump($cursor->gotoParent());
	var_dump($cursor->gotoLastChild());
	echo $cursor->getCurrentNode()->getType(), PHP_EOL;
	var_dump($cursor->gotoPreviousSibling());
	echo $cursor->getCurrentNode()->getType(), PHP_EOL;

	// Reset to another node
	$cursor->reset($tree->getRootNode());
	echo $cursor->getCurrentNode()->getType(), ' ', $cursor->getDepth(), PHP_EOL;

	// The tree is kept alive by the cursor
	$cursor = (new TreeSitter\Parser(TreeSitter\Grammar::JSON))->parse('[]')->walk();
	gc_collect_cycles();
	echo $cursor->getCurrentNode()->getType(), PHP_EOL;

	try {
		new TreeSitter\TreeCursor;
	} catch (Error $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
bool(true)
bool(true)
document object { pair string " string_content " : array [ number , null ] }
0 NULL
bool(true)
bool(true)
bool(true)
pair 2
bool(true)
'key'
bool(true)
bool(true)
array
bool(true)
:
document 0
document
TreeSitter\TreeCursor class is non-instantiable
