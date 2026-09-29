--TEST--
Tree-sitter: Grammar::getQuery embedded queries
--FILE--
<?php
	use TreeSitter as TS;

	// every query that ships with a grammar can be compiled for that grammar
	$grammars = [
		'PHP' => TS\Grammar::PHP,
		'PHP_ONLY' => TS\Grammar::PHP_ONLY,
		'HTML' => TS\Grammar::HTML,
		'CSS' => TS\Grammar::CSS,
		'JAVASCRIPT' => TS\Grammar::JAVASCRIPT,
		'TYPESCRIPT' => TS\Grammar::TYPESCRIPT,
		'PYTHON' => TS\Grammar::PYTHON,
		'JSON' => TS\Grammar::JSON,
	];

	foreach ($grammars as $label => $grammar) {
		$names = TS\Grammar::getQueryNames($grammar);
		$compiled = 0;

		foreach ($names as $name) {
			new TS\Query($grammar, TS\Grammar::getQuery($grammar, $name));
			$compiled++;
		}

		echo $label, ': ', $compiled, ' of ', count($names), ' compiled', PHP_EOL;
	}

	echo implode(',', array_intersect(['highlights', 'tags', 'injections'], TS\Grammar::getQueryNames(TS\Grammar::PHP_ONLY))), PHP_EOL;
	var_dump(in_array('injections-text', TS\Grammar::getQueryNames(TS\Grammar::PHP)));
	var_dump(in_array('injections-text', TS\Grammar::getQueryNames(TS\Grammar::PHP_ONLY)));
	var_dump(TS\Grammar::getQuery(TS\Grammar::JSON, 'tags'));
	var_dump(str_contains(TS\Grammar::getQuery(TS\Grammar::PHP_ONLY, 'tags'), '@definition.class'));

	foreach ([fn() => TS\Grammar::getQuery(1, 'tags'), fn() => TS\Grammar::getQueryNames(1)] as $check) {
		try {
			$check();
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	// the PHP tags query on real code: what a symbol index is made of
	$source = "<?php\nnamespace A;\nclass Foo extends Bar { const X = 1; public function run(int \$a): string { return \$this->run(1); } }\nfunction top() {}\n";
	$root = (new TS\Parser(TS\Grammar::PHP_ONLY))->parse($source)->getRootNode();

	$cursor = new TS\QueryCursor(new TS\Query(TS\Grammar::PHP_ONLY, TS\Grammar::getQuery(TS\Grammar::PHP_ONLY, 'tags')));
	$cursor->exec($root);

	foreach ($cursor->getMatches() as $match) {
		$name = $match->getNode('name');
		$kind = array_values(array_filter(array_map(fn($capture) => $capture->name, $match->captures), fn($n) => $n !== 'name'))[0];
		echo $kind, ' ', $name->getText(), ' @', $name->getStartPoint()->row, PHP_EOL;
	}
?>
--EXPECT--
PHP: 4 of 4 compiled
PHP_ONLY: 3 of 3 compiled
HTML: 2 of 2 compiled
CSS: 1 of 1 compiled
JAVASCRIPT: 6 of 6 compiled
TYPESCRIPT: 3 of 3 compiled
PYTHON: 2 of 2 compiled
JSON: 1 of 1 compiled
highlights,tags,injections
bool(true)
bool(false)
NULL
bool(true)
Unknown grammar
Unknown grammar
definition.module A @1
definition.class Foo @2
definition.function run @2
reference.call run @2
definition.function top @3
