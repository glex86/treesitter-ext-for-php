<?php

use TreeSitter as TS;

class AstDumper {
    const PREFIX_LAST    = '└─ ';
    const PREFIX_NOTLAST = '├─ ';
    const PREFIX_PARENT  = '│   ';
    const PREFIX_EMPTY   = '   ';

    private TS\Node $node;

    public function __construct(TS\Node $node) {
        $this->node = $node;
        $this->dumpChildrens('', $node);
    }

    private function dumpChildrens(string $prefix, TS\Node $node) {
        //$childCount = $node->countNamedChildren();
        $childCount = $node->countChildren();

        for ($i=0; $i < $childCount; $i++) {
            $child = $node->getChild($i);
            if (!$child->isNamed()) {
                continue;
            }

            echo $prefix;
            $childPrefix = $prefix;
            if ($i === $childCount - 1) {
                echo self::PREFIX_LAST;
                $childPrefix .= self::PREFIX_EMPTY;
            } else {
                echo self::PREFIX_NOTLAST;
                $childPrefix .= self::PREFIX_PARENT;
            }
            $name = $node->getFieldNameForChild($i);
            echo $this->dumpNode($name, $child);

            echo "\n";

            if ($child->countNamedChildren() > 0) {
                $this->dumpChildrens($childPrefix, $child);
            }
        }
    }

    private function dumpNode(?string $name, TS\Node $node) {
        $startPoint = $node->getStartPoint();
        $endPoint = $node->getEndPoint();
#️⃣️ gfdg
#️⃣
#⃣
#️⃣️  =>  23 EF B8 8F E2 83 A3 EF B8 8F   (keycap hash emoji)
#⃣  =>  23 E2 83 A3                      (hash + combining enclosing keycap)
#️  =>  23 EF B8 8F                      (hash + variation selector-16)
        $text = match (true) {
            in_array($node->getType(), ['comment']) => '/* .. */',
            in_array($node->getType(), ['compound_statement', 'anonymous_function', 'arguments', 'method_declaration', 'assignment_expression']) => '',
            default => $node->getText(),
        };
        $line =  sprintf("[%s] %d:%d - %s: %s", $name ?? '--', $startPoint->row, $startPoint->column, $node->getType(), $text);
        return $line;
    }
}