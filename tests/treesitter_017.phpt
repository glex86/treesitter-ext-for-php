--TEST--
Tree-sitter: incremental parsing (InputEdit, Tree::edit, changed ranges)
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$fresh = new TS\Parser(TS\Grammar::PHP_ONLY);
	$point = fn(int $row, int $column) => new TS\Point($row, $column);
	$describe = fn(TS\InputEdit $edit) => json_encode([
		$edit->startByte, $edit->oldEndByte, $edit->newEndByte,
		[$edit->startPoint->row, $edit->startPoint->column],
		[$edit->oldEndPoint->row, $edit->oldEndPoint->column],
		[$edit->newEndPoint->row, $edit->newEndPoint->column],
	]);

	// an edit that changes the structure: a statement is inserted
	$old = "<?php\nfunction foo() { return 1; }\n";
	$new = "<?php\nfunction foo() { \$a = 2; return 1; }\n";
	$tree = $parser->parse($old);
	$edit = TS\InputEdit::fromContentChange($old, 1, 17, 1, 17, '$a = 2; ');
	echo $describe($edit), PHP_EOL;

	$backup = $tree->copy();
	$tree->edit($edit);
	$incremental = $parser->parse($new, $tree);

	echo $incremental->getRootNode(), PHP_EOL;
	var_dump((string) $incremental->getRootNode() === (string) $fresh->parse($new)->getRootNode());
	echo $incremental->getRootNode()->getText() === $new ? 'current text' : 'wrong text', PHP_EOL;

	foreach ($incremental->getChangedRanges($tree) as $range) {
		echo 'changed ', $range->startByte, '-', $range->endByte, ' ',
			$range->startPoint->row, ':', $range->startPoint->column, '..', $range->endPoint->row, ':', $range->endPoint->column, PHP_EOL;
	}

	// an edit that keeps the structure changes no ranges
	$same = $parser->parse($old);
	$same->edit(TS\InputEdit::fromContentChange($old, 1, 24, 1, 25, '42'));
	echo count($parser->parse("<?php\nfunction foo() { return 42; }\n", $same)->getChangedRanges($same)), PHP_EOL;

	// the edited tree is stale, the copy made before is not
	echo "stale tree\n";
	$checks = [
		fn() => $tree->getRootNode()->getText(),
		fn() => $tree->byteToUtf16Column(0, 0),
		fn() => $tree->utf16ToByteColumn(0, 0),
		fn() => (new TS\QueryCursor(new TS\Query(TS\Grammar::PHP_ONLY, '(name) @n')))->exec($tree->getRootNode()),
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (Error $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	echo $backup->getRootNode()->getText() === $old ? 'copy intact' : 'copy broken', PHP_EOL;

	// what parse() and getChangedRanges() refuse
	echo "refused\n";
	$json = new TS\Parser(TS\Grammar::JSON);
	$checks = [
		fn() => $parser->parse($new, $backup),
		fn() => $json->parse('[]', $backup),
		fn() => $incremental->getChangedRanges($json->parse('[]')),
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (ValueError $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	// the same source with an unedited tree is fine
	echo (string) $parser->parse($old, $backup)->getRootNode() === (string) $fresh->parse($old)->getRootNode() ? 'same source accepted' : 'different', PHP_EOL;

	// InputEdit: validation, also when the public properties are changed afterwards
	echo "InputEdit\n";
	$checks = [
		fn() => new TS\InputEdit(5, 3, 6, $point(0, 5), $point(0, 3), $point(0, 6)),
		fn() => new TS\InputEdit(5, 5, 5, $point(1, 0), $point(0, 5), $point(1, 0)),
		function () use ($parser, $point) {
			$edit = new TS\InputEdit(1, 1, 1, $point(0, 1), $point(0, 1), $point(0, 1));
			$edit->oldEndByte = 0;
			$parser->parse('<?php f();')->edit($edit);
		},
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (ValueError $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}

	// fromContentChange: UTF-16 columns, clamping to the end of the line, multi-line replacement
	echo $describe(TS\InputEdit::fromContentChange("héllo 😀 wörld\nsecond", 0, 9, 0, 999, "X\nY\nZZ")), PHP_EOL;
	echo $describe(TS\InputEdit::fromContentChange("héllo 😀 wörld\nsecond", 0, 6, 1, 3, '')), PHP_EOL;
	echo $describe(TS\InputEdit::fromContentChange("abc\n", 1, 0, 1, 0, 'x')), PHP_EOL;
	echo $describe(TS\InputEdit::fromContentChange('abc', 0, 1, 0, 1, '')), PHP_EOL;

	$checks = [
		fn() => TS\InputEdit::fromContentChange("a\nb", 5, 0, 5, 0, 'x'),
		fn() => TS\InputEdit::fromContentChange("a\nb", 1, 0, 0, 0, 'x'),
		fn() => TS\InputEdit::fromContentChange("a\nb", -1, 0, 0, 0, 'x'),
	];

	foreach ($checks as $check) {
		try {
			$check();
		} catch (ValueError $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}
?>
--EXPECT--
[23,23,31,[1,17],[1,17],[1,25]]
(program (php_tag) (function_definition name: (name) parameters: (formal_parameters) body: (compound_statement (expression_statement (assignment_expression left: (variable_name (name)) right: (integer))) (return_statement (integer)))))
bool(true)
current text
changed 23-30 1:17..1:24
0
stale tree
The tree was edited, parse the new source with it to get a current tree
The tree was edited, parse the new source with it to get a current tree
The tree was edited, parse the new source with it to get a current tree
The tree was edited, parse the new source with it to get a current tree
copy intact
refused
TreeSitter\Parser::parse(): Argument #2 ($old_tree) must be edited with Tree::edit() to match the new source
TreeSitter\Parser::parse(): Argument #2 ($old_tree) must have been parsed with the same grammar
TreeSitter\Tree::getChangedRanges(): Argument #1 ($old) must have been parsed with the same grammar
same source accepted
InputEdit
TreeSitter\InputEdit::__construct(): Argument #2 ($old_end_byte) must be between $start_byte and 4294967295
TreeSitter\InputEdit::__construct(): Argument #5 ($old_end_point) the old end point must not be before the start point
Invalid InputEdit: must be between $start_byte and 4294967295
[12,18,18,[0,12],[0,18],[2,2]]
[7,22,7,[0,7],[1,3],[0,7]]
[4,4,5,[1,0],[1,0],[1,1]]
[1,1,1,[0,1],[0,1],[0,1]]
TreeSitter\InputEdit::fromContentChange(): Argument #2 ($start_line) must be an existing line of the old text
TreeSitter\InputEdit::fromContentChange(): Argument #4 ($end_line) and argument #5 ($end_character) must not be before the start of the range
TreeSitter\InputEdit::fromContentChange(): Argument #2 ($start_line) must not be negative
