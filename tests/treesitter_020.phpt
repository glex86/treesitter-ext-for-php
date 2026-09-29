--TEST--
Tree-sitter: nodes and cursors survive Tree::edit, huge parser timeout
--ENV--
MALLOC_PERTURB_=165
USE_ZEND_ALLOC=0
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$old = "<?php\nfunction foo() { return 1; }\nfunction bar() {}\n";

	// nodes, cursors and queries taken before an edit stay valid: they show the tree as it was,
	// also after the copy that shared its subtrees is gone
	$tree = $parser->parse($old);
	$root = $tree->getRootNode();
	$function = $root->getNamedChild(1);
	$cursor = $tree->walk();
	$cursor->gotoFirstChild();

	$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @n)');
	$queryCursor = new TS\QueryCursor($query);
	$queryCursor->exec($root);

	$copy = $tree->copy();
	$tree->edit(TS\InputEdit::fromContentChange($old, 1, 17, 1, 17, '$a = 2; $b = 3; '));
	unset($copy);
	gc_collect_cycles();

	// churn the allocator, a use of freed memory would show garbage here
	$garbage = [];
	for ($i = 0; $i < 2000; $i++) {
		$garbage[] = str_repeat('Z', 200 + $i % 50);
	}

	echo json_encode(array_map(fn($node) => $node->getType(), $root->getNamedChildren())), PHP_EOL;
	echo $function->getChildByFieldName('name')->getType(), ' ', $function->getChildByFieldName('body')->countNamedChildren(), PHP_EOL;
	echo $cursor->getCurrentNode()->getType(), ' ', $cursor->gotoNextSibling() ? 'moved' : 'end', ' ', $cursor->getCurrentNode()->getType(), PHP_EOL;
	echo count($queryCursor->getMatches()), PHP_EOL;

	// many edits of one tree
	for ($i = 0; $i < 50; $i++) {
		$tree->edit(TS\InputEdit::fromContentChange($old, 2, 0, 2, 0, "x();\n"));
	}
	echo $function->getParent()->getType(), ' ', count($root->getNamedChildren()), PHP_EOL;

	// the edited tree itself is still usable for parsing
	$new = "<?php\nfunction foo() { \$a = 2; \$b = 3; return 1; }\nfunction bar() {}\n";
	$tree = $parser->parse($old);
	$tree->edit(TS\InputEdit::fromContentChange($old, 1, 17, 1, 17, '$a = 2; $b = 3; '));
	echo (string) $parser->parse($new, $tree)->getRootNode() === (string) (new TS\Parser(TS\Grammar::PHP_ONLY))->parse($new)->getRootNode() ? 'incremental equals fresh' : 'different', PHP_EOL;

	// a huge timeout means "no practical limit", it does not wrap around into the past
	$parser->setTimeoutMicros(PHP_INT_MAX);
	echo $parser->getTimeoutMicros() === PHP_INT_MAX ? 'max timeout kept' : 'changed', PHP_EOL;
	echo $parser->parse($old)->getRootNode()->getType(), PHP_EOL;
?>
--EXPECT--
["php_tag","function_definition","function_definition"]
name 1
php_tag moved function_definition
2
function_definition 3
incremental equals fresh
max timeout kept
program
