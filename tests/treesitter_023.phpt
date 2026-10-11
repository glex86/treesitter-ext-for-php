--TEST--
Tree-sitter: PHPDoc grammar
--ENV--
MALLOC_PERTURB_=165
USE_ZEND_ALLOC=0
--FILE--
<?php
	use TreeSitter as TS;

	$parser = new TS\Parser(TS\Grammar::PHPDOC);
	$tree = $parser->parse("/**\n * Summary.\n *\n * @param int|string \$a The a\n * @return false\n */");
	$root = $tree->getRootNode();

	echo $root->getType(), PHP_EOL;
	foreach ($root->getNamedChildren() as $child) {
		echo $child->getType(), ' ', json_encode($child->getText()), PHP_EOL;
	}

	$tag = $root->getNamedChildren()[1];
	foreach ($tag->getNamedChildren() as $part) {
		echo '  ', $part->getType(), ' ', json_encode($part->getText()), PHP_EOL;
	}
?>
--EXPECT--
document
description "Summary."
tag "@param int|string $a The a"
tag "@return false"
  tag_name "@param"
  union_type "int|string"
  variable_name "$a"
  description "The a"
