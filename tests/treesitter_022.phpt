--TEST--
Tree-sitter: included ranges validation and incremental parsing
--ENV--
MALLOC_PERTURB_=165
USE_ZEND_ALLOC=0
--FILE--
<?php
	use TreeSitter as TS;

	$range = fn(int $start, int $end) => new TS\Range($start, $end, new TS\Point(0, $start), new TS\Point(0, $end));
	$show = fn(array $ranges) => implode(' ', array_map(fn(TS\Range $r) => $r->startByte . '-' . $r->endByte, $ranges));

	// invalid ranges
	$parser = new TS\Parser(TS\Grammar::HTML);
	$checks = [
		fn() => $parser->setIncludedRanges([$range(10, 20), $range(0, 5)]),
		fn() => $parser->setIncludedRanges([$range(0, 10), $range(5, 15)]),
		fn() => $parser->setIncludedRanges([$range(0, 10), 'x']),
		function () use ($parser, $range) {
			$broken = $range(0, 5);
			$broken->endByte = 1;
			$broken->startByte = 3;
			$parser->setIncludedRanges([$broken]);
		},
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (Throwable $e) {
			echo get_class($e), ': ', $e->getMessage(), PHP_EOL;
		}
	}

	// after a rejected call the previous ranges are kept, an empty list removes them
	$parser->setIncludedRanges([$range(2, 4)]);
	try {
		$parser->setIncludedRanges([$range(9, 10), $range(0, 1)]);
	} catch (ValueError $e) {
	}
	echo $show($parser->getIncludedRanges()), PHP_EOL;

	$parser->setIncludedRanges([]);
	$all = $parser->getIncludedRanges();
	echo count($all), ' ', $all[0]->startByte, '-', $all[0]->endByte, PHP_EOL;

	// incremental parsing of a document with included ranges: after the edit the parser needs the shifted ranges
	$text = '<p>a</p>X<p>b</p>';
	$new = '<p>zz</p>X<p>b</p>';

	$parser->setIncludedRanges([$range(0, 8), $range(9, 17)]);
	$tree = $parser->parseWithErrors($text);
	echo 'tree: ', $show($tree->getIncludedRanges()), PHP_EOL;

	$tree->edit(TS\InputEdit::fromContentChange($text, 0, 3, 0, 4, 'zz'));
	echo 'edited tree: ', $show($tree->getIncludedRanges()), PHP_EOL;

	$fresh = new TS\Parser(TS\Grammar::HTML);
	$fresh->setIncludedRanges([$range(0, 9), $range(10, 18)]);
	$expected = (string) $fresh->parseWithErrors($new)->getRootNode();

	$parser->setIncludedRanges($tree->getIncludedRanges());
	$incremental = $parser->parseWithErrors($new, $tree);
	echo 'new tree: ', $show($incremental->getIncludedRanges()), PHP_EOL;
	var_dump((string) $incremental->getRootNode() === $expected, $incremental->getRootNode()->hasError());

	// with the ranges from before the edit the result is wrong
	$parser->setIncludedRanges([$range(0, 8), $range(9, 17)]);
	$stale = $parser->parseWithErrors($new, $tree);
	var_dump($stale->getRootNode()->hasError());
?>
--EXPECT--
ValueError: TreeSitter\Parser::setIncludedRanges(): Argument #1 ($ranges) must be sorted by position and must not overlap
ValueError: TreeSitter\Parser::setIncludedRanges(): Argument #1 ($ranges) must be sorted by position and must not overlap
TypeError: TreeSitter\Parser::setIncludedRanges(): Argument #1 ($ranges) must be a list of TreeSitter\Range objects
ValueError: Invalid Range: the bytes must be between 0 and 4294967295 and the end must not be before the start
2-4
1 0-4294967295
tree: 0-8 9-17
edited tree: 0-9 10-18
new tree: 0-9 10-18
bool(true)
bool(false)
bool(true)
