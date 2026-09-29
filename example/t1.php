<?php
use TreeSitter as TS;
use TreeSitter\Node;

require_once __DIR__ ."/astDumper.php";


$parser = new TS\Parser(TS\Grammar::PHP);

$tree = $parser->parse(file_get_contents(__DIR__.'/JsonRpcApplication.php'));

$rootNode = $tree->getRootNode();


function nodeInfo(TS\Node $node): void {
    $startPoint = $node->getStartPoint();
    $endPoint = $node->getEndPoint();


    printf(
        "Pos: %d:%d - %d:%d, type: %s\n",
        $startPoint->row + 1,
        $startPoint->column + 1,
        $endPoint->row + 1,
        $endPoint->column + 1,
        $node->getType()
    );
}

//nodeInfo($rootNode);
//echo $rootNode; // (document (array (number) (null)))

// Recursive walk over the named nodes
function dump(TS\Node $node, int $depth = 0, int $maxDepth = 10): void {
    echo str_repeat('  ', $depth);
    nodeInfo($node);

    if ($depth >= $maxDepth) {
        return;
    }

    $childCount = $node->countNamedChildren();
    $c = 1;
    foreach ($node->getNamedChildren() as $child) {
        dump($child, $depth + 1, $maxDepth);
        $c++;
    }
    if ($childCount) {
        echo "\n";
    }
}

dump($rootNode);

// The same with a cursor: no Node objects are created until getCurrentNode() is called
$cursor = $tree->walk();
$count = 0;

while (true) {
    $count++;

    if ($cursor->gotoFirstChild()) {
        continue;
    }

    while (!$cursor->gotoNextSibling()) {
        if (!$cursor->gotoParent()) {
            break 2;
        }
    }
}

echo "Total nodes: $count\n";

$dumper = new AstDumper($rootNode);