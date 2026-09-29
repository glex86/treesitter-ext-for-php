--TEST--
Tree-sitter: 300 random edits, incremental parse equals fresh parse
--FILE--
<?php
	use TreeSitter as TS;

	// 300 random edits: the incrementally parsed tree must always equal a fresh parse
	$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
	$fresh = new TS\Parser(TS\Grammar::PHP_ONLY);

	$pieces = ['a', 'b', 'é', '😀', ' ', "\n", ';', '(', ')', '{', '}', '$x', 'function f() {}', 'foo(1);', "// c\n", "'s'", '1+2', 'class A {}', 'if (1) {', '}'];
	$initial = "<?php\nnamespace App;\nclass Foo extends Bar {\n\tpublic function run(int \$a): string {\n\t\treturn 'héllo' . \$this->x(\$a);\n\t}\n}\nfunction top(\$é) { return [1, 2, \$é]; }\n";
	$units = function (string $text): int {
		$count = 0;
		foreach (preg_split('//u', $text, -1, PREG_SPLIT_NO_EMPTY) as $character) {
			$count += strlen($character) === 4 ? 2 : 1;
		}
		return $count;
	};

	mt_srand(42);
	$text = $initial;
	$tree = $parser->parseWithErrors($text);
	$applied = $skipped = $mismatches = $changed = 0;

	for ($i = 0; $i < 300; $i++) {
		$lines = explode("\n", $text);
		$startLine = mt_rand(0, count($lines) - 1);
		$endLine = min(count($lines) - 1, $startLine + mt_rand(0, 1));
		$startCharacter = mt_rand(0, $units($lines[$startLine]) + 2);
		$endCharacter = mt_rand(0, $units($lines[$endLine]) + 2);

		if ($endLine === $startLine && $endCharacter < $startCharacter) {
			[$startCharacter, $endCharacter] = [$endCharacter, $startCharacter];
		}

		$insert = '';
		for ($k = mt_rand(0, 3); $k > 0; $k--) {
			$insert .= $pieces[mt_rand(0, count($pieces) - 1)];
		}

		try {
			$edit = TS\InputEdit::fromContentChange($text, $startLine, $startCharacter, $endLine, $endCharacter, $insert);
		} catch (ValueError $e) {
			$skipped++;
			continue;
		}

		$newText = substr($text, 0, $edit->startByte) . $insert . substr($text, $edit->oldEndByte);

		$tree->edit($edit);
		$incremental = $parser->parseWithErrors($newText, $tree);

		if ((string) $incremental->getRootNode() !== (string) $fresh->parseWithErrors($newText)->getRootNode()) {
			$mismatches++;
		}

		$changed += count($incremental->getChangedRanges($tree));
		$applied++;
		$tree = $incremental;
		$text = $newText;

		if (strlen($text) > 3000 || strlen($text) < 40) {
			$text = $initial;
			$tree = $parser->parseWithErrors($text);
		}
	}

	echo 'applied ', $applied, ', skipped ', $skipped, ', mismatches ', $mismatches, PHP_EOL;
	var_dump($changed > 0);
?>
--EXPECT--
applied 300, skipped 0, mismatches 0
bool(true)
