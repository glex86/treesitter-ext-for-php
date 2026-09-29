/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 713dc1f1ba6abbb7f99319baeb9b5867f49165d1 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Grammar_getQuery, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Grammar_getQueryNames, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_Parser___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, grammar, TreeSitter\\Grammar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Parser_parse, 0, 1, TreeSitter\\Tree, 0)
	ZEND_ARG_TYPE_INFO(0, source_code, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, old_tree, TreeSitter\\Tree, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Parser_parseWithErrors arginfo_class_TreeSitter_Parser_parse

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Parser_setIncludedRanges, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, ranges, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Parser_getIncludedRanges arginfo_class_TreeSitter_Grammar_getQueryNames

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Parser_reset, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Parser_setTimeoutMicros, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Parser_getTimeoutMicros, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Parser_getLanguage, 0, 0, TreeSitter\\Language, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_InputEdit___construct, 0, 0, 6)
	ZEND_ARG_TYPE_INFO(0, start_byte, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, old_end_byte, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new_end_byte, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, start_point, TreeSitter\\Point, 0)
	ZEND_ARG_OBJ_INFO(0, old_end_point, TreeSitter\\Point, 0)
	ZEND_ARG_OBJ_INFO(0, new_end_point, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_InputEdit_fromContentChange, 0, 6, TreeSitter\\InputEdit, 0)
	ZEND_ARG_TYPE_INFO(0, old_text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start_line, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start_character, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_line, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_character, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new_text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_Language___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Language_fromGrammar, 0, 1, TreeSitter\\Language, 0)
	ZEND_ARG_OBJ_INFO(0, grammar, TreeSitter\\Grammar, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Language_getAbiVersion arginfo_class_TreeSitter_Parser_getTimeoutMicros

#define arginfo_class_TreeSitter_Language_getSymbolCount arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Language_getSymbolName, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, symbol, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Language_getSymbolForName, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, named, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Language_getFieldName, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, field_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Language_getFieldIdForName, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Tree___construct arginfo_class_TreeSitter_Language___construct

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Tree_getRootNode, 0, 0, TreeSitter\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Tree_getSource, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Tree_walk, 0, 0, TreeSitter\\TreeCursor, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Tree_edit, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, edit, TreeSitter\\InputEdit, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Tree_copy, 0, 0, TreeSitter\\Tree, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Tree_getIncludedRanges arginfo_class_TreeSitter_Grammar_getQueryNames

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Tree_getChangedRanges, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, old, TreeSitter\\Tree, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Tree_byteToUtf16Column, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, byte_column, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Tree_utf16ToByteColumn, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, utf16_column, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node___construct arginfo_class_TreeSitter_Language___construct

#define arginfo_class_TreeSitter_Node_getType arginfo_class_TreeSitter_Tree_getSource

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Node_hasChildren, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_countChildren arginfo_class_TreeSitter_Parser_getTimeoutMicros

#define arginfo_class_TreeSitter_Node_getStartByte arginfo_class_TreeSitter_Parser_getTimeoutMicros

#define arginfo_class_TreeSitter_Node_getEndByte arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getStartPoint, 0, 0, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getEndPoint arginfo_class_TreeSitter_Node_getStartPoint

#define arginfo_class_TreeSitter_Node___toString arginfo_class_TreeSitter_Tree_getSource

#define arginfo_class_TreeSitter_Node_isNamed arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_Node_isMissing arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_Node_isExtra arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_Node_hasError arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_Node_countNamedChildren arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getChild, 0, 1, TreeSitter\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getNamedChild arginfo_class_TreeSitter_Node_getChild

#define arginfo_class_TreeSitter_Node_getChildren arginfo_class_TreeSitter_Grammar_getQueryNames

#define arginfo_class_TreeSitter_Node_getNamedChildren arginfo_class_TreeSitter_Grammar_getQueryNames

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getParent, 0, 0, TreeSitter\\\116ode, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getNextSibling arginfo_class_TreeSitter_Node_getParent

#define arginfo_class_TreeSitter_Node_getPrevSibling arginfo_class_TreeSitter_Node_getParent

#define arginfo_class_TreeSitter_Node_getNextNamedSibling arginfo_class_TreeSitter_Node_getParent

#define arginfo_class_TreeSitter_Node_getPrevNamedSibling arginfo_class_TreeSitter_Node_getParent

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getChildByFieldName, 0, 1, TreeSitter\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Node_getFieldNameForChild, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getText arginfo_class_TreeSitter_Tree_getSource

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_descendantForByteRange, 0, 2, TreeSitter\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, start_byte, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_byte, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_namedDescendantForByteRange arginfo_class_TreeSitter_Node_descendantForByteRange

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_descendantForPointRange, 0, 2, TreeSitter\\\116ode, 1)
	ZEND_ARG_OBJ_INFO(0, start, TreeSitter\\Point, 0)
	ZEND_ARG_OBJ_INFO(0, end, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_namedDescendantForPointRange arginfo_class_TreeSitter_Node_descendantForPointRange

#define arginfo_class_TreeSitter_Node_walk arginfo_class_TreeSitter_Tree_walk

#define arginfo_class_TreeSitter_Node_isError arginfo_class_TreeSitter_Node_hasChildren

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Node_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, other, TreeSitter\\\116ode, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getId arginfo_class_TreeSitter_Parser_getTimeoutMicros

#define arginfo_class_TreeSitter_Node_getKindId arginfo_class_TreeSitter_Parser_getTimeoutMicros

#define arginfo_class_TreeSitter_Node_getGrammarType arginfo_class_TreeSitter_Tree_getSource

#define arginfo_class_TreeSitter_Node_getDescendantCount arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getFirstChildForByte, 0, 1, TreeSitter\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, byte, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Node_getFirstNamedChildForByte arginfo_class_TreeSitter_Node_getFirstChildForByte

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getChildByFieldId, 0, 1, TreeSitter\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, field_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Node_getChildrenByFieldName, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_Node_getRange, 0, 0, TreeSitter\\Range, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_TreeCursor___construct arginfo_class_TreeSitter_Language___construct

#define arginfo_class_TreeSitter_TreeCursor_gotoFirstChild arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_TreeCursor_gotoLastChild arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_TreeCursor_gotoNextSibling arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_TreeCursor_gotoPreviousSibling arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_TreeCursor_gotoParent arginfo_class_TreeSitter_Node_hasChildren

#define arginfo_class_TreeSitter_TreeCursor_getCurrentNode arginfo_class_TreeSitter_Tree_getRootNode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_TreeCursor_getCurrentFieldName, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_TreeCursor_getDepth arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_TreeCursor_reset, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, node, TreeSitter\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_Point___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_Range___construct, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, start_byte, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_byte, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, start_point, TreeSitter\\Point, 0)
	ZEND_ARG_OBJ_INFO(0, end_point, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Range_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, point, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_Query___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, grammar, TreeSitter\\Grammar, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_Query_getCaptureNames arginfo_class_TreeSitter_Grammar_getQueryNames

#define arginfo_class_TreeSitter_Query_getPatternCount arginfo_class_TreeSitter_Parser_getTimeoutMicros

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Query_disablePattern, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, pattern_index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_Query_disableCapture, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TreeSitter_QueryCursor___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, query, TreeSitter\\Query, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_QueryCursor_exec arginfo_class_TreeSitter_TreeCursor_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_QueryCursor_setByteRange, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, start_byte, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_byte, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_QueryCursor_setPointRange, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, start, TreeSitter\\Point, 0)
	ZEND_ARG_OBJ_INFO(0, end, TreeSitter\\Point, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TreeSitter_QueryCursor_setMatchLimit, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_QueryCursor_didExceedMatchLimit arginfo_class_TreeSitter_Node_hasChildren

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_QueryCursor_nextMatch, 0, 0, TreeSitter\\QueryMatch, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TreeSitter_QueryCursor_nextCapture, 0, 0, TreeSitter\\QueryCapture, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_TreeSitter_QueryCursor_getMatches arginfo_class_TreeSitter_Grammar_getQueryNames

#define arginfo_class_TreeSitter_QueryCursor_getCaptures arginfo_class_TreeSitter_Grammar_getQueryNames

#define arginfo_class_TreeSitter_QueryMatch___construct arginfo_class_TreeSitter_Language___construct

#define arginfo_class_TreeSitter_QueryMatch_getNode arginfo_class_TreeSitter_Node_getChildByFieldName

#define arginfo_class_TreeSitter_QueryMatch_getNodes arginfo_class_TreeSitter_Node_getChildrenByFieldName

#define arginfo_class_TreeSitter_QueryCapture___construct arginfo_class_TreeSitter_Language___construct

ZEND_METHOD(TreeSitter_Grammar, getQuery);
ZEND_METHOD(TreeSitter_Grammar, getQueryNames);
ZEND_METHOD(TreeSitter_Parser, __construct);
ZEND_METHOD(TreeSitter_Parser, parse);
ZEND_METHOD(TreeSitter_Parser, parseWithErrors);
ZEND_METHOD(TreeSitter_Parser, setIncludedRanges);
ZEND_METHOD(TreeSitter_Parser, getIncludedRanges);
ZEND_METHOD(TreeSitter_Parser, reset);
ZEND_METHOD(TreeSitter_Parser, setTimeoutMicros);
ZEND_METHOD(TreeSitter_Parser, getTimeoutMicros);
ZEND_METHOD(TreeSitter_Parser, getLanguage);
ZEND_METHOD(TreeSitter_InputEdit, __construct);
ZEND_METHOD(TreeSitter_InputEdit, fromContentChange);
ZEND_METHOD(TreeSitter_Language, __construct);
ZEND_METHOD(TreeSitter_Language, fromGrammar);
ZEND_METHOD(TreeSitter_Language, getAbiVersion);
ZEND_METHOD(TreeSitter_Language, getSymbolCount);
ZEND_METHOD(TreeSitter_Language, getSymbolName);
ZEND_METHOD(TreeSitter_Language, getSymbolForName);
ZEND_METHOD(TreeSitter_Language, getFieldName);
ZEND_METHOD(TreeSitter_Language, getFieldIdForName);
ZEND_METHOD(TreeSitter_Tree, __construct);
ZEND_METHOD(TreeSitter_Tree, getRootNode);
ZEND_METHOD(TreeSitter_Tree, getSource);
ZEND_METHOD(TreeSitter_Tree, walk);
ZEND_METHOD(TreeSitter_Tree, edit);
ZEND_METHOD(TreeSitter_Tree, copy);
ZEND_METHOD(TreeSitter_Tree, getIncludedRanges);
ZEND_METHOD(TreeSitter_Tree, getChangedRanges);
ZEND_METHOD(TreeSitter_Tree, byteToUtf16Column);
ZEND_METHOD(TreeSitter_Tree, utf16ToByteColumn);
ZEND_METHOD(TreeSitter_Node, __construct);
ZEND_METHOD(TreeSitter_Node, getType);
ZEND_METHOD(TreeSitter_Node, hasChildren);
ZEND_METHOD(TreeSitter_Node, countChildren);
ZEND_METHOD(TreeSitter_Node, getStartByte);
ZEND_METHOD(TreeSitter_Node, getEndByte);
ZEND_METHOD(TreeSitter_Node, getStartPoint);
ZEND_METHOD(TreeSitter_Node, getEndPoint);
ZEND_METHOD(TreeSitter_Node, __toString);
ZEND_METHOD(TreeSitter_Node, isNamed);
ZEND_METHOD(TreeSitter_Node, isMissing);
ZEND_METHOD(TreeSitter_Node, isExtra);
ZEND_METHOD(TreeSitter_Node, hasError);
ZEND_METHOD(TreeSitter_Node, countNamedChildren);
ZEND_METHOD(TreeSitter_Node, getChild);
ZEND_METHOD(TreeSitter_Node, getNamedChild);
ZEND_METHOD(TreeSitter_Node, getChildren);
ZEND_METHOD(TreeSitter_Node, getNamedChildren);
ZEND_METHOD(TreeSitter_Node, getParent);
ZEND_METHOD(TreeSitter_Node, getNextSibling);
ZEND_METHOD(TreeSitter_Node, getPrevSibling);
ZEND_METHOD(TreeSitter_Node, getNextNamedSibling);
ZEND_METHOD(TreeSitter_Node, getPrevNamedSibling);
ZEND_METHOD(TreeSitter_Node, getChildByFieldName);
ZEND_METHOD(TreeSitter_Node, getFieldNameForChild);
ZEND_METHOD(TreeSitter_Node, getText);
ZEND_METHOD(TreeSitter_Node, descendantForByteRange);
ZEND_METHOD(TreeSitter_Node, namedDescendantForByteRange);
ZEND_METHOD(TreeSitter_Node, descendantForPointRange);
ZEND_METHOD(TreeSitter_Node, namedDescendantForPointRange);
ZEND_METHOD(TreeSitter_Node, walk);
ZEND_METHOD(TreeSitter_Node, isError);
ZEND_METHOD(TreeSitter_Node, equals);
ZEND_METHOD(TreeSitter_Node, getId);
ZEND_METHOD(TreeSitter_Node, getKindId);
ZEND_METHOD(TreeSitter_Node, getGrammarType);
ZEND_METHOD(TreeSitter_Node, getDescendantCount);
ZEND_METHOD(TreeSitter_Node, getFirstChildForByte);
ZEND_METHOD(TreeSitter_Node, getFirstNamedChildForByte);
ZEND_METHOD(TreeSitter_Node, getChildByFieldId);
ZEND_METHOD(TreeSitter_Node, getChildrenByFieldName);
ZEND_METHOD(TreeSitter_Node, getRange);
ZEND_METHOD(TreeSitter_TreeCursor, __construct);
ZEND_METHOD(TreeSitter_TreeCursor, gotoFirstChild);
ZEND_METHOD(TreeSitter_TreeCursor, gotoLastChild);
ZEND_METHOD(TreeSitter_TreeCursor, gotoNextSibling);
ZEND_METHOD(TreeSitter_TreeCursor, gotoPreviousSibling);
ZEND_METHOD(TreeSitter_TreeCursor, gotoParent);
ZEND_METHOD(TreeSitter_TreeCursor, getCurrentNode);
ZEND_METHOD(TreeSitter_TreeCursor, getCurrentFieldName);
ZEND_METHOD(TreeSitter_TreeCursor, getDepth);
ZEND_METHOD(TreeSitter_TreeCursor, reset);
ZEND_METHOD(TreeSitter_Point, __construct);
ZEND_METHOD(TreeSitter_Range, __construct);
ZEND_METHOD(TreeSitter_Range, contains);
ZEND_METHOD(TreeSitter_Query, __construct);
ZEND_METHOD(TreeSitter_Query, getCaptureNames);
ZEND_METHOD(TreeSitter_Query, getPatternCount);
ZEND_METHOD(TreeSitter_Query, disablePattern);
ZEND_METHOD(TreeSitter_Query, disableCapture);
ZEND_METHOD(TreeSitter_QueryCursor, __construct);
ZEND_METHOD(TreeSitter_QueryCursor, exec);
ZEND_METHOD(TreeSitter_QueryCursor, setByteRange);
ZEND_METHOD(TreeSitter_QueryCursor, setPointRange);
ZEND_METHOD(TreeSitter_QueryCursor, setMatchLimit);
ZEND_METHOD(TreeSitter_QueryCursor, didExceedMatchLimit);
ZEND_METHOD(TreeSitter_QueryCursor, nextMatch);
ZEND_METHOD(TreeSitter_QueryCursor, nextCapture);
ZEND_METHOD(TreeSitter_QueryCursor, getMatches);
ZEND_METHOD(TreeSitter_QueryCursor, getCaptures);
ZEND_METHOD(TreeSitter_QueryMatch, __construct);
ZEND_METHOD(TreeSitter_QueryMatch, getNode);
ZEND_METHOD(TreeSitter_QueryMatch, getNodes);
ZEND_METHOD(TreeSitter_QueryCapture, __construct);

static const zend_function_entry class_TreeSitter_Grammar_methods[] = {
	ZEND_ME(TreeSitter_Grammar, getQuery, arginfo_class_TreeSitter_Grammar_getQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Grammar, getQueryNames, arginfo_class_TreeSitter_Grammar_getQueryNames, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Parser_methods[] = {
	ZEND_ME(TreeSitter_Parser, __construct, arginfo_class_TreeSitter_Parser___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, parse, arginfo_class_TreeSitter_Parser_parse, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, parseWithErrors, arginfo_class_TreeSitter_Parser_parseWithErrors, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, setIncludedRanges, arginfo_class_TreeSitter_Parser_setIncludedRanges, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, getIncludedRanges, arginfo_class_TreeSitter_Parser_getIncludedRanges, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, reset, arginfo_class_TreeSitter_Parser_reset, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, setTimeoutMicros, arginfo_class_TreeSitter_Parser_setTimeoutMicros, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, getTimeoutMicros, arginfo_class_TreeSitter_Parser_getTimeoutMicros, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Parser, getLanguage, arginfo_class_TreeSitter_Parser_getLanguage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_InputEdit_methods[] = {
	ZEND_ME(TreeSitter_InputEdit, __construct, arginfo_class_TreeSitter_InputEdit___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_InputEdit, fromContentChange, arginfo_class_TreeSitter_InputEdit_fromContentChange, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Language_methods[] = {
	ZEND_ME(TreeSitter_Language, __construct, arginfo_class_TreeSitter_Language___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, fromGrammar, arginfo_class_TreeSitter_Language_fromGrammar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(TreeSitter_Language, getAbiVersion, arginfo_class_TreeSitter_Language_getAbiVersion, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, getSymbolCount, arginfo_class_TreeSitter_Language_getSymbolCount, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, getSymbolName, arginfo_class_TreeSitter_Language_getSymbolName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, getSymbolForName, arginfo_class_TreeSitter_Language_getSymbolForName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, getFieldName, arginfo_class_TreeSitter_Language_getFieldName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Language, getFieldIdForName, arginfo_class_TreeSitter_Language_getFieldIdForName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Tree_methods[] = {
	ZEND_ME(TreeSitter_Tree, __construct, arginfo_class_TreeSitter_Tree___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, getRootNode, arginfo_class_TreeSitter_Tree_getRootNode, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, getSource, arginfo_class_TreeSitter_Tree_getSource, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, walk, arginfo_class_TreeSitter_Tree_walk, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, edit, arginfo_class_TreeSitter_Tree_edit, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, copy, arginfo_class_TreeSitter_Tree_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, getIncludedRanges, arginfo_class_TreeSitter_Tree_getIncludedRanges, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, getChangedRanges, arginfo_class_TreeSitter_Tree_getChangedRanges, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, byteToUtf16Column, arginfo_class_TreeSitter_Tree_byteToUtf16Column, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Tree, utf16ToByteColumn, arginfo_class_TreeSitter_Tree_utf16ToByteColumn, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Node_methods[] = {
	ZEND_ME(TreeSitter_Node, __construct, arginfo_class_TreeSitter_Node___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getType, arginfo_class_TreeSitter_Node_getType, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, hasChildren, arginfo_class_TreeSitter_Node_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, countChildren, arginfo_class_TreeSitter_Node_countChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getStartByte, arginfo_class_TreeSitter_Node_getStartByte, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getEndByte, arginfo_class_TreeSitter_Node_getEndByte, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getStartPoint, arginfo_class_TreeSitter_Node_getStartPoint, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getEndPoint, arginfo_class_TreeSitter_Node_getEndPoint, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, __toString, arginfo_class_TreeSitter_Node___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, isNamed, arginfo_class_TreeSitter_Node_isNamed, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, isMissing, arginfo_class_TreeSitter_Node_isMissing, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, isExtra, arginfo_class_TreeSitter_Node_isExtra, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, hasError, arginfo_class_TreeSitter_Node_hasError, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, countNamedChildren, arginfo_class_TreeSitter_Node_countNamedChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getChild, arginfo_class_TreeSitter_Node_getChild, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getNamedChild, arginfo_class_TreeSitter_Node_getNamedChild, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getChildren, arginfo_class_TreeSitter_Node_getChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getNamedChildren, arginfo_class_TreeSitter_Node_getNamedChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getParent, arginfo_class_TreeSitter_Node_getParent, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getNextSibling, arginfo_class_TreeSitter_Node_getNextSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getPrevSibling, arginfo_class_TreeSitter_Node_getPrevSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getNextNamedSibling, arginfo_class_TreeSitter_Node_getNextNamedSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getPrevNamedSibling, arginfo_class_TreeSitter_Node_getPrevNamedSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getChildByFieldName, arginfo_class_TreeSitter_Node_getChildByFieldName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getFieldNameForChild, arginfo_class_TreeSitter_Node_getFieldNameForChild, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getText, arginfo_class_TreeSitter_Node_getText, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, descendantForByteRange, arginfo_class_TreeSitter_Node_descendantForByteRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, namedDescendantForByteRange, arginfo_class_TreeSitter_Node_namedDescendantForByteRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, descendantForPointRange, arginfo_class_TreeSitter_Node_descendantForPointRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, namedDescendantForPointRange, arginfo_class_TreeSitter_Node_namedDescendantForPointRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, walk, arginfo_class_TreeSitter_Node_walk, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, isError, arginfo_class_TreeSitter_Node_isError, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, equals, arginfo_class_TreeSitter_Node_equals, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getId, arginfo_class_TreeSitter_Node_getId, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getKindId, arginfo_class_TreeSitter_Node_getKindId, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getGrammarType, arginfo_class_TreeSitter_Node_getGrammarType, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getDescendantCount, arginfo_class_TreeSitter_Node_getDescendantCount, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getFirstChildForByte, arginfo_class_TreeSitter_Node_getFirstChildForByte, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getFirstNamedChildForByte, arginfo_class_TreeSitter_Node_getFirstNamedChildForByte, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getChildByFieldId, arginfo_class_TreeSitter_Node_getChildByFieldId, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getChildrenByFieldName, arginfo_class_TreeSitter_Node_getChildrenByFieldName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Node, getRange, arginfo_class_TreeSitter_Node_getRange, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_TreeCursor_methods[] = {
	ZEND_ME(TreeSitter_TreeCursor, __construct, arginfo_class_TreeSitter_TreeCursor___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, gotoFirstChild, arginfo_class_TreeSitter_TreeCursor_gotoFirstChild, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, gotoLastChild, arginfo_class_TreeSitter_TreeCursor_gotoLastChild, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, gotoNextSibling, arginfo_class_TreeSitter_TreeCursor_gotoNextSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, gotoPreviousSibling, arginfo_class_TreeSitter_TreeCursor_gotoPreviousSibling, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, gotoParent, arginfo_class_TreeSitter_TreeCursor_gotoParent, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, getCurrentNode, arginfo_class_TreeSitter_TreeCursor_getCurrentNode, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, getCurrentFieldName, arginfo_class_TreeSitter_TreeCursor_getCurrentFieldName, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, getDepth, arginfo_class_TreeSitter_TreeCursor_getDepth, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_TreeCursor, reset, arginfo_class_TreeSitter_TreeCursor_reset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Point_methods[] = {
	ZEND_ME(TreeSitter_Point, __construct, arginfo_class_TreeSitter_Point___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Range_methods[] = {
	ZEND_ME(TreeSitter_Range, __construct, arginfo_class_TreeSitter_Range___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Range, contains, arginfo_class_TreeSitter_Range_contains, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_Query_methods[] = {
	ZEND_ME(TreeSitter_Query, __construct, arginfo_class_TreeSitter_Query___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Query, getCaptureNames, arginfo_class_TreeSitter_Query_getCaptureNames, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Query, getPatternCount, arginfo_class_TreeSitter_Query_getPatternCount, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Query, disablePattern, arginfo_class_TreeSitter_Query_disablePattern, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_Query, disableCapture, arginfo_class_TreeSitter_Query_disableCapture, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_QueryCursor_methods[] = {
	ZEND_ME(TreeSitter_QueryCursor, __construct, arginfo_class_TreeSitter_QueryCursor___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, exec, arginfo_class_TreeSitter_QueryCursor_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, setByteRange, arginfo_class_TreeSitter_QueryCursor_setByteRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, setPointRange, arginfo_class_TreeSitter_QueryCursor_setPointRange, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, setMatchLimit, arginfo_class_TreeSitter_QueryCursor_setMatchLimit, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, didExceedMatchLimit, arginfo_class_TreeSitter_QueryCursor_didExceedMatchLimit, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, nextMatch, arginfo_class_TreeSitter_QueryCursor_nextMatch, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, nextCapture, arginfo_class_TreeSitter_QueryCursor_nextCapture, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, getMatches, arginfo_class_TreeSitter_QueryCursor_getMatches, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryCursor, getCaptures, arginfo_class_TreeSitter_QueryCursor_getCaptures, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_QueryMatch_methods[] = {
	ZEND_ME(TreeSitter_QueryMatch, __construct, arginfo_class_TreeSitter_QueryMatch___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryMatch, getNode, arginfo_class_TreeSitter_QueryMatch_getNode, ZEND_ACC_PUBLIC)
	ZEND_ME(TreeSitter_QueryMatch, getNodes, arginfo_class_TreeSitter_QueryMatch_getNodes, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_TreeSitter_QueryCapture_methods[] = {
	ZEND_ME(TreeSitter_QueryCapture, __construct, arginfo_class_TreeSitter_QueryCapture___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_TreeSitter_Grammar(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("TreeSitter\\Grammar", IS_LONG, class_TreeSitter_Grammar_methods);

	zval enum_case_PHP_value;
	ZVAL_LONG(&enum_case_PHP_value, 0x706870);
	zend_enum_add_case_cstr(class_entry, "PHP", &enum_case_PHP_value);

	zval enum_case_PHP_ONLY_value;
	ZVAL_LONG(&enum_case_PHP_ONLY_value, 0x7068705f6f6e6c79);
	zend_enum_add_case_cstr(class_entry, "PHP_ONLY", &enum_case_PHP_ONLY_value);

	zval enum_case_HTML_value;
	ZVAL_LONG(&enum_case_HTML_value, 0x68746d6c);
	zend_enum_add_case_cstr(class_entry, "HTML", &enum_case_HTML_value);

	zval enum_case_CSS_value;
	ZVAL_LONG(&enum_case_CSS_value, 0x637373);
	zend_enum_add_case_cstr(class_entry, "CSS", &enum_case_CSS_value);

	zval enum_case_JAVASCRIPT_value;
	ZVAL_LONG(&enum_case_JAVASCRIPT_value, 0x6a73);
	zend_enum_add_case_cstr(class_entry, "JAVASCRIPT", &enum_case_JAVASCRIPT_value);

	zval enum_case_TYPESCRIPT_value;
	ZVAL_LONG(&enum_case_TYPESCRIPT_value, 0x7473);
	zend_enum_add_case_cstr(class_entry, "TYPESCRIPT", &enum_case_TYPESCRIPT_value);

	zval enum_case_PYTHON_value;
	ZVAL_LONG(&enum_case_PYTHON_value, 0x707974686f6e);
	zend_enum_add_case_cstr(class_entry, "PYTHON", &enum_case_PYTHON_value);

	zval enum_case_JSON_value;
	ZVAL_LONG(&enum_case_JSON_value, 0x6a736f6e);
	zend_enum_add_case_cstr(class_entry, "JSON", &enum_case_JSON_value);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Parser(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Parser", class_TreeSitter_Parser_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_InputEdit(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "InputEdit", class_TreeSitter_InputEdit_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval property_startByte_default_value;
	ZVAL_UNDEF(&property_startByte_default_value);
	zend_string *property_startByte_name = zend_string_init("startByte", sizeof("startByte") - 1, 1);
	zend_declare_typed_property(class_entry, property_startByte_name, &property_startByte_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_startByte_name);

	zval property_oldEndByte_default_value;
	ZVAL_UNDEF(&property_oldEndByte_default_value);
	zend_string *property_oldEndByte_name = zend_string_init("oldEndByte", sizeof("oldEndByte") - 1, 1);
	zend_declare_typed_property(class_entry, property_oldEndByte_name, &property_oldEndByte_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_oldEndByte_name);

	zval property_newEndByte_default_value;
	ZVAL_UNDEF(&property_newEndByte_default_value);
	zend_string *property_newEndByte_name = zend_string_init("newEndByte", sizeof("newEndByte") - 1, 1);
	zend_declare_typed_property(class_entry, property_newEndByte_name, &property_newEndByte_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_newEndByte_name);

	zval property_startPoint_default_value;
	ZVAL_UNDEF(&property_startPoint_default_value);
	zend_string *property_startPoint_name = zend_string_init("startPoint", sizeof("startPoint") - 1, 1);
	zend_string *property_startPoint_class_TreeSitter_Point = zend_string_init("TreeSitter\\Point", sizeof("TreeSitter\\Point")-1, 1);
	zend_declare_typed_property(class_entry, property_startPoint_name, &property_startPoint_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_startPoint_class_TreeSitter_Point, 0, 0));
	zend_string_release(property_startPoint_name);

	zval property_oldEndPoint_default_value;
	ZVAL_UNDEF(&property_oldEndPoint_default_value);
	zend_string *property_oldEndPoint_name = zend_string_init("oldEndPoint", sizeof("oldEndPoint") - 1, 1);
	zend_string *property_oldEndPoint_class_TreeSitter_Point = zend_string_init("TreeSitter\\Point", sizeof("TreeSitter\\Point")-1, 1);
	zend_declare_typed_property(class_entry, property_oldEndPoint_name, &property_oldEndPoint_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_oldEndPoint_class_TreeSitter_Point, 0, 0));
	zend_string_release(property_oldEndPoint_name);

	zval property_newEndPoint_default_value;
	ZVAL_UNDEF(&property_newEndPoint_default_value);
	zend_string *property_newEndPoint_name = zend_string_init("newEndPoint", sizeof("newEndPoint") - 1, 1);
	zend_string *property_newEndPoint_class_TreeSitter_Point = zend_string_init("TreeSitter\\Point", sizeof("TreeSitter\\Point")-1, 1);
	zend_declare_typed_property(class_entry, property_newEndPoint_name, &property_newEndPoint_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_newEndPoint_class_TreeSitter_Point, 0, 0));
	zend_string_release(property_newEndPoint_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Language(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Language", class_TreeSitter_Language_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Tree(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Tree", class_TreeSitter_Tree_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Node(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Node", class_TreeSitter_Node_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_TreeCursor(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "TreeCursor", class_TreeSitter_TreeCursor_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Point(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Point", class_TreeSitter_Point_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval property_row_default_value;
	ZVAL_UNDEF(&property_row_default_value);
	zend_string *property_row_name = zend_string_init("row", sizeof("row") - 1, 1);
	zend_declare_typed_property(class_entry, property_row_name, &property_row_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_row_name);

	zval property_column_default_value;
	ZVAL_UNDEF(&property_column_default_value);
	zend_string *property_column_name = zend_string_init("column", sizeof("column") - 1, 1);
	zend_declare_typed_property(class_entry, property_column_name, &property_column_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_column_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Range(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Range", class_TreeSitter_Range_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval property_startByte_default_value;
	ZVAL_UNDEF(&property_startByte_default_value);
	zend_string *property_startByte_name = zend_string_init("startByte", sizeof("startByte") - 1, 1);
	zend_declare_typed_property(class_entry, property_startByte_name, &property_startByte_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_startByte_name);

	zval property_endByte_default_value;
	ZVAL_UNDEF(&property_endByte_default_value);
	zend_string *property_endByte_name = zend_string_init("endByte", sizeof("endByte") - 1, 1);
	zend_declare_typed_property(class_entry, property_endByte_name, &property_endByte_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_endByte_name);

	zval property_startPoint_default_value;
	ZVAL_UNDEF(&property_startPoint_default_value);
	zend_string *property_startPoint_name = zend_string_init("startPoint", sizeof("startPoint") - 1, 1);
	zend_string *property_startPoint_class_TreeSitter_Point = zend_string_init("TreeSitter\\Point", sizeof("TreeSitter\\Point")-1, 1);
	zend_declare_typed_property(class_entry, property_startPoint_name, &property_startPoint_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_startPoint_class_TreeSitter_Point, 0, 0));
	zend_string_release(property_startPoint_name);

	zval property_endPoint_default_value;
	ZVAL_UNDEF(&property_endPoint_default_value);
	zend_string *property_endPoint_name = zend_string_init("endPoint", sizeof("endPoint") - 1, 1);
	zend_string *property_endPoint_class_TreeSitter_Point = zend_string_init("TreeSitter\\Point", sizeof("TreeSitter\\Point")-1, 1);
	zend_declare_typed_property(class_entry, property_endPoint_name, &property_endPoint_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_endPoint_class_TreeSitter_Point, 0, 0));
	zend_string_release(property_endPoint_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Query(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Query", class_TreeSitter_Query_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_QueryCursor(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "QueryCursor", class_TreeSitter_QueryCursor_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_QueryMatch(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "QueryMatch", class_TreeSitter_QueryMatch_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval property_patternIndex_default_value;
	ZVAL_UNDEF(&property_patternIndex_default_value);
	zend_string *property_patternIndex_name = zend_string_init("patternIndex", sizeof("patternIndex") - 1, 1);
	zend_declare_typed_property(class_entry, property_patternIndex_name, &property_patternIndex_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_patternIndex_name);

	zval property_captures_default_value;
	ZVAL_UNDEF(&property_captures_default_value);
	zend_string *property_captures_name = zend_string_init("captures", sizeof("captures") - 1, 1);
	zend_declare_typed_property(class_entry, property_captures_name, &property_captures_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_captures_name);

	zval property_properties_default_value;
	ZVAL_UNDEF(&property_properties_default_value);
	zend_string *property_properties_name = zend_string_init("properties", sizeof("properties") - 1, 1);
	zend_declare_typed_property(class_entry, property_properties_name, &property_properties_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_properties_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_QueryCapture(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "QueryCapture", class_TreeSitter_QueryCapture_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_node_default_value;
	ZVAL_UNDEF(&property_node_default_value);
	zend_string *property_node_name = zend_string_init("node", sizeof("node") - 1, 1);
	zend_string *property_node_class_TreeSitter_Node = zend_string_init("TreeSitter\\\116ode", sizeof("TreeSitter\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_node_name, &property_node_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_node_class_TreeSitter_Node, 0, 0));
	zend_string_release(property_node_name);

	zval property_index_default_value;
	ZVAL_UNDEF(&property_index_default_value);
	zend_string *property_index_name = zend_string_init("index", sizeof("index") - 1, 1);
	zend_declare_typed_property(class_entry, property_index_name, &property_index_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_index_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_Exception(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "Exception", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_QueryException(zend_class_entry *class_entry_TreeSitter_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "QueryException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_TreeSitter_Exception, 0);

	zval property_offset_default_value;
	ZVAL_LONG(&property_offset_default_value, 0);
	zend_string *property_offset_name = zend_string_init("offset", sizeof("offset") - 1, 1);
	zend_declare_typed_property(class_entry, property_offset_name, &property_offset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_offset_name);

	zval property_errorType_default_value;
	ZVAL_EMPTY_STRING(&property_errorType_default_value);
	zend_string *property_errorType_name = zend_string_init("errorType", sizeof("errorType") - 1, 1);
	zend_declare_typed_property(class_entry, property_errorType_name, &property_errorType_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_errorType_name);

	return class_entry;
}

static zend_class_entry *register_class_TreeSitter_SyntaxError(zend_class_entry *class_entry_TreeSitter_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TreeSitter", "SyntaxError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_TreeSitter_Exception, ZEND_ACC_FINAL);

	return class_entry;
}
