--TEST--
Tree-sitter: Query and QueryCursor basics, QueryException
--FILE--
<?php
	use TreeSitter as TS;

	$source = "<?php\nfunction foo(\$a) { return bar(\$a); }\nfunction Bar() {}\nclass Baz { function foo() {} }\nfoo(1); bar(2); Baz::x();\n";
	$root = (new TS\Parser(TS\Grammar::PHP_ONLY))->parse($source)->getRootNode();

	function matches(string $query_source, TS\Node $root): void {
		$cursor = new TS\QueryCursor(new TS\Query(TS\Grammar::PHP_ONLY, $query_source));
		$cursor->exec($root);

		$lines = [];
		foreach ($cursor->getMatches() as $match) {
			$captures = array_map(fn($capture) => $capture->name . '=' . $capture->node->getText(), $match->captures);
			$lines[] = '#' . $match->patternIndex . '[' . implode(',', $captures) . ']' . ($match->properties ? json_encode($match->properties) : '');
		}
		echo implode(' ', $lines), PHP_EOL;
	}

	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @fn) @def');
	echo implode(',', $query->getCaptureNames()), ' ', $query->getPatternCount(), PHP_EOL;

	matches('(function_definition name: (name) @fn)', $root);
	matches('(function_call_expression function: (name) @f (arguments (argument (integer) @n)))', $root);
	matches('(compound_statement (_)* @stmts) @body', $root);
	matches('(function_definition name: (name) @fn) (function_call_expression function: (name) @f)', $root);

	// captures in document order
	$cursor = new TS\QueryCursor($query);
	$cursor->exec($root);
	echo implode(' ', array_map(fn($capture) => $capture->name . '@' . $capture->index . '=' . $capture->node->getText(), $cursor->getCaptures())), PHP_EOL;

	// getNode() / getNodes()
	$cursor->exec($root);
	$match = $cursor->nextMatch();
	echo $match->getNode('fn')->getText(), ' ', var_export($match->getNode('nope'), true), ' ', count($match->getNodes('def')), ' ', count($match->getNodes('nope')), PHP_EOL;

	// disabling patterns
	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @fn) (function_call_expression function: (name) @f)');
	$query->disablePattern(1);
	$cursor = new TS\QueryCursor($query);
	$cursor->exec($root);
	echo count($cursor->getMatches()), PHP_EOL;

	// invalid queries
	$invalid = [
		'(function_definition',
		'(no_such_node)',
		'(function_definition nope: (name))',
		"(name)\n(bogus_type)",
		'((name) @y (#eq? @y))',
		'((name) @x (#match? @x "("))',
		'((name) @x (#any-of? @x))',
		'((name) @x (#set!))',
	];

	foreach ($invalid as $source) {
		try {
			new TS\Query(TS\Grammar::PHP_ONLY, $source);
			echo 'no error', PHP_EOL;
		} catch (TS\QueryException $e) {
			echo $e->getMessage(), ' | ', $e->offset, ' ', $e->errorType, PHP_EOL;
			var_dump($e instanceof TS\Exception);
		}
	}

	try {
		new TS\Query(1, '(name)');
	} catch (Error $e) {
		echo $e->getMessage(), PHP_EOL;
	}

	try {
		$query->disablePattern(9);
	} catch (ValueError $e) {
		echo $e->getMessage(), PHP_EOL;
	}
?>
--EXPECT--
fn,def 1
#0[fn=foo] #0[fn=Bar]
#0[f=foo,n=1] #0[f=bar,n=2]
#0[body={ return bar($a); },stmts=return bar($a);] #0[body={}] #0[body={}]
#0[fn=foo] #1[f=bar] #0[fn=Bar] #1[f=foo] #1[f=bar]
def@1=function foo($a) { return bar($a); } fn@0=foo def@1=function Bar() {} fn@0=Bar
foo NULL 1 0
2
Invalid query: syntax error at line 1, column 21 | 20 syntax
bool(true)
Invalid query: unknown node type 'no_such_node' at line 1, column 2 | 1 node_type
bool(true)
Invalid query: unknown field 'nope' at line 1, column 22 | 21 field
bool(true)
Invalid query: unknown node type 'bogus_type' at line 2, column 2 | 8 node_type
bool(true)
Invalid query: the predicate needs a capture and one more argument at line 1, column 1 | 0 predicate
bool(true)
Invalid query: invalid regular expression in a #match? predicate at line 1, column 1 | 0 predicate
bool(true)
Invalid query: the #any-of? predicate needs a capture and at least one string at line 1, column 1 | 0 predicate
bool(true)
Invalid query: the #set! predicate needs a key and an optional value at line 1, column 1 | 0 predicate
bool(true)
TreeSitter\Query::__construct(): Argument #1 ($grammar) must be of type TreeSitter\Grammar, int given
TreeSitter\Query::disablePattern(): Argument #1 ($pattern_index) must be a valid pattern index