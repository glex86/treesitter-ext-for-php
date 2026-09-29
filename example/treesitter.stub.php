<?php

/** @generate-class-entries */

namespace TreeSitter {
	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Grammar {
		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_PHP
		 */
		public const int PHP = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_HTML
		 */
		public const int HTML = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_CSS
		 */
		public const int CSS = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_JAVASCRIPT
		 */
		public const int JAVASCRIPT = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_TYPESCRIPT
		 */
		public const int TYPESCRIPT = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_PYTHON
		 */
		public const int PYTHON = UNKNOWN;

		/**
		 * @cvalue PHP_TREESITTER_GRAMMAR_JSON
		 */
		public const int JSON = UNKNOWN;

		public function __construct() {}
	}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class Parser {
		public function __construct(int $grammar) {}

		public function parse(string $source_code): Tree {}
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

		public function walk(): TreeCursor {}
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
	}

	final class SyntaxError extends Exception {
	}
}
