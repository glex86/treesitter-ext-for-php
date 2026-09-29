--TEST--
Tree-sitter: QueryCursor ranges, iteration and lifetime
--FILE--
<?php
	use TreeSitter as TS;

	$source = "<?php\nfunction foo(\$a) { return bar(\$a); }\nfunction Bar() {}\nclass Baz { function foo() {} }\n";
	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$root = $parser->parse($source)->getRootNode();
	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @fn)');

	$names = fn(array $matches) => implode(',', array_map(fn($match) => $match->getNode('fn')->getText(), $matches));

	// the cursor has to be started first
	$cursor = new TS\QueryCursor($query);
	foreach (['nextMatch', 'nextCapture', 'getMatches', 'getCaptures'] as $method) {
		try {
			$cursor->$method();
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	// iterating one by one
	$cursor->exec($root);
	echo $cursor->nextMatch()->getNode('fn')->getText(), PHP_EOL;
	echo $cursor->nextMatch()->getNode('fn')->getText(), PHP_EOL;
	var_dump($cursor->nextMatch());

	// byte and point ranges
	$cursor->exec($root);
	$cursor->setByteRange(0, 10);
	echo count($cursor->getMatches()), PHP_EOL;
	$cursor->exec($root);
	$cursor->setByteRange(0, strlen($source));
	echo count($cursor->getMatches()), PHP_EOL;
	$cursor->exec($root);
	$cursor->setPointRange(new TS\Point(2, 0), new TS\Point(2, 20));
	echo $names($cursor->getMatches()), PHP_EOL;

	// one query, several cursors
	$a = new TS\QueryCursor($query);
	$b = new TS\QueryCursor($query);
	$a->exec($root);
	$b->exec($root);
	echo $a->nextMatch()->getNode('fn')->getText(), $b->nextMatch()->getNode('fn')->getText(), $a->nextMatch()->getNode('fn')->getText(), PHP_EOL;

	// the cursor keeps the query alive
	unset($query);
	$a->exec($root);
	echo count($a->getMatches()), PHP_EOL;

	// the match limit
	$cursor->setMatchLimit(1);
	var_dump($cursor->didExceedMatchLimit());

	// results outlive the cursor, the query and the tree
	$match = (function () use ($parser) {
		$tree = $parser->parse('<?php function keep() {}');
		$cursor = new TS\QueryCursor(new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @n)'));
		$cursor->exec($tree->getRootNode());

		return $cursor->nextMatch();
	})();
	gc_collect_cycles();
	echo $match->getNode('n')->getText(), ' ', $match->getNode('n')->getParent()->getType(), PHP_EOL;

	// invalid arguments
	$checks = [
		fn() => $cursor->setMatchLimit(0),
		fn() => $cursor->setByteRange(5, 1),
		fn() => $cursor->setPointRange(new TS\Point(3, 0), new TS\Point(1, 0)),
		fn() => new TS\QueryMatch,
		fn() => new TS\QueryCapture,
		fn() => new TS\QueryCursor($parser),
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (Throwable $e) {
			echo get_class($e), ': ', $e->getMessage(), PHP_EOL;
		}
	}
?>
--EXPECT--
exec() has to be called before iterating the matches
exec() has to be called before iterating the captures
exec() has to be called before iterating the matches
exec() has to be called before iterating the captures
foo
Bar
NULL
1
2
Bar
foofooBar
2
bool(false)
keep function_definition
ValueError: TreeSitter\QueryCursor::setMatchLimit(): Argument #1 ($limit) must be between 1 and 4294967295
ValueError: TreeSitter\QueryCursor::setByteRange(): Argument #2 ($end_byte) must be greater than or equal to argument #1 ($start_byte)
ValueError: TreeSitter\QueryCursor::setPointRange(): Argument #2 ($end) must not be before argument #1 ($start)
Error: TreeSitter\QueryMatch class is non-instantiable
Error: TreeSitter\QueryCapture class is non-instantiable
TypeError: TreeSitter\QueryCursor::__construct(): Argument #1 ($query) must be of type TreeSitter\Query, TreeSitter\Parser given
