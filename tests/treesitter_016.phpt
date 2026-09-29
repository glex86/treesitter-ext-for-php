--TEST--
Tree-sitter: QueryCursor re-initialization, numeric #set! keys
--FILE--
<?php
	use TreeSitter as TS;

	$root = (new TS\Parser(TS\Grammar::PHP_ONLY))->parse('<?php foo();')->getRootNode();

	// a cursor cannot be pointed at another query after it was created
	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(name) @n');
	$cursor = new TS\QueryCursor($query);
	$cursor->exec($root);

	try {
		$cursor->__construct(new TS\Query(TS\Grammar::PHP_ONLY, '(name) @other'));
	} catch (Error $e) {
		echo $e->getMessage(), PHP_EOL;
	}

	unset($query);
	echo count($cursor->getMatches()), ' ', $cursor->nextMatch(), PHP_EOL;

	// numeric #set! keys are real integer keys
	$query = new TS\Query(TS\Grammar::PHP_ONLY, '((name) @n (#set! "1" "one") (#set! "a" "b") (#set! "007" "string"))');
	$cursor = new TS\QueryCursor($query);
	$cursor->exec($root);
	$properties = $cursor->nextMatch()->properties;

	var_dump($properties[1] ?? null, isset($properties['1']), $properties['a'], $properties['007']);
	echo json_encode(array_keys($properties)), PHP_EOL;

	// a query with a broken predicate is not created at all
	try {
		new TS\Query(TS\Grammar::PHP_ONLY, '((name) @n (#eq? @n))');
	} catch (TS\QueryException $e) {
		echo $e->errorType, PHP_EOL;
	}
?>
--EXPECT--
The query cursor is already initialized
1 
string(3) "one"
bool(true)
string(1) "b"
string(6) "string"
[1,"a","007"]
predicate
