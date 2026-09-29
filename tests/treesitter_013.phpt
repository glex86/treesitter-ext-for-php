--TEST--
Tree-sitter: Query predicates (eq, match, any-of, set, any- and not- variants)
--FILE--
<?php
	use TreeSitter as TS;

	$source = "<?php\nfunction foo() {}\nfunction Árvíz() {}\nfunction b_1() {}\nfunction bar() { return foo(); }\nfoo(); foo(); bar();\n";
	$root = (new TS\Parser(TS\Grammar::PHP_ONLY))->parse($source)->getRootNode();

	function matches(string $query_source, TS\Node $root): void {
		$cursor = new TS\QueryCursor(new TS\Query(TS\Grammar::PHP_ONLY, $query_source));
		$cursor->exec($root);

		$lines = [];
		foreach ($cursor->getMatches() as $match) {
			$captures = array_map(fn($capture) => $capture->name . '=' . $capture->node->getText(), $match->captures);
			$lines[] = '[' . implode(',', $captures) . ']' . ($match->properties ? json_encode($match->properties) : '');
		}
		echo implode(' ', $lines), PHP_EOL;
	}

	$function = '(function_definition name: (name) @fn ';

	echo "eq\n";
	matches($function . '(#eq? @fn "foo"))', $root);
	matches($function . '(#not-eq? @fn "foo"))', $root);

	echo "match (regular expressions, Unicode aware)\n";
	matches($function . '(#match? @fn "^[a-z_0-9]+$"))', $root);
	matches($function . '(#not-match? @fn "^[a-z_0-9]+$"))', $root);
	matches($function . '(#match? @fn "^\\\\p{Lu}"))', $root);
	matches($function . '(#match? @fn "(?i)^ÁRVÍZ$"))', $root);

	echo "any-of\n";
	matches($function . '(#any-of? @fn "foo" "bar"))', $root);
	matches($function . '(#not-any-of? @fn "foo" "bar"))', $root);

	echo "capture against capture\n";
	$nested = '(function_definition name: (name) @n body: (compound_statement (return_statement (function_call_expression function: (name) @c)))';
	matches($nested . ' (#eq? @n @c))', $root);
	matches($nested . ' (#not-eq? @n @c))', $root);

	echo "quantified captures: all nodes or any node\n";
	matches('(program (_)* @all (#eq? @all "foo();"))', $root);
	matches('(program (_)* @all (#any-eq? @all "foo();"))', $root);
	matches('(program (_)* @all (#any-match? @all "^bar"))', $root);
	matches('(program (_)* @all (#not-eq? @all "zzz"))', $root);
	matches('(program (_)* @all (#any-not-eq? @all "foo();"))', $root);
	matches('(program (_)* @all (#any-eq? @all "zzz"))', $root);

	echo "properties\n";
	matches('((function_definition name: (name) @fn) (#set! kind "def") (#set! flag) (#eq? @fn "foo"))', $root);

	echo "ignored predicates\n";
	matches($function . '(#is? local))', $root);
	matches($function . '(#lua-match? @fn "zzz"))', $root);

	echo "predicates apply to the captures too\n";
	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_call_expression function: (name) @f (#eq? @f "foo")) @call');
	$cursor = new TS\QueryCursor($query);
	$cursor->exec($root);
	$lines = [];
	while (($capture = $cursor->nextCapture()) !== null) {
		$lines[] = $capture->name . '=' . $capture->node->getText() . '@' . $capture->index;
	}
	echo implode(' ', $lines), PHP_EOL;

	echo "invalid UTF-8 never matches\n";
	$js = (new TS\Parser(TS\Grammar::JAVASCRIPT))->parse("var a = \"\xff\"; var b = \"ok\"; var c = \"xok!\";")->getRootNode();

	foreach (['#match?', '#not-match?'] as $predicate) {
		$cursor = new TS\QueryCursor(new TS\Query(TS\Grammar::JAVASCRIPT, '((string) @s (' . $predicate . ' @s "^.ok.$"))'));
		$cursor->exec($js);
		echo implode(' ', array_map(fn($match) => bin2hex($match->getNode('s')->getText()), $cursor->getMatches())), PHP_EOL;
	}
?>
--EXPECT--
eq
[fn=foo]
[fn=Árvíz] [fn=b_1] [fn=bar]
match (regular expressions, Unicode aware)
[fn=foo] [fn=b_1] [fn=bar]
[fn=Árvíz]
[fn=Árvíz]
[fn=Árvíz]
any-of
[fn=foo] [fn=bar]
[fn=Árvíz] [fn=b_1]
capture against capture

[n=bar,c=foo]
quantified captures: all nodes or any node

[all=<?php,all=function foo() {},all=function Árvíz() {},all=function b_1() {},all=function bar() { return foo(); },all=foo();,all=foo();,all=bar();]
[all=<?php,all=function foo() {},all=function Árvíz() {},all=function b_1() {},all=function bar() { return foo(); },all=foo();,all=foo();,all=bar();]
[all=<?php,all=function foo() {},all=function Árvíz() {},all=function b_1() {},all=function bar() { return foo(); },all=foo();,all=foo();,all=bar();]
[all=<?php,all=function foo() {},all=function Árvíz() {},all=function b_1() {},all=function bar() { return foo(); },all=foo();,all=foo();,all=bar();]

properties
[fn=foo]{"kind":"def","flag":null}
ignored predicates
[fn=foo] [fn=Árvíz] [fn=b_1] [fn=bar]
[fn=foo] [fn=Árvíz] [fn=b_1] [fn=bar]
predicates apply to the captures too
call=foo()@1 f=foo@0 call=foo()@1 f=foo@0 call=foo()@1 f=foo@0
invalid UTF-8 never matches
226f6b22
22ff22 22786f6b2122
