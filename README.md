# PHP8 bindings for Tree-sitter

https://tree-sitter.github.io/tree-sitter/

## Building

```bash
git submodule update --init --recursive

phpize

./configure

make
make test
make install
```

`extension=treesitter.so` has to be added to your PHP ini file.

## Usage

```php
use TreeSitter as TS;

$parser = new TS\Parser(TS\Grammar::JSON);

$tree = $parser->parse("[1, null]");

$rootNode = $tree->getRootNode();

echo $rootNode; // (document (array (number) (null)))
```

## API

### `TreeSitter\Parser`

#### `::__construct(TreeSitter\Grammar $grammar): TreeSitter\Parser`

Creates a new Parser object with the given `TreeSitter\Grammar` case. See built-in parser grammars in the [Parsers and Grammars](#parsers-and-grammars) section below.

#### `::parse(string $source_code, ?TreeSitter\Tree $old_tree = null): TreeSitter\Tree`

Parses the source code and produces a `TreeSitter\Tree` instance or throws `TreeSitter\SyntaxError` if the tree contains an `ERROR` or `MISSING` node. The message names the position of the first one (`Syntax error at line 1, column 7`, 1-based, byte columns).

#### `::parseWithErrors(string $source_code, ?TreeSitter\Tree $old_tree = null): TreeSitter\Tree`

Parses the source code and returns the `TreeSitter\Tree` even if it contains `ERROR` / `MISSING` nodes. Use this for editors and language servers, where the code is often incomplete. Find the errors with `Node::hasError()`, `Node::isError()` and `Node::isMissing()`.

If `$old_tree` is given, the parser reuses the parts of it that did not change, which is much faster than a full parse for a large document. See [Incremental parsing](#incremental-parsing).

#### `::getLanguage(): TreeSitter\Language`

The language the parser was created with.

#### `::setTimeoutMicros(int $timeout): void`, `::getTimeoutMicros(): int`

A parse that takes longer than the timeout throws a `TreeSitter\Exception`. `0` (the default) means no limit. The parser is reset when a parse was cancelled, the next parse starts from scratch.

#### `::setIncludedRanges(array $ranges): void`, `::getIncludedRanges(): array`

Restricts the parser to the given `Range`s of the source, which is how a document that embeds another language is parsed, see [Embedded languages](#embedded-languages). An empty list lifts the restriction. `::getIncludedRanges()` returns the current ranges, or one range that covers everything (`0` to `4294967295`) if none were set.

#### `::reset(): void`

Discards the state of a parse that was interrupted.

### `TreeSitter\Tree`

#### `::getRootNode(void): TreeSitter\Node`

Returns the root `TreeSitter\Node` instance that contains the AST (Abstract Syntax Tree).

### `TreeSitter\Node`

#### `::getType(): string`

Returns the type of the node as per the grammar selected.

#### `::hasChildren(): bool`

Whether this node has any children or not.

#### `::countChildren(): int`

Return the number of children this node has.

#### `::getStartByte(): int`

The start byte for this node within the `$source_code`. Note that the position will include multibyte characters. See `::getStartPoint`. Is relative to the root.

#### `::getEndByte(): int`

The end byte for this node within the `$source_code`. Note that the result will count multibyte characters as individual bytes. See `::getEndPoint`. Is relative to the root.

#### `::getStartPoint(): TreeSitter\Point`

Returns a `TreeSitter\Point` instance with the `int $row` and `int $column` properties set to the starting point of this node, taking into account multi-byte characters. Is relative to the root.

#### `::getEndPoint(): TreeSitter\Point`

Returns a `TreeSitter\Point` instance with the `int $row` and `int $column` properties set to the ending point of this node, taking into account multi-byte characters. Is relative to the root.

#### `::__toString(void): string`

Ouputs the S-expression syntax tree for the current node and its siblings.

#### Traversal

Every `TreeSitter\Node` keeps its `TreeSitter\Tree` alive, so nodes stay valid even if the tree variable goes out of scope. Methods that may not find a node return `null`. Nodes can be compared with `==`.

|Method|Description|
|-|-|
|`::getChild(int $index): ?Node`|The child at `$index` (anonymous nodes included), `null` if out of range.|
|`::getNamedChild(int $index): ?Node`|The child at `$index` counting named nodes only.|
|`::getChildren(): array`|All children as a list.|
|`::getNamedChildren(): array`|Named children as a list.|
|`::countNamedChildren(): int`|Number of named children (`::countChildren()` counts all).|
|`::getParent(): ?Node`|The parent node, `null` for the root.|
|`::getNextSibling()` / `::getPrevSibling(): ?Node`|Adjacent siblings.|
|`::getNextNamedSibling()` / `::getPrevNamedSibling(): ?Node`|Adjacent named siblings.|
|`::getChildByFieldName(string $name): ?Node`|The child assigned to a grammar field, e.g. `name` or `body`.|
|`::getFieldNameForChild(int $index): ?string`|The grammar field name of the child at `$index`.|
|`::getChildrenByFieldName(string $name): array`|All children assigned to a grammar field, as a list (empty if the field is unknown).|
|`::getChildByFieldId(int $id): ?Node`|Like `::getChildByFieldName()`, with a field id from `Language::getFieldIdForName()`.|
|`::getFirstChildForByte(int $byte): ?Node`|The first child that extends beyond `$byte` (`::getFirstNamedChildForByte()` for named nodes only).|
|`::descendantForByteRange(int $start, int $end): ?Node`|The smallest node spanning the byte range (`::namedDescendantForByteRange()` for named nodes only).|
|`::descendantForPointRange(Point $start, Point $end): ?Node`|The smallest node spanning the point range, for row/column positions such as the ones an LSP client sends (`::namedDescendantForPointRange()` for named nodes only). Columns are byte columns, see `Tree::utf16ToByteColumn()`.|
|`::getText(): string`|The source text covered by the node (byte based).|
|`::isNamed()`, `::isMissing()`, `::isExtra()`, `::hasError()`, `::isError(): bool`|Node flags. `::hasError()` is true if the node or any descendant is an error, `::isError()` only for the `ERROR` node itself.|
|`::equals(Node $other): bool`|Whether both objects are the same node of the same tree (`==` does the same).|
|`::getId(): int`|An id that is unique among the nodes of one tree, usable as an array key.|
|`::getKindId(): int`|The numeric node type, comparable with `Language::getSymbolForName()`. Faster than comparing `::getType()` strings.|
|`::getGrammarType(): string`|The type without aliases (`::getType()` returns the alias).|
|`::getDescendantCount(): int`|The number of nodes in the subtree, including the node itself.|
|`::getRange(): Range`|The byte and point range of the node.|
|`::walk(): TreeCursor`|A cursor rooted at this node.|

### `TreeSitter\TreeCursor`

An efficient way to walk a tree without allocating a `Node` per step. Created with `Tree::walk()` or `Node::walk()`.

```php
$cursor = $tree->walk();

while (true) {
    echo str_repeat('  ', $cursor->getDepth()), $cursor->getCurrentNode()->getType(), PHP_EOL;

    if ($cursor->gotoFirstChild()) {
        continue;
    }

    while (!$cursor->gotoNextSibling()) {
        if (!$cursor->gotoParent()) {
            break 2;
        }
    }
}
```

|Method|Description|
|-|-|
|`::gotoFirstChild()`, `::gotoLastChild()`, `::gotoNextSibling()`, `::gotoPreviousSibling()`, `::gotoParent(): bool`|Move the cursor, `false` if there is nowhere to go.|
|`::getCurrentNode(): Node`|The node the cursor points at.|
|`::getCurrentFieldName(): ?string`|The grammar field name of the current node.|
|`::getDepth(): int`|Depth relative to the node the cursor was created from.|
|`::reset(Node $node): void`|Restart the cursor at another node.|

### `TreeSitter\Tree`

`::getRootNode(): Node`, `::getSource(): string` (the parsed source) and `::walk(): TreeCursor`.

LSP clients count columns in UTF-16 code units, tree-sitter in UTF-8 bytes. Convert with:

|Method|Description|
|-|-|
|`::byteToUtf16Column(int $row, int $byteColumn): int`|Byte column to UTF-16 column of the given row. Throws `ValueError` for a row or column outside the source.|
|`::utf16ToByteColumn(int $row, int $utf16Column): int`|The reverse. A column inside a surrogate pair rounds up to the end of the character.|

Rows are counted by the line feed character only, like tree-sitter does.

### `TreeSitter\Point`

#### `int $row`

The row within the source tree. See `TreeSitter\Node::getStartPoint()` and `TreeSitter\Node::getEndPoint()`.

#### `int $column`

The column within the source tree, counted in bytes. See `TreeSitter\Node::getStartPoint()` and `TreeSitter\Node::getEndPoint()`.

#### `::__construct(int $row, int $column)`

Creates a point, e.g. to pass to `Node::descendantForPointRange()`. Both values must be between 0 and 4294967295.

### `TreeSitter\Range`

A range in the source: `int $startByte`, `int $endByte`, `Point $startPoint`, `Point $endPoint`. Returned by `Node::getRange()`, can be created with `new Range($startByte, $endByte, $startPoint, $endPoint)`. `::contains(Point $point): bool` tells whether a position lies in the range (the end is exclusive), e.g. to find out in which embedded language the cursor of an editor is.

### `TreeSitter\Language`

Describes a grammar. Get it from `Parser::getLanguage()` or `Language::fromGrammar(Grammar $grammar)`.

|Method|Description|
|-|-|
|`::getAbiVersion(): int`|The tree-sitter ABI version of the grammar.|
|`::getSymbolCount(): int`|Number of node types.|
|`::getSymbolName(int $symbol): ?string`|The type name of a node kind id (`Node::getKindId()`), `null` if unknown.|
|`::getSymbolForName(string $name, bool $named): int`|The kind id of a node type, `0` if there is none.|
|`::getFieldName(int $id): ?string`|The name of a field id, `null` if unknown.|
|`::getFieldIdForName(string $name): int`|The id of a field, `0` if there is none.|

### Incremental parsing

A language server gets a small change of a large document on every key stroke. Instead of parsing everything again, tell the old tree what changed and parse with it:

```php
use TreeSitter as TS;

$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
$tree = $parser->parse($text);

// LSP textDocument/didChange: range (line, UTF-16 character) and the new text
$edit = TS\InputEdit::fromContentChange($text, $startLine, $startCharacter, $endLine, $endCharacter, $newText);
$text = substr($text, 0, $edit->startByte) . $newText . substr($text, $edit->oldEndByte);

$tree->edit($edit);                          // the old tree is stale from now on
$newTree = $parser->parse($text, $tree);     // reuses everything that did not change

foreach ($newTree->getChangedRanges($tree) as $range) {
    // only these ranges changed structurally: re-analyze just them
}
```

Apply the changes of one notification one after the other, each with its own `Tree::edit()`, and parse once at the end. The result equals a full parse of the new text.

#### `TreeSitter\InputEdit`

Describes one edit: `int $startByte`, `int $oldEndByte`, `int $newEndByte` and the same positions as `Point`s (`$startPoint`, `$oldEndPoint`, `$newEndPoint`, rows and byte columns). `new InputEdit(...)` takes them in this order and validates them.

`InputEdit::fromContentChange(string $oldText, int $startLine, int $startCharacter, int $endLine, int $endCharacter, string $newText): InputEdit` builds it from an LSP text document change. The range is in lines and UTF-16 code unit columns of the *old* text; a character past the end of a line means the end of the line, as the LSP specification says. Lines are separated by `\n`, like everywhere in this extension.

#### `TreeSitter\Tree`: editing

|Method|Description|
|-|-|
|`::edit(InputEdit $edit): void`|Records an edit. Afterwards the tree is stale: `Node::getText()`, the column conversions and `QueryCursor::exec()` throw an `Error` until you parse the new source with it. Positions of the nodes are already the updated ones.|
|`::copy(): Tree`|A cheap copy, for example to keep the unedited tree around.|
|`::getChangedRanges(Tree $old): array`|The ranges (a list of `Range`) whose syntactic structure differs between `$old`, which has to be the edited tree the new one was parsed with, and this tree. A change that keeps the structure (`1` to `42`) reports nothing.|

`Parser::parse()` refuses an old tree that was parsed with another grammar, and an unedited old tree together with a different source (a `ValueError`), both mistakes would silently produce a wrong tree.

### Embedded languages

A PHP template contains HTML, which contains JavaScript and CSS. Each language is parsed with its own grammar, restricted to the parts of the document that belong to it: the parser only sees the text inside the ranges, the positions of the nodes are still the ones of the whole document.

```php
use TreeSitter as TS;

// 1. which parts are HTML? Ask the query that ships with the PHP grammar
$php = new TS\Parser(TS\Grammar::PHP);
$query = new TS\Query(TS\Grammar::PHP, TS\Grammar::PHP->getQuery('injections-text'));

$cursor = new TS\QueryCursor($query);
$cursor->exec($php->parse($document)->getRootNode());

$ranges = [];
foreach ($cursor->getMatches() as $match) {
    foreach ($match->getNodes('injection.content') as $node) {
        $ranges[] = $node->getRange();
    }
}

// 2. parse just those parts as HTML
$html = new TS\Parser(TS\Grammar::HTML);
$html->setIncludedRanges($ranges);
$htmlTree = $html->parse($document);

// 3. the HTML grammar has its own 'injections' query: <script> and <style>
//    the language is in $match->properties['injection.language'] ('javascript', 'css')
```

The `injections` queries of the grammars name the embedded content (`@injection.content`) and its language (`#set! injection.language "html"`, or a capture `@injection.language`); `#set! injection.combined` says that all the ranges belong to one document, as with the HTML text around the PHP tags. Which parser to use for a language name, and how to keep the layers up to date, is up to the application: the extension provides the ranges, the queries and the parsers.

The ranges must be sorted and must not overlap (a `ValueError` otherwise), and `Tree::getIncludedRanges()` returns the ranges a tree was parsed with.

Incremental parsing works with included ranges too. `Tree::edit()` moves the ranges of the tree, but the parser keeps the ranges it was given, so hand it the moved ones before parsing with the edited tree:

```php
$tree->edit($edit);
$parser->setIncludedRanges($tree->getIncludedRanges());
$newTree = $parser->parse($newText, $tree);
```

Ranges that changed for another reason (a PHP edit that turned text into code) have to be recomputed from the parent layer and set the same way.

### Queries: `TreeSitter\Query`, `TreeSitter\QueryCursor`

Queries find nodes with tree-sitter's [pattern syntax](https://tree-sitter.github.io/tree-sitter/using-parsers/queries/index.html), which is much faster than walking the tree in PHP.

```php
use TreeSitter as TS;

$parser = new TS\Parser(TS\Grammar::PHP_ONLY);
$root = $parser->parse('<?php function foo() {} function Bar() {}')->getRootNode();

$query = new TS\Query(TS\Grammar::PHP_ONLY, '(function_definition name: (name) @fn (#match? @fn "^[a-z]"))');

$cursor = new TS\QueryCursor($query);
$cursor->exec($root);

foreach ($cursor->getMatches() as $match) {
    echo $match->getNode('fn')->getText(), PHP_EOL; // foo
}
```

#### `TreeSitter\Query`

|Method|Description|
|-|-|
|`::__construct(Grammar $grammar, string $source)`|Compiles the query for a grammar. Throws `QueryException` if the source is invalid.|
|`::getCaptureNames(): array`|The capture names (without `@`), indexed by capture id.|
|`::getPatternCount(): int`|The number of patterns in the source.|
|`::disablePattern(int $index): void`, `::disableCapture(string $name): void`|Remove a pattern or a capture from the matching. Cannot be undone.|

Tree-sitter itself does not evaluate the predicates of a query, this extension does. Predicates must be inside the pattern they belong to, e.g. `((name) @n (#eq? @n "foo"))`; a predicate outside of a pattern becomes a pattern of its own that matches everything.

|Predicate|Meaning|
|-|-|
|`(#eq? @a "text")`, `(#eq? @a @b)`|The text of the capture equals the string, or the text of the other capture.|
|`(#match? @a "regex")`|The text matches the regular expression. It is evaluated with PCRE in Unicode mode (`preg_match`), so use PCRE syntax. Text that is not valid UTF-8 never matches.|
|`(#any-of? @a "x" "y")`|The text is one of the strings.|
|`(#not-eq? ...)`, `(#not-match? ...)`, `(#not-any-of? ...)`|The negations.|
|`(#any-eq? ...)`, `(#any-match? ...)`, `(#any-not-eq? ...)`, `(#any-not-match? ...)`|For a quantified capture (`(_)* @a`) the predicate has to hold for *one* node. Without `any-` it has to hold for *every* node of the capture.|
|`(#set! key "value")`, `(#set! key)`|Adds the property to `QueryMatch::$properties` (`null` if there is no value).|

Other predicates (`#is?`, `#is-not?`, and editor specific ones like `#lua-match?`) are accepted and ignored: they do not filter matches. A malformed predicate of the supported kinds, or an invalid regular expression, throws `QueryException` when the query is created.

#### `TreeSitter\QueryCursor`

Runs a query. It keeps the `Query` and the tree of the node alive, and can be reused with `::exec()`.

|Method|Description|
|-|-|
|`::__construct(Query $query)`|Creates a cursor for the query.|
|`::exec(Node $node): void`|Starts the query on a node (and its descendants). Range and limit settings stay in place.|
|`::nextMatch(): ?QueryMatch`|The next match, `null` when done. A match holds all the captures of one pattern hit.|
|`::nextCapture(): ?QueryCapture`|The next capture in document order, `null` when done. Use this for highlighting.|
|`::getMatches(): array`, `::getCaptures(): array`|The rest of the matches or captures as a list. `nextMatch()` and `nextCapture()` do not allocate a list, prefer them for big trees.|
|`::setByteRange(int $start, int $end): void`, `::setPointRange(Point $start, Point $end): void`|Only return matches that intersect the range, e.g. the visible part of a document.|
|`::setMatchLimit(int $limit): void`, `::didExceedMatchLimit(): bool`|Limits the number of in-progress matches, which bounds the memory of pathological queries.|

#### `TreeSitter\QueryMatch`, `TreeSitter\QueryCapture`

`QueryMatch` has `int $patternIndex`, `array $captures` (a list of `QueryCapture`), `array $properties` (from `#set!`), and `::getNode(string $name): ?Node`, `::getNodes(string $name): array`. `QueryCapture` has `string $name`, `Node $node` and `int $index` (the capture id). Both keep their tree alive.

#### `TreeSitter\QueryException`

Extends `TreeSitter\Exception`. The message names the position (`Invalid query: unknown node type 'foo' at line 1, column 2`), `int $offset` is the byte offset in the query source and `string $errorType` is one of `syntax`, `node_type`, `field`, `capture`, `structure`, `language` or `predicate`.

### `TreeSitter\Exception` and `TreeSitter\SyntaxError`

`TreeSitter\Exception` extends `\Exception` and is the base of the exceptions of this extension. `TreeSitter\SyntaxError` extends it and is thrown from `TreeSitter\Parser::parse` if a syntax error is detected.

### Queries that ship with the grammars

`Grammar::getQuery(string $name): ?string` returns the source of a query file of the grammar (the `queries/*.scm` files: `highlights`, `tags`, `injections`, `locals`, ...), `null` if the grammar has none by that name. `Grammar::getQueryNames(): array` lists the available names. The files are embedded in the extension, so it does not matter where it is installed.

```php
$tags = new TS\Query(TS\Grammar::PHP_ONLY, TS\Grammar::PHP_ONLY->getQuery('tags'));
// captures: @definition.class, @definition.function, @reference.call, ... each with a @name
```

Note that the query files of a grammar can rely on those of another one: the TypeScript queries are meant to be used together with the JavaScript ones (`Grammar::JAVASCRIPT->getQuery('highlights') . Grammar::TYPESCRIPT->getQuery('highlights')` compiled for TypeScript). Queries that use editor specific predicates (e.g. `#lua-match?`) still compile, but those predicates do not filter, see `Query`.

The embedded copy is `lib/parsers/queries.gen.h`. After updating the grammar submodules regenerate it with `php scripts/embed-queries.php`.

## Parsers and Grammars

`TreeSitter\Grammar` is an enum, so a parser, a query or a language can only be created for a grammar that exists. Every case has a stable number as its value (`Grammar::PHP->value`), which is what `Grammar::from(int)` and `Grammar::tryFrom(int)` take, e.g. when a grammar is chosen by a configuration value. `Grammar::cases()` lists them.

```php
$grammar = TS\Grammar::tryFrom($config['grammar']) ?? TS\Grammar::PHP_ONLY;
$parser = new TS\Parser($grammar);
```

Upgrading from a version where `Grammar` had integer constants: `Grammar::PHP` and the other names are still there and are still what you pass to `Parser`, `Query` and `Language::fromGrammar()`, but they are enum cases now, an integer is rejected with a `TypeError`. Use `Grammar::from($int)` for a stored number, and `$grammar->getQuery($name)` instead of `Grammar::getQuery($grammar, $name)`.

This extension includes the following parsers:

|Parser|Case|
|-|-|
|[PHP](https://github.com/tree-sitter/tree-sitter-php)|`TreeSitter\Grammar::PHP` (PHP embedded in HTML: text outside `<?php ... ?>` is markup)|
|[PHP only](https://github.com/tree-sitter/tree-sitter-php)|`TreeSitter\Grammar::PHP_ONLY` (pure PHP: the source is code even without an opening `<?php` tag)|
|[HTML](https://github.com/tree-sitter/tree-sitter-html)|`TreeSitter\Grammar::HTML`|
|[CSS](https://github.com/tree-sitter/tree-sitter-css)|`TreeSitter\Grammar::CSS`|
|[JavaScript & JSX](https://github.com/tree-sitter/tree-sitter-javascript)|`TreeSitter\Grammar::JAVASCRIPT`|
|[TypeScript & TSX](https://github.com/tree-sitter/tree-sitter-typscript)|`TreeSitter\Grammar::TYPESCRIPT`|
|[JSON](https://github.com/tree-sitter/tree-sitter-json)|`TreeSitter\Grammar::JSON`|
|[Python](https://github.com/tree-sitter/tree-sitter-python)|`TreeSitter\Grammar::PYTHON`|

## Contributing

`ext-treesitter` is undergoing initial **heavy development and is highly unstable**.

### Working on the extension

|What|How|
|-|-|
|Build and test|`phpize && ./configure && make && make test` (`make test TESTS=tests/treesitter_012.phpt` for one test). Run `phpize --clean && phpize` again after `config.m4` changed.|
|Arginfo|`treesitter_arginfo.h` is generated from `treesitter.stub.php`, never edit it by hand: `php build/gen_stub.php treesitter.stub.php`. In a stub docblock `@param` cannot carry a generic type (`list<Range>`), `@return` and `@var` can.|
|Embedded queries|`lib/parsers/queries.gen.h` is generated from the `queries/*.scm` files of the grammars with `php scripts/embed-queries.php`. Run it after updating the grammar submodules.|
|Memory errors|`bash scripts/asan-test.sh` builds a copy with AddressSanitizer and LeakSanitizer and runs all the tests with it. Some tests also run with `MALLOC_PERTURB_` (`--ENV--` section) so that a use of freed memory shows up as garbage even without a sanitizer.|

A test that needs the output of the extension is easiest to write by running a script first, checking the output line by line, and only then pasting it into the `--EXPECT--` section.

Missing something? File an [issue](https://github.com/soulseekah/ext-treesitter/issues) and/or open a PR.
