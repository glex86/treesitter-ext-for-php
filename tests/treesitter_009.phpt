--TEST--
Tree-sitter: Point/Range, point based lookups, node identity and field helpers
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$root = $parser->parse("<?php\nfunction f(\$a, \$b) {\n\treturn \$a + \$b;\n}\n")->getRootNode();

	$at = fn(int $row, int $column) => $root->descendantForPointRange(new TS\Point($row, $column), new TS\Point($row, $column));

	$name = $at(2, 9);
	echo $name->getType(), ' ', $name->getText(), PHP_EOL;
	echo $root->namedDescendantForPointRange(new TS\Point(2, 1), new TS\Point(2, 7))->getType(), PHP_EOL;

	// identity
	var_dump($name->equals($at(2, 9)), $name->equals($root), $name->getId() === $at(2, 9)->getId(), $name->getId() === $root->getId());
	var_dump($name->getKindId() > 0, $name->getGrammarType(), $root->getDescendantCount());

	// fields
	$function = $root->getNamedChild(1);
	$language = $parser->getLanguage();
	$fieldId = $language->getFieldIdForName('parameters');

	echo $function->getChildByFieldId($fieldId)->getType(), ' ', var_export($function->getChildByFieldId(0), true), PHP_EOL;
	echo count($function->getChildrenByFieldName('parameters')), ' ', count($function->getChildrenByFieldName('nope')), PHP_EOL;

	// byte based child lookups
	echo $function->getFirstChildForByte(0)->getType(), ' ', $function->getFirstNamedChildForByte(9)->getType(), PHP_EOL;

	// ranges
	$range = $function->getRange();
	echo $range->startByte, ' ', $range->endByte, ' ',
		$range->startPoint->row, ',', $range->startPoint->column, ' ',
		$range->endPoint->row, ',', $range->endPoint->column, PHP_EOL;

	$custom = new TS\Range(1, 5, new TS\Point(0, 1), new TS\Point(0, 5));
	echo $custom->startByte, $custom->endByte, $custom->startPoint->column, $custom->endPoint->column, PHP_EOL;

	$checks = [
		fn() => new TS\Point(-1, 0),
		fn() => new TS\Range(5, 1, new TS\Point(0, 0), new TS\Point(0, 0)),
		fn() => $root->descendantForPointRange(new TS\Point(3, 0), new TS\Point(1, 0)),
	];

	foreach ($checks as $check) {
		try {
			$check();
			echo 'no error', PHP_EOL;
		} catch (ValueError $e) {
			echo $e->getMessage(), PHP_EOL;
		}
	}
?>
--EXPECT--
name a
return_statement
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
string(4) "name"
int(31)
formal_parameters NULL
1 0
function name
6 45 1,0 3,1
1515
TreeSitter\Point::__construct(): Argument #1 ($row) must be between 0 and 4294967295
TreeSitter\Range::__construct(): Argument #2 ($end_byte) must be greater than or equal to argument #1 ($start_byte)
TreeSitter\Node::descendantForPointRange(): Argument #2 ($end) must not be before argument #1 ($start)
