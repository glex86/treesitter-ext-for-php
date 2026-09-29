--TEST--
Tree-sitter: injections, PHP with HTML with JavaScript and CSS (included ranges)
--ENV--
MALLOC_PERTURB_=165
USE_ZEND_ALLOC=0
--FILE--
<?php
	use TreeSitter as TS;

	// a PHP template embedding HTML, which embeds JavaScript and CSS
	$document = "<!DOCTYPE html>\n<html><body>\n<script>var a = 1;</script>\n<style>p { color: red; }</style>\n<?php echo \"hi\"; ?>\n<p>text é</p>\n</body></html>\n";

	function injections(TS\Grammar $grammar, string $queryName, TS\Node $root): array {
		$cursor = new TS\QueryCursor(new TS\Query($grammar, $grammar->getQuery($queryName)));
		$cursor->exec($root);

		$found = [];
		foreach ($cursor->getMatches() as $match) {
			foreach ($match->getNodes('injection.content') as $node) {
				$found[] = [$node->getRange(), $match->properties];
			}
		}

		return $found;
	}

	function show(array $ranges): string {
		return implode(' ', array_map(fn(TS\Range $range) => $range->startByte . '-' . $range->endByte, $ranges));
	}

	// 1. the PHP grammar finds the text between the PHP tags: that is HTML
	$php = new TS\Parser(TS\Grammar::PHP);
	$found = injections(TS\Grammar::PHP, 'injections-text', $php->parse($document)->getRootNode());
	echo 'html: ', show(array_column($found, 0)), ' ', json_encode($found[0][1]), PHP_EOL;

	// 2. parse only those ranges with the HTML grammar
	$html = new TS\Parser(TS\Grammar::HTML);
	$html->setIncludedRanges(array_column($found, 0));
	echo 'parser: ', show($html->getIncludedRanges()), PHP_EOL;

	$htmlTree = $html->parseWithErrors($document);
	echo 'tree: ', show($htmlTree->getIncludedRanges()), PHP_EOL;
	echo 'root: ', $htmlTree->getRootNode()->getStartByte(), '-', $htmlTree->getRootNode()->getEndByte(), PHP_EOL;
	echo $htmlTree->getRootNode()->getNamedChild(0)->getType(), ' ', $htmlTree->getRootNode()->getNamedChild(1)->getType(), PHP_EOL;

	// 3. the HTML grammar finds the script and the style
	$scripts = injections(TS\Grammar::HTML, 'injections', $htmlTree->getRootNode());

	foreach ($scripts as [$range, $properties]) {
		echo $properties['injection.language'], ' ', show([$range]), ' ', json_encode(substr($document, $range->startByte, $range->endByte - $range->startByte)), PHP_EOL;
	}

	// 4. parse each of them with its own grammar, positions stay the ones of the whole document
	foreach ([TS\Grammar::JAVASCRIPT, TS\Grammar::CSS] as $index => $grammar) {
		$parser = new TS\Parser($grammar);
		$parser->setIncludedRanges([$scripts[$index][0]]);
		$root = $parser->parse($document)->getRootNode();

		echo $root, ' @', $root->getStartByte(), '-', $root->getEndByte(), ' ', json_encode($root->getNamedChild(0)->getText()), PHP_EOL;
	}

	// 5. which language is at the cursor?
	$range = $scripts[0][0];
	var_dump(
		$range->contains($range->startPoint),
		$range->contains($range->endPoint),
		$range->contains(new TS\Point($range->startPoint->row, $range->startPoint->column + 3)),
		$range->contains(new TS\Point(0, 0))
	);
?>
--EXPECT--
html: 0-90 110-140 {"injection.language":"html","injection.combined":null}
parser: 0-90 110-140
tree: 0-90 110-140
root: 0-140
doctype element
javascript 37-47 "var a = 1;"
css 64-81 "p { color: red; }"
(program (variable_declaration (variable_declarator name: (identifier) value: (number)))) @37-47 "var a = 1;"
(stylesheet (rule_set (selectors (tag_name)) (block (declaration (property_name) (plain_value))))) @64-81 "p { color: red; }"
bool(true)
bool(false)
bool(true)
bool(false)
