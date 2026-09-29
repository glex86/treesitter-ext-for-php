<?php

declare(strict_types=1);

namespace Tree\Sitter\Language\Pack;

class AstRenderer {
    const PREFIX_LAST = '└── ';
    const PREFIX_NOTLAST = '├── ';
    const PREFIX_EMPTY = '    ';

    private $prefixes = [];

    private ProcessResult $result;

    private $lines = [];

    public function __construct(ProcessResult $result) {
        $this->result = $result;
    }

    public function render() {
        $this->addLine(sprintf('AST (%s)', $this->result->getLanguage()));
        $this->addPrefix('    ');

        $items = $this->result->getStructure();
        $this->renderStructureItems($items);

        echo implode("\n", $this->lines);
    }

    private function addPrefix(string $prefix): void {
        $this->prefixes[] = $prefix;
    }

    private function getPrefix(): string {
        return implode('', $this->prefixes);
    }

    private function addLine(string $line): void {
        $this->lines[] = $this->getPrefix() . $line;
    }


    private function renderStructureItems(array $items): void {
        $last = count($items) - 1;
        foreach ($items as $item) {
            $isLast = $item === $items[$last];
            $this->addLine(($isLast ? self::PREFIX_LAST : self::PREFIX_NOTLAST) . $this->formatItem($item, true));
            $childrens = $item->getChildren();
            if (count($childrens) > 0) {
                $this->addPrefix($isLast ? self::PREFIX_EMPTY : self::PREFIX_NOTLAST);
                $this->renderStructureItems($childrens);
                array_pop($this->prefixes);
            }
        }
    }

    private function formatItem(StructureItem $item, bool $includeDetails): string
    {
        $kind = $item->getKind();

        $kindName = $kind;

        $label = $kindName;
        if ($item->getName() !== null) {
            $label .= ' ' . $item->getName();
        }

        if (!$includeDetails) {
            return $label;
        }

        $span = $item->getSpan();
        $label .= sprintf(
            ' [%d:%d-%d:%d]',
            $span->getStartLine() + 1,
            $span->getStartColumn() + 1,
            $span->getEndLine() + 1,
            $span->getEndColumn() + 1
        );

        if ($item->getSignature() !== null) {
            $label .= ' — ' . trim($item->getSignature());
        }

        return $label;
    }
}