<?php

/** @generate-class-entries */

namespace TreeSitter {
	/**
	 * The grammars that are built into the extension. The value of a case is a stable number.
	 */
	enum Grammar: int {
		/**
		 * PHP embedded in HTML: the text outside of the PHP tags is markup.
		 */
		case PHP = 0x706870;

		/**
		 * Pure PHP: the source is code even without an opening tag.
		 */
		case PHP_ONLY = 0x7068705f6f6e6c79;

		/**
		 * The content of the PHPDoc comments of a PHP tree, to parse the text of its comment nodes.
		 */
		case PHPDOC = 0x706870646f63;

		case HTML = 0x68746d6c;

		case CSS = 0x637373;

		case JAVASCRIPT = 0x6a73;

		case TYPESCRIPT = 0x7473;

		case PYTHON = 0x707974686f6e;

		case JSON = 0x6a736f6e;

		/**
		 * The source of a query file that ships with the grammar (highlights, tags, injections,
		 * locals, ...), or null if the grammar has no such query. See getQueryNames().
		 */
		public function getQuery(string $name): ?string {}

		/** @return list<string> */
		public function getQueryNames(): array {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Parser {
		public function __construct(Grammar $grammar) {}

		/**
		 * @param Tree|null $old_tree the previous tree of the document, edited with Tree::edit() to match the new source
		 * @throws SyntaxError when the source code contains syntax errors
		 */
		public function parse(string $source_code, ?Tree $old_tree = null): Tree {}

		/**
		 * Like parse(), but returns the tree even if it contains ERROR / MISSING nodes.
		 */
		public function parseWithErrors(string $source_code, ?Tree $old_tree = null): Tree {}

		/**
		 * Restricts parsing to the given ranges of the source, which is how a document that
		 * embeds another language is parsed: the parser sees only the text inside the ranges,
		 * the positions of the nodes stay the ones of the whole document. The ranges must be
		 * sorted and must not overlap. An empty list parses the whole source again.
		 * $ranges is a list of Range objects.
		 */
		public function setIncludedRanges(array $ranges): void {}

		/**
		 * The ranges set with setIncludedRanges(), or one range that covers everything.
		 *
		 * @return list<Range>
		 */
		public function getIncludedRanges(): array {}

		/**
		 * Discards the state of an interrupted parse.
		 */
		public function reset(): void {}

		/**
		 * Parsing that takes longer than this many microseconds throws an Exception. 0 means no limit.
		 */
		public function setTimeoutMicros(int $timeout): void {}

		public function getTimeoutMicros(): int {}

		public function getLanguage(): Language {}
	}

	/**
	 * Describes an edit of the source, in bytes and in points (rows and byte columns).
	 *
	 * @strict-properties
	 * @not-serializable
	 */
	final class InputEdit {
		public int $startByte;

		public int $oldEndByte;

		public int $newEndByte;

		public Point $startPoint;

		public Point $oldEndPoint;

		public Point $newEndPoint;

		public function __construct(
			int $start_byte,
			int $old_end_byte,
			int $new_end_byte,
			Point $start_point,
			Point $old_end_point,
			Point $new_end_point
		) {}

		/**
		 * Builds the edit from an LSP text document change: the range is given in lines and UTF-16
		 * code unit columns of the old text, columns past the end of a line count as the end of it.
		 */
		public static function fromContentChange(
			string $old_text,
			int $start_line,
			int $start_character,
			int $end_line,
			int $end_character,
			string $new_text
		): InputEdit {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Language {
		public function __construct() {}

		public static function fromGrammar(Grammar $grammar): Language {}

		public function getAbiVersion(): int {}

		public function getSymbolCount(): int {}

		public function getSymbolName(int $symbol): ?string {}

		public function getSymbolForName(string $name, bool $named): int {}

		public function getFieldName(int $field_id): ?string {}

		public function getFieldIdForName(string $name): int {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Tree {
		public function __construct() {}

		public function getRootNode(): Node {}

		public function getSource(): string {}

		public function walk(): TreeCursor {}

		/**
		 * Records an edit of the source. The tree is stale afterwards: pass it to Parser::parse()
		 * together with the new source to get the current tree. Until then Node::getText(),
		 * the column conversions and queries throw an Error.
		 */
		public function edit(InputEdit $edit): void {}

		/**
		 * A cheap copy, e.g. to keep the unedited tree around for getChangedRanges().
		 */
		public function copy(): Tree {}

		/**
		 * The ranges of the source this tree was parsed from (see Parser::setIncludedRanges()).
		 *
		 * @return list<Range>
		 */
		public function getIncludedRanges(): array {}

		/**
		 * The ranges whose syntactic structure differs between the given old tree (edited, and
		 * used to parse this tree) and this tree.
		 *
		 * @return list<Range>
		 */
		public function getChangedRanges(Tree $old): array {}

		/**
		 * Converts a UTF-8 byte column of the given row to the UTF-16 code unit column used by LSP.
		 */
		public function byteToUtf16Column(int $row, int $byte_column): int {}

		/**
		 * Converts an LSP UTF-16 code unit column of the given row to the UTF-8 byte column.
		 */
		public function utf16ToByteColumn(int $row, int $utf16_column): int {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Node {
		public function __construct() {}

		public function getType(): string {}

		public function hasChildren(): bool {}

		public function countChildren(): int {}

		public function getStartByte(): int {}

		public function getEndByte(): int {}

		public function getStartPoint(): Point {}

		public function getEndPoint(): Point {}

		public function __toString(): string {}

		public function isNamed(): bool {}

		public function isMissing(): bool {}

		public function isExtra(): bool {}

		public function hasError(): bool {}

		public function countNamedChildren(): int {}

		public function getChild(int $index): ?Node {}

		public function getNamedChild(int $index): ?Node {}

		/** @return list<Node> */
		public function getChildren(): array {}

		/** @return list<Node> */
		public function getNamedChildren(): array {}

		public function getParent(): ?Node {}

		public function getNextSibling(): ?Node {}

		public function getPrevSibling(): ?Node {}

		public function getNextNamedSibling(): ?Node {}

		public function getPrevNamedSibling(): ?Node {}

		public function getChildByFieldName(string $name): ?Node {}

		public function getFieldNameForChild(int $index): ?string {}

		public function getText(): string {}

		public function descendantForByteRange(int $start_byte, int $end_byte): ?Node {}

		public function namedDescendantForByteRange(int $start_byte, int $end_byte): ?Node {}

		public function descendantForPointRange(Point $start, Point $end): ?Node {}

		public function namedDescendantForPointRange(Point $start, Point $end): ?Node {}

		public function walk(): TreeCursor {}

		public function isError(): bool {}

		public function equals(Node $other): bool {}

		public function getId(): int {}

		public function getKindId(): int {}

		public function getGrammarType(): string {}

		public function getDescendantCount(): int {}

		public function getFirstChildForByte(int $byte): ?Node {}

		public function getFirstNamedChildForByte(int $byte): ?Node {}

		public function getChildByFieldId(int $field_id): ?Node {}

		/** @return list<Node> */
		public function getChildrenByFieldName(string $name): array {}

		public function getRange(): Range {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class TreeCursor {
		public function __construct() {}

		public function gotoFirstChild(): bool {}

		public function gotoLastChild(): bool {}

		public function gotoNextSibling(): bool {}

		public function gotoPreviousSibling(): bool {}

		public function gotoParent(): bool {}

		public function getCurrentNode(): Node {}

		public function getCurrentFieldName(): ?string {}

		public function getDepth(): int {}

		public function reset(Node $node): void {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Point {
		public int $row;

		public int $column;

		public function __construct(int $row, int $column) {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Range {
		public int $startByte;

		public int $endByte;

		public Point $startPoint;

		public Point $endPoint;

		public function __construct(int $start_byte, int $end_byte, Point $start_point, Point $end_point) {}

		/**
		 * Whether the point lies in the range (its end is exclusive).
		 */
		public function contains(Point $point): bool {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Query {
		/**
		 * @throws QueryException when the query source is invalid
		 */
		public function __construct(Grammar $grammar, string $source) {}

		/** @return list<string> */
		public function getCaptureNames(): array {}

		public function getPatternCount(): int {}

		public function disablePattern(int $pattern_index): void {}

		public function disableCapture(string $name): void {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class QueryCursor {
		public function __construct(Query $query) {}

		public function exec(Node $node): void {}

		public function setByteRange(int $start_byte, int $end_byte): void {}

		public function setPointRange(Point $start, Point $end): void {}

		public function setMatchLimit(int $limit): void {}

		public function didExceedMatchLimit(): bool {}

		public function nextMatch(): ?QueryMatch {}

		public function nextCapture(): ?QueryCapture {}

		/** @return list<QueryMatch> */
		public function getMatches(): array {}

		/** @return list<QueryCapture> */
		public function getCaptures(): array {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class QueryMatch {
		public int $patternIndex;

		/** @var list<QueryCapture> */
		public array $captures;

		/** @var array<string, string|null> */
		public array $properties;

		public function __construct() {}

		public function getNode(string $name): ?Node {}

		/** @return list<Node> */
		public function getNodes(string $name): array {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class QueryCapture {
		public string $name;

		public Node $node;

		public int $index;

		public function __construct() {}
	}

	class Exception extends \Exception {
	}

	class QueryException extends Exception {
		public int $offset = 0;

		public string $errorType = '';
	}

	final class SyntaxError extends Exception {
	}
}
