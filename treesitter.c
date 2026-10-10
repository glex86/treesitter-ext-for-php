#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "tree_sitter/api.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "zend_enum.h"
#include <ctype.h>
#include "zend_hrtime.h"

#include "php_treesitter.h"
#include "lib/parsers/parsers.h"
#include "lib/parsers/queries.gen.h"
#include "treesitter_arginfo.h"

/**
 * TODO
 *
 * To implement query:
 *
 *    - ts_query_new
 *    - ...
 */

static zend_class_entry *php_treesitter_exception_ce = NULL;
static zend_class_entry *php_treesitter_syntax_error_ce = NULL;

/**
 * Maps a TreeSitter\Grammar constant to its TSLanguage, NULL for unknown grammars.
 */
static const TSLanguage *php_treesitter_language_for_grammar(zend_long grammar)
{
    switch (grammar) {
        case PHP_TREESITTER_GRAMMAR_PHP:
            return tree_sitter_php();
        case PHP_TREESITTER_GRAMMAR_PHP_ONLY:
            return tree_sitter_php_only();
        case PHP_TREESITTER_GRAMMAR_PHPDOC:
            return tree_sitter_phpdoc();
        case PHP_TREESITTER_GRAMMAR_HTML:
            return tree_sitter_html();
        case PHP_TREESITTER_GRAMMAR_CSS:
            return tree_sitter_css();
        case PHP_TREESITTER_GRAMMAR_JAVASCRIPT:
            return tree_sitter_javascript();
        case PHP_TREESITTER_GRAMMAR_TYPESCRIPT:
            return tree_sitter_typescript();
        case PHP_TREESITTER_GRAMMAR_PYTHON:
            return tree_sitter_python();
        case PHP_TREESITTER_GRAMMAR_JSON:
            return tree_sitter_json();
        default:
            return NULL;
    }
}

static zend_class_entry *php_treesitter_grammar_ce = NULL;

/**
 * The number behind a TreeSitter\Grammar case.
 */
static zend_long php_treesitter_grammar_value(zend_object *grammar)
{
    return Z_LVAL_P(zend_enum_fetch_case_value(grammar));
}

/**
 * The numbers of the enum cases are written in the stub (the stub generator cannot take them from
 * C constants), the constants of parsers.h are what the C code switches on. They must be the same.
 */
static void php_treesitter_check_grammar_values(void)
{
    static const struct {
        const char *name;
        zend_long value;
    } expected[] = {
        { "PHP", PHP_TREESITTER_GRAMMAR_PHP },
        { "PHP_ONLY", PHP_TREESITTER_GRAMMAR_PHP_ONLY },
        { "PHPDOC", PHP_TREESITTER_GRAMMAR_PHPDOC },
        { "HTML", PHP_TREESITTER_GRAMMAR_HTML },
        { "CSS", PHP_TREESITTER_GRAMMAR_CSS },
        { "JAVASCRIPT", PHP_TREESITTER_GRAMMAR_JAVASCRIPT },
        { "TYPESCRIPT", PHP_TREESITTER_GRAMMAR_TYPESCRIPT },
        { "PYTHON", PHP_TREESITTER_GRAMMAR_PYTHON },
        { "JSON", PHP_TREESITTER_GRAMMAR_JSON },
    };

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        zend_object *grammar = zend_enum_get_case_cstr(php_treesitter_grammar_ce, expected[i].name);

        if (php_treesitter_grammar_value(grammar) != expected[i].value) {
            zend_error_noreturn(E_CORE_ERROR, "treesitter: the value of Grammar::%s in the stub differs from lib/parsers/parsers.h", expected[i].name);
        }
    }
}

ZEND_METHOD(TreeSitter_Grammar, getQuery)
{
    zend_string *name;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    zend_long grammar = php_treesitter_grammar_value(Z_OBJ_P(ZEND_THIS));

    for (size_t i = 0; i < sizeof(php_treesitter_embedded_queries) / sizeof(php_treesitter_embedded_queries[0]); i++) {
        const php_treesitter_embedded_query *query = &php_treesitter_embedded_queries[i];

        if (query->grammar == grammar && ZSTR_LEN(name) == strlen(query->name) && memcmp(ZSTR_VAL(name), query->name, ZSTR_LEN(name)) == 0) {
            RETURN_STRINGL(query->source, query->length);
        }
    }

    RETURN_NULL();
}

ZEND_METHOD(TreeSitter_Grammar, getQueryNames)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_long grammar = php_treesitter_grammar_value(Z_OBJ_P(ZEND_THIS));

    array_init(return_value);

    for (size_t i = 0; i < sizeof(php_treesitter_embedded_queries) / sizeof(php_treesitter_embedded_queries[0]); i++) {
        const php_treesitter_embedded_query *query = &php_treesitter_embedded_queries[i];

        if (query->grammar == grammar) {
            add_next_index_string(return_value, query->name);
        }
    }
}

static zend_class_entry *php_treesitter_point_ce = NULL;
static zend_object_handlers php_treesitter_point_handlers;

static zend_object *php_treesitter_point_object_create(zend_class_entry *ce)
{
    zend_object *php_treesitter_point_object = zend_object_alloc(sizeof(zend_object), ce);

    zend_object_std_init(php_treesitter_point_object, ce);
    object_properties_init(php_treesitter_point_object, ce);

    return php_treesitter_point_object;
}

ZEND_METHOD(TreeSitter_Point, __construct)
{
    zend_long row, column;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(row)
        Z_PARAM_LONG(column)
    ZEND_PARSE_PARAMETERS_END();

    if (row < 0 || row > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    if (column < 0 || column > UINT32_MAX) {
        zend_argument_value_error(2, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    zend_update_property_long(php_treesitter_point_ce, Z_OBJ_P(ZEND_THIS), "row", sizeof("row") - 1, row);
    zend_update_property_long(php_treesitter_point_ce, Z_OBJ_P(ZEND_THIS), "column", sizeof("column") - 1, column);
}

static void php_treesitter_point_object_free(zend_object *obj)
{
    zend_object_std_dtor(obj);
}

static void php_treesitter_point_create(zval *return_value, TSPoint point)
{
    object_init_ex(return_value, php_treesitter_point_ce);

    zend_update_property_long(php_treesitter_point_ce, Z_OBJ_P(return_value), "row", sizeof("row") - 1, point.row);
    zend_update_property_long(php_treesitter_point_ce, Z_OBJ_P(return_value), "column", sizeof("column") - 1, point.column);
}

/**
 * Reads a TreeSitter\Point object into a TSPoint. Throws and returns false
 * when a property is uninitialized or out of the uint32 range.
 */
static bool php_treesitter_point_read(zval *point_zv, TSPoint *point)
{
    zval rv, *row, *column;

    row = zend_read_property(php_treesitter_point_ce, Z_OBJ_P(point_zv), "row", sizeof("row") - 1, 1, &rv);
    if (Z_TYPE_P(row) != IS_LONG || Z_LVAL_P(row) < 0 || Z_LVAL_P(row) > UINT32_MAX) {
        zend_value_error("Point::$row must be an integer between 0 and 4294967295");
        return false;
    }
    point->row = (uint32_t) Z_LVAL_P(row);

    column = zend_read_property(php_treesitter_point_ce, Z_OBJ_P(point_zv), "column", sizeof("column") - 1, 1, &rv);
    if (Z_TYPE_P(column) != IS_LONG || Z_LVAL_P(column) < 0 || Z_LVAL_P(column) > UINT32_MAX) {
        zend_value_error("Point::$column must be an integer between 0 and 4294967295");
        return false;
    }
    point->column = (uint32_t) Z_LVAL_P(column);

    return true;
}

/*
 * Range
 */

static zend_class_entry *php_treesitter_range_ce = NULL;

ZEND_METHOD(TreeSitter_Range, __construct)
{
    zend_long start_byte, end_byte;
    zval *start_point, *end_point;

    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_LONG(start_byte)
        Z_PARAM_LONG(end_byte)
        Z_PARAM_OBJECT_OF_CLASS(start_point, php_treesitter_point_ce)
        Z_PARAM_OBJECT_OF_CLASS(end_point, php_treesitter_point_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (start_byte < 0 || start_byte > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    if (end_byte > UINT32_MAX) {
        zend_argument_value_error(2, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    if (end_byte < start_byte) {
        zend_argument_value_error(2, "must be greater than or equal to argument #1 ($start_byte)");
        RETURN_THROWS();
    }

    zend_update_property_long(php_treesitter_range_ce, Z_OBJ_P(ZEND_THIS), "startByte", sizeof("startByte") - 1, start_byte);
    zend_update_property_long(php_treesitter_range_ce, Z_OBJ_P(ZEND_THIS), "endByte", sizeof("endByte") - 1, end_byte);
    zend_update_property(php_treesitter_range_ce, Z_OBJ_P(ZEND_THIS), "startPoint", sizeof("startPoint") - 1, start_point);
    zend_update_property(php_treesitter_range_ce, Z_OBJ_P(ZEND_THIS), "endPoint", sizeof("endPoint") - 1, end_point);
}

static void php_treesitter_range_create_from_range(zval *return_value, TSRange range)
{
    zval start_point, end_point;

    php_treesitter_point_create(&start_point, range.start_point);
    php_treesitter_point_create(&end_point, range.end_point);

    object_init_ex(return_value, php_treesitter_range_ce);

    zend_update_property_long(php_treesitter_range_ce, Z_OBJ_P(return_value), "startByte", sizeof("startByte") - 1, range.start_byte);
    zend_update_property_long(php_treesitter_range_ce, Z_OBJ_P(return_value), "endByte", sizeof("endByte") - 1, range.end_byte);
    zend_update_property(php_treesitter_range_ce, Z_OBJ_P(return_value), "startPoint", sizeof("startPoint") - 1, &start_point);
    zend_update_property(php_treesitter_range_ce, Z_OBJ_P(return_value), "endPoint", sizeof("endPoint") - 1, &end_point);

    zval_ptr_dtor(&start_point);
    zval_ptr_dtor(&end_point);
}

static void php_treesitter_range_create(zval *return_value, TSNode node)
{
    TSRange range = {
        .start_point = ts_node_start_point(node),
        .end_point = ts_node_end_point(node),
        .start_byte = ts_node_start_byte(node),
        .end_byte = ts_node_end_byte(node),
    };

    php_treesitter_range_create_from_range(return_value, range);
}

/*
 * InputEdit
 */

static zend_class_entry *php_treesitter_input_edit_ce = NULL;

static bool php_treesitter_point_is_before(TSPoint a, TSPoint b)
{
    return a.row < b.row || (a.row == b.row && a.column < b.column);
}

/**
 * Validates the values of an edit: every position within the uint32 range,
 * and the end of the old and of the new text not before the start.
 */
static bool php_treesitter_input_edit_validate(
    zend_long start_byte, zend_long old_end_byte, zend_long new_end_byte,
    TSPoint start_point, TSPoint old_end_point, TSPoint new_end_point, bool as_arguments)
{
    const char *messages[] = {
        "must be between 0 and 4294967295",
        "must be between $start_byte and 4294967295",
        "must be between $start_byte and 4294967295",
        "the old end point must not be before the start point",
        "the new end point must not be before the start point",
    };
    int argument = 0;
    const char *problem = NULL;

    if (start_byte < 0 || start_byte > UINT32_MAX) {
        argument = 1;
        problem = messages[0];
    } else if (old_end_byte < start_byte || old_end_byte > UINT32_MAX) {
        argument = 2;
        problem = messages[1];
    } else if (new_end_byte < start_byte || new_end_byte > UINT32_MAX) {
        argument = 3;
        problem = messages[2];
    } else if (php_treesitter_point_is_before(old_end_point, start_point)) {
        argument = 5;
        problem = messages[3];
    } else if (php_treesitter_point_is_before(new_end_point, start_point)) {
        argument = 6;
        problem = messages[4];
    }

    if (problem == NULL) {
        return true;
    }

    if (as_arguments) {
        zend_argument_value_error(argument, "%s", problem);
    } else {
        zend_value_error("Invalid InputEdit: %s", problem);
    }

    return false;
}

static void php_treesitter_input_edit_create(zval *return_value, const TSInputEdit *edit)
{
    zval start_point, old_end_point, new_end_point;

    php_treesitter_point_create(&start_point, edit->start_point);
    php_treesitter_point_create(&old_end_point, edit->old_end_point);
    php_treesitter_point_create(&new_end_point, edit->new_end_point);

    object_init_ex(return_value, php_treesitter_input_edit_ce);

    zend_update_property_long(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "startByte", sizeof("startByte") - 1, edit->start_byte);
    zend_update_property_long(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "oldEndByte", sizeof("oldEndByte") - 1, edit->old_end_byte);
    zend_update_property_long(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "newEndByte", sizeof("newEndByte") - 1, edit->new_end_byte);
    zend_update_property(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "startPoint", sizeof("startPoint") - 1, &start_point);
    zend_update_property(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "oldEndPoint", sizeof("oldEndPoint") - 1, &old_end_point);
    zend_update_property(php_treesitter_input_edit_ce, Z_OBJ_P(return_value), "newEndPoint", sizeof("newEndPoint") - 1, &new_end_point);

    zval_ptr_dtor(&start_point);
    zval_ptr_dtor(&old_end_point);
    zval_ptr_dtor(&new_end_point);
}

/**
 * Reads a TreeSitter\Range object (its properties are public, so they may have been changed)
 * into a TSRange. Throws and returns false if it is not initialized or not valid.
 */
static bool php_treesitter_range_read(zval *range_zv, TSRange *range)
{
    zval rv, *start_byte, *end_byte, *start_point, *end_point;
    zend_object *object = Z_OBJ_P(range_zv);
    zend_class_entry *ce = php_treesitter_range_ce;

    start_byte = zend_read_property(ce, object, "startByte", sizeof("startByte") - 1, 1, &rv);
    end_byte = zend_read_property(ce, object, "endByte", sizeof("endByte") - 1, 1, &rv);
    start_point = zend_read_property(ce, object, "startPoint", sizeof("startPoint") - 1, 1, &rv);
    end_point = zend_read_property(ce, object, "endPoint", sizeof("endPoint") - 1, 1, &rv);

    if (Z_TYPE_P(start_byte) != IS_LONG || Z_TYPE_P(end_byte) != IS_LONG
        || Z_TYPE_P(start_point) != IS_OBJECT || Z_TYPE_P(end_point) != IS_OBJECT) {
        zend_throw_error(NULL, "The Range is not initialized");
        return false;
    }

    if (Z_LVAL_P(start_byte) < 0 || Z_LVAL_P(start_byte) > UINT32_MAX
        || Z_LVAL_P(end_byte) < Z_LVAL_P(start_byte) || Z_LVAL_P(end_byte) > UINT32_MAX) {
        zend_value_error("Invalid Range: the bytes must be between 0 and 4294967295 and the end must not be before the start");
        return false;
    }

    if (!php_treesitter_point_read(start_point, &range->start_point) || !php_treesitter_point_read(end_point, &range->end_point)) {
        return false;
    }

    if (php_treesitter_point_is_before(range->end_point, range->start_point)) {
        zend_value_error("Invalid Range: the end point must not be before the start point");
        return false;
    }

    range->start_byte = (uint32_t) Z_LVAL_P(start_byte);
    range->end_byte = (uint32_t) Z_LVAL_P(end_byte);

    return true;
}

ZEND_METHOD(TreeSitter_Range, contains)
{
    zval *point_zv;
    TSRange range;
    TSPoint point;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(point_zv, php_treesitter_point_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_range_read(ZEND_THIS, &range) || !php_treesitter_point_read(point_zv, &point)) {
        RETURN_THROWS();
    }

    RETURN_BOOL(!php_treesitter_point_is_before(point, range.start_point) && php_treesitter_point_is_before(point, range.end_point));
}

ZEND_METHOD(TreeSitter_InputEdit, __construct)
{
    zend_long start_byte, old_end_byte, new_end_byte;
    zval *start_point_zv, *old_end_point_zv, *new_end_point_zv;
    TSPoint start_point, old_end_point, new_end_point;

    ZEND_PARSE_PARAMETERS_START(6, 6)
        Z_PARAM_LONG(start_byte)
        Z_PARAM_LONG(old_end_byte)
        Z_PARAM_LONG(new_end_byte)
        Z_PARAM_OBJECT_OF_CLASS(start_point_zv, php_treesitter_point_ce)
        Z_PARAM_OBJECT_OF_CLASS(old_end_point_zv, php_treesitter_point_ce)
        Z_PARAM_OBJECT_OF_CLASS(new_end_point_zv, php_treesitter_point_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_point_read(start_point_zv, &start_point)
        || !php_treesitter_point_read(old_end_point_zv, &old_end_point)
        || !php_treesitter_point_read(new_end_point_zv, &new_end_point)) {
        RETURN_THROWS();
    }

    if (!php_treesitter_input_edit_validate(start_byte, old_end_byte, new_end_byte, start_point, old_end_point, new_end_point, true)) {
        RETURN_THROWS();
    }

    zend_object *object = Z_OBJ_P(ZEND_THIS);

    zend_update_property_long(php_treesitter_input_edit_ce, object, "startByte", sizeof("startByte") - 1, start_byte);
    zend_update_property_long(php_treesitter_input_edit_ce, object, "oldEndByte", sizeof("oldEndByte") - 1, old_end_byte);
    zend_update_property_long(php_treesitter_input_edit_ce, object, "newEndByte", sizeof("newEndByte") - 1, new_end_byte);
    zend_update_property(php_treesitter_input_edit_ce, object, "startPoint", sizeof("startPoint") - 1, start_point_zv);
    zend_update_property(php_treesitter_input_edit_ce, object, "oldEndPoint", sizeof("oldEndPoint") - 1, old_end_point_zv);
    zend_update_property(php_treesitter_input_edit_ce, object, "newEndPoint", sizeof("newEndPoint") - 1, new_end_point_zv);
}

/**
 * Reads the (public, so possibly modified) properties of an InputEdit. Throws and returns false
 * if the edit is not valid or not initialized.
 */
static bool php_treesitter_input_edit_read(zval *edit_zv, TSInputEdit *edit)
{
    zval rv, *start_byte, *old_end_byte, *new_end_byte, *start_point, *old_end_point, *new_end_point;
    zend_object *object = Z_OBJ_P(edit_zv);
    zend_class_entry *ce = php_treesitter_input_edit_ce;

    start_byte = zend_read_property(ce, object, "startByte", sizeof("startByte") - 1, 1, &rv);
    if (Z_TYPE_P(start_byte) != IS_LONG) {
        zend_throw_error(NULL, "The InputEdit is not initialized");
        return false;
    }
    zend_long start_byte_value = Z_LVAL_P(start_byte);

    old_end_byte = zend_read_property(ce, object, "oldEndByte", sizeof("oldEndByte") - 1, 1, &rv);
    new_end_byte = zend_read_property(ce, object, "newEndByte", sizeof("newEndByte") - 1, 1, &rv);
    start_point = zend_read_property(ce, object, "startPoint", sizeof("startPoint") - 1, 1, &rv);
    old_end_point = zend_read_property(ce, object, "oldEndPoint", sizeof("oldEndPoint") - 1, 1, &rv);
    new_end_point = zend_read_property(ce, object, "newEndPoint", sizeof("newEndPoint") - 1, 1, &rv);

    if (Z_TYPE_P(old_end_byte) != IS_LONG || Z_TYPE_P(new_end_byte) != IS_LONG
        || Z_TYPE_P(start_point) != IS_OBJECT || Z_TYPE_P(old_end_point) != IS_OBJECT || Z_TYPE_P(new_end_point) != IS_OBJECT) {
        zend_throw_error(NULL, "The InputEdit is not initialized");
        return false;
    }

    if (!php_treesitter_point_read(start_point, &edit->start_point)
        || !php_treesitter_point_read(old_end_point, &edit->old_end_point)
        || !php_treesitter_point_read(new_end_point, &edit->new_end_point)) {
        return false;
    }

    if (!php_treesitter_input_edit_validate(
            start_byte_value, Z_LVAL_P(old_end_byte), Z_LVAL_P(new_end_byte),
            edit->start_point, edit->old_end_point, edit->new_end_point, false)) {
        return false;
    }

    edit->start_byte = (uint32_t) start_byte_value;
    edit->old_end_byte = (uint32_t) Z_LVAL_P(old_end_byte);
    edit->new_end_byte = (uint32_t) Z_LVAL_P(new_end_byte);

    return true;
}

/*
 * Language
 */

static zend_class_entry *php_treesitter_language_ce = NULL;
static zend_object_handlers php_treesitter_language_handlers;

typedef struct {
    const TSLanguage *language; /* owned by the statically linked grammar, never freed */
    zend_object std;
} php_treesitter_language_object;

static inline php_treesitter_language_object *php_treesitter_language_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_language_object *)((char *)obj - XtOffsetOf(php_treesitter_language_object, std));
}

static zend_object *php_treesitter_language_object_create(zend_class_entry *ce)
{
    php_treesitter_language_object *obj = zend_object_alloc(sizeof(php_treesitter_language_object), ce);

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

static void php_treesitter_language_object_free(zend_object *obj)
{
    zend_object_std_dtor(obj);
}

static void php_treesitter_language_wrap(zval *return_value, const TSLanguage *language)
{
    object_init_ex(return_value, php_treesitter_language_ce);

    php_treesitter_language_object_from_zend_object(Z_OBJ_P(return_value))->language = language;
}

ZEND_METHOD(TreeSitter_Language, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

ZEND_METHOD(TreeSitter_Language, fromGrammar)
{
    zval *grammar_zv;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(grammar_zv, php_treesitter_grammar_ce)
    ZEND_PARSE_PARAMETERS_END();

    const TSLanguage *language = php_treesitter_language_for_grammar(php_treesitter_grammar_value(Z_OBJ_P(grammar_zv)));

    if (language == NULL) {
        zend_throw_error(NULL, "Unknown grammar");
        RETURN_THROWS();
    }

    php_treesitter_language_wrap(return_value, language);
}

ZEND_METHOD(TreeSitter_Language, getAbiVersion)
{
    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(ts_language_abi_version(language->language));
}

ZEND_METHOD(TreeSitter_Language, getSymbolCount)
{
    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(ts_language_symbol_count(language->language));
}

ZEND_METHOD(TreeSitter_Language, getSymbolName)
{
    zend_long symbol;

    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(symbol)
    ZEND_PARSE_PARAMETERS_END();

    if (symbol < 0 || (zend_ulong) symbol >= ts_language_symbol_count(language->language)) {
        RETURN_NULL();
    }

    const char *name = ts_language_symbol_name(language->language, (TSSymbol) symbol);

    if (name == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(name);
}

ZEND_METHOD(TreeSitter_Language, getSymbolForName)
{
    zend_string *name;
    bool named;

    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(name)
        Z_PARAM_BOOL(named)
    ZEND_PARSE_PARAMETERS_END();

    /* 0 is also the id of the "end" symbol, which is never a valid lookup result for a name */
    RETURN_LONG(ts_language_symbol_for_name(language->language, ZSTR_VAL(name), (uint32_t) ZSTR_LEN(name), named));
}

ZEND_METHOD(TreeSitter_Language, getFieldName)
{
    zend_long field_id;

    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(field_id)
    ZEND_PARSE_PARAMETERS_END();

    if (field_id <= 0 || field_id > UINT16_MAX) {
        RETURN_NULL();
    }

    const char *name = ts_language_field_name_for_id(language->language, (TSFieldId) field_id);

    if (name == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(name);
}

ZEND_METHOD(TreeSitter_Language, getFieldIdForName)
{
    zend_string *name;

    php_treesitter_language_object *language = php_treesitter_language_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    /* 0 means "no such field" */
    RETURN_LONG(ts_language_field_id_for_name(language->language, ZSTR_VAL(name), (uint32_t) ZSTR_LEN(name)));
}

/*
 * Tree
 *
 * The tree owns the TSTree and a copy of the source code. Every Node and
 * TreeCursor keeps a reference to the Tree object they belong to, since a
 * TSNode is only a handle into the memory owned by the TSTree.
 */

static zend_class_entry *php_treesitter_tree_ce = NULL;
static zend_object_handlers php_treesitter_tree_handlers;

typedef struct {
    TSTree *tree;
    zend_string *source;
    bool edited; /* Tree::edit() was called: the positions do not match the source anymore */
    /*
     * Copies of the tree from before each edit. ts_tree_edit() rewrites the subtrees that Node and
     * cursor objects point at (and frees the old ones when nothing else shares them). A retained copy
     * keeps the pre-edit subtrees alive and untouched for as long as the Tree object lives.
     */
    TSTree **retired;
    uint32_t retired_count;
    uint32_t retired_capacity;
    zend_object std;
} php_treesitter_tree_object;

static inline php_treesitter_tree_object *php_treesitter_tree_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_tree_object *)((char *)obj - XtOffsetOf(php_treesitter_tree_object, std));
}

static zend_object *php_treesitter_tree_object_create(zend_class_entry *ce)
{
    php_treesitter_tree_object *obj = zend_object_alloc(sizeof(php_treesitter_tree_object), ce);

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

/**
 * Wraps a TSTree (which the new object owns) and the source it was parsed from.
 */
static void php_treesitter_tree_create(zval *return_value, TSTree *ts_tree, zend_string *source, bool edited)
{
    object_init_ex(return_value, php_treesitter_tree_ce);

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(return_value));

    tree->tree = ts_tree;
    tree->source = zend_string_copy(source);
    tree->edited = edited;
}

/**
 * The source of an edited tree is out of date; whatever combines its positions with the source has to refuse it.
 */
static bool php_treesitter_tree_ensure_current(php_treesitter_tree_object *tree)
{
    if (tree->edited) {
        zend_throw_error(NULL, "The tree was edited, parse the new source with it to get a current tree");
        return false;
    }

    return true;
}

static void php_treesitter_tree_object_free(zend_object *obj)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(obj);

    if (tree->tree) {
        ts_tree_delete(tree->tree);
    }

    for (uint32_t i = 0; i < tree->retired_count; i++) {
        ts_tree_delete(tree->retired[i]);
    }

    if (tree->retired) {
        efree(tree->retired);
    }

    if (tree->source) {
        zend_string_release(tree->source);
    }

    zend_object_std_dtor(obj);
}

/*
 * Node
 */

static zend_class_entry *php_treesitter_node_ce = NULL;
static zend_object_handlers php_treesitter_node_handlers;

typedef struct {
    TSNode node;
    zval tree; /* the TreeSitter\Tree this node belongs to */
    zend_object std;
} php_treesitter_node_object;

static inline php_treesitter_node_object *php_treesitter_node_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_node_object *)((char *)obj - XtOffsetOf(php_treesitter_node_object, std));
}

static zend_object *php_treesitter_node_object_create(zend_class_entry *ce)
{
    php_treesitter_node_object *obj = zend_object_alloc(sizeof(php_treesitter_node_object), ce);

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

/**
 * Wraps a TSNode into a TreeSitter\Node object that keeps its Tree alive.
 */
static void php_treesitter_node_wrap(zval *return_value, TSNode ts_node, zval *tree)
{
    object_init_ex(return_value, php_treesitter_node_ce);

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(return_value));

    node->node = ts_node;
    ZVAL_COPY(&node->tree, tree);
}

/**
 * Like php_treesitter_node_wrap, but yields null for the "null node" that
 * tree-sitter returns when there is no such node (no parent, no sibling, ...).
 */
static void php_treesitter_node_wrap_or_null(zval *return_value, TSNode ts_node, zval *tree)
{
    if (ts_node_is_null(ts_node)) {
        ZVAL_NULL(return_value);
        return;
    }

    php_treesitter_node_wrap(return_value, ts_node, tree);
}

static int php_treesitter_node_compare(zval *o1, zval *o2)
{
    ZEND_COMPARE_OBJECTS_FALLBACK(o1, o2);

    php_treesitter_node_object *a = php_treesitter_node_object_from_zend_object(Z_OBJ_P(o1));
    php_treesitter_node_object *b = php_treesitter_node_object_from_zend_object(Z_OBJ_P(o2));

    return ts_node_eq(a->node, b->node) ? 0 : ZEND_UNCOMPARABLE;
}

typedef bool (*php_treesitter_node_predicate)(TSNode);
typedef TSNode (*php_treesitter_node_navigation)(TSNode);

static void php_treesitter_node_test(INTERNAL_FUNCTION_PARAMETERS, php_treesitter_node_predicate predicate)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(predicate(node->node));
}

static void php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAMETERS, php_treesitter_node_navigation navigate)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_node_wrap_or_null(return_value, navigate(node->node), &node->tree);
}

static void php_treesitter_node_collect_children(zval *return_value, php_treesitter_node_object *node, bool named_only)
{
    array_init(return_value);

    /* ts_node_child(i) is O(i), a cursor makes the whole walk O(n) */
    TSTreeCursor cursor = ts_tree_cursor_new(node->node);

    if (ts_tree_cursor_goto_first_child(&cursor)) {
        do {
            TSNode child = ts_tree_cursor_current_node(&cursor);

            if (named_only && !ts_node_is_named(child)) {
                continue;
            }

            zval child_zv;
            php_treesitter_node_wrap(&child_zv, child, &node->tree);
            add_next_index_zval(return_value, &child_zv);
        } while (ts_tree_cursor_goto_next_sibling(&cursor));
    }

    ts_tree_cursor_delete(&cursor);
}

static void php_treesitter_cursor_create(zval *return_value, TSNode ts_node, zval *tree);

ZEND_METHOD(TreeSitter_Node, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

ZEND_METHOD(TreeSitter_Node, hasChildren)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZVAL_BOOL(return_value, ts_node_child_count(node->node) > 0);
}

ZEND_METHOD(TreeSitter_Node, countChildren)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZVAL_LONG(return_value, ts_node_child_count(node->node));
}

ZEND_METHOD(TreeSitter_Node, countNamedChildren)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    ZVAL_LONG(return_value, ts_node_named_child_count(node->node));
}

ZEND_METHOD(TreeSitter_Node, getType)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    const char *type = ts_node_type(node->node);

    zend_string *return_string = zend_string_init(type, strlen(type), 0);
    RETURN_NEW_STR(return_string);
}

ZEND_METHOD(TreeSitter_Node, getStartByte)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZVAL_LONG(return_value, ts_node_start_byte(node->node));
}

ZEND_METHOD(TreeSitter_Node, getEndByte)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZVAL_LONG(return_value, ts_node_end_byte(node->node));
}

ZEND_METHOD(TreeSitter_Node, getStartPoint)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    TSPoint point = ts_node_start_point(node->node);

    php_treesitter_point_create(return_value, point);
}

ZEND_METHOD(TreeSitter_Node, getEndPoint)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    TSPoint point = ts_node_end_point(node->node);

    php_treesitter_point_create(return_value, point);
}

ZEND_METHOD(TreeSitter_Node, __toString)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    char *string = ts_node_string(node->node);

    zend_string *return_string = zend_string_init(string, strlen(string), 0);
    free(string);

    RETURN_NEW_STR(return_string);
}

ZEND_METHOD(TreeSitter_Node, isNamed)
{
    php_treesitter_node_test(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_is_named);
}

ZEND_METHOD(TreeSitter_Node, isMissing)
{
    php_treesitter_node_test(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_is_missing);
}

ZEND_METHOD(TreeSitter_Node, isExtra)
{
    php_treesitter_node_test(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_is_extra);
}

ZEND_METHOD(TreeSitter_Node, hasError)
{
    php_treesitter_node_test(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_has_error);
}

ZEND_METHOD(TreeSitter_Node, getChild)
{
    zend_long index;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(index)
    ZEND_PARSE_PARAMETERS_END();

    if (index < 0 || (zend_ulong) index >= ts_node_child_count(node->node)) {
        RETURN_NULL();
    }

    php_treesitter_node_wrap_or_null(return_value, ts_node_child(node->node, (uint32_t) index), &node->tree);
}

ZEND_METHOD(TreeSitter_Node, getNamedChild)
{
    zend_long index;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(index)
    ZEND_PARSE_PARAMETERS_END();

    if (index < 0 || (zend_ulong) index >= ts_node_named_child_count(node->node)) {
        RETURN_NULL();
    }

    php_treesitter_node_wrap_or_null(return_value, ts_node_named_child(node->node, (uint32_t) index), &node->tree);
}

ZEND_METHOD(TreeSitter_Node, getChildren)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_node_collect_children(return_value, node, false);
}

ZEND_METHOD(TreeSitter_Node, getNamedChildren)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_node_collect_children(return_value, node, true);
}

ZEND_METHOD(TreeSitter_Node, getParent)
{
    php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_parent);
}

ZEND_METHOD(TreeSitter_Node, getNextSibling)
{
    php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_next_sibling);
}

ZEND_METHOD(TreeSitter_Node, getPrevSibling)
{
    php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_prev_sibling);
}

ZEND_METHOD(TreeSitter_Node, getNextNamedSibling)
{
    php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_next_named_sibling);
}

ZEND_METHOD(TreeSitter_Node, getPrevNamedSibling)
{
    php_treesitter_node_navigate(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_prev_named_sibling);
}

ZEND_METHOD(TreeSitter_Node, getChildByFieldName)
{
    zend_string *name;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_node_wrap_or_null(
        return_value,
        ts_node_child_by_field_name(node->node, ZSTR_VAL(name), (uint32_t) ZSTR_LEN(name)),
        &node->tree
    );
}

ZEND_METHOD(TreeSitter_Node, getFieldNameForChild)
{
    zend_long index;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(index)
    ZEND_PARSE_PARAMETERS_END();

    if (index < 0 || (zend_ulong) index >= ts_node_child_count(node->node)) {
        RETURN_NULL();
    }

    const char *field_name = ts_node_field_name_for_child(node->node, (uint32_t) index);

    if (field_name == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(field_name);
}

ZEND_METHOD(TreeSitter_Node, getText)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ(node->tree));

    if (!php_treesitter_tree_ensure_current(tree)) {
        RETURN_THROWS();
    }

    uint32_t start = ts_node_start_byte(node->node);
    uint32_t end = ts_node_end_byte(node->node);

    if (tree->source == NULL || start > end || end > ZSTR_LEN(tree->source)) {
        RETURN_EMPTY_STRING();
    }

    RETURN_STRINGL(ZSTR_VAL(tree->source) + start, end - start);
}

static void php_treesitter_node_descendant_for_byte_range(INTERNAL_FUNCTION_PARAMETERS, bool named_only)
{
    zend_long start_byte, end_byte;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(start_byte)
        Z_PARAM_LONG(end_byte)
    ZEND_PARSE_PARAMETERS_END();

    if (start_byte < 0 || start_byte > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    if (end_byte < start_byte || end_byte > UINT32_MAX) {
        zend_argument_value_error(2, "must be greater than or equal to argument #1 ($start_byte)");
        RETURN_THROWS();
    }

    TSNode descendant = named_only
        ? ts_node_named_descendant_for_byte_range(node->node, (uint32_t) start_byte, (uint32_t) end_byte)
        : ts_node_descendant_for_byte_range(node->node, (uint32_t) start_byte, (uint32_t) end_byte);

    php_treesitter_node_wrap_or_null(return_value, descendant, &node->tree);
}

ZEND_METHOD(TreeSitter_Node, descendantForByteRange)
{
    php_treesitter_node_descendant_for_byte_range(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(TreeSitter_Node, namedDescendantForByteRange)
{
    php_treesitter_node_descendant_for_byte_range(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static int php_treesitter_point_compare(TSPoint a, TSPoint b)
{
    if (a.row != b.row) {
        return a.row < b.row ? -1 : 1;
    }

    if (a.column != b.column) {
        return a.column < b.column ? -1 : 1;
    }

    return 0;
}

static void php_treesitter_node_descendant_for_point_range(INTERNAL_FUNCTION_PARAMETERS, bool named_only)
{
    zval *start_zv, *end_zv;
    TSPoint start, end;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(start_zv, php_treesitter_point_ce)
        Z_PARAM_OBJECT_OF_CLASS(end_zv, php_treesitter_point_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_point_read(start_zv, &start) || !php_treesitter_point_read(end_zv, &end)) {
        RETURN_THROWS();
    }

    if (php_treesitter_point_compare(end, start) < 0) {
        zend_argument_value_error(2, "must not be before argument #1 ($start)");
        RETURN_THROWS();
    }

    TSNode descendant = named_only
        ? ts_node_named_descendant_for_point_range(node->node, start, end)
        : ts_node_descendant_for_point_range(node->node, start, end);

    php_treesitter_node_wrap_or_null(return_value, descendant, &node->tree);
}

ZEND_METHOD(TreeSitter_Node, descendantForPointRange)
{
    php_treesitter_node_descendant_for_point_range(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(TreeSitter_Node, namedDescendantForPointRange)
{
    php_treesitter_node_descendant_for_point_range(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(TreeSitter_Node, walk)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_cursor_create(return_value, node->node, &node->tree);
}

ZEND_METHOD(TreeSitter_Node, isError)
{
    php_treesitter_node_test(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_node_is_error);
}

ZEND_METHOD(TreeSitter_Node, equals)
{
    zval *other_zv;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(other_zv, php_treesitter_node_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_node_object *other = php_treesitter_node_object_from_zend_object(Z_OBJ_P(other_zv));

    RETURN_BOOL(ts_node_eq(node->node, other->node));
}

ZEND_METHOD(TreeSitter_Node, getId)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    /* the id is the address of the node inside its tree, unique only within one Tree */
    RETURN_LONG((zend_long) (uintptr_t) node->node.id);
}

ZEND_METHOD(TreeSitter_Node, getKindId)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(ts_node_symbol(node->node));
}

ZEND_METHOD(TreeSitter_Node, getGrammarType)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_STRING(ts_node_grammar_type(node->node));
}

ZEND_METHOD(TreeSitter_Node, getDescendantCount)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(ts_node_descendant_count(node->node));
}

static void php_treesitter_node_child_for_byte(INTERNAL_FUNCTION_PARAMETERS, bool named_only)
{
    zend_long byte;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(byte)
    ZEND_PARSE_PARAMETERS_END();

    if (byte < 0 || byte > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    TSNode child = named_only
        ? ts_node_first_named_child_for_byte(node->node, (uint32_t) byte)
        : ts_node_first_child_for_byte(node->node, (uint32_t) byte);

    php_treesitter_node_wrap_or_null(return_value, child, &node->tree);
}

ZEND_METHOD(TreeSitter_Node, getFirstChildForByte)
{
    php_treesitter_node_child_for_byte(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(TreeSitter_Node, getFirstNamedChildForByte)
{
    php_treesitter_node_child_for_byte(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(TreeSitter_Node, getChildByFieldId)
{
    zend_long field_id;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(field_id)
    ZEND_PARSE_PARAMETERS_END();

    if (field_id <= 0 || field_id > UINT16_MAX) {
        RETURN_NULL();
    }

    php_treesitter_node_wrap_or_null(return_value, ts_node_child_by_field_id(node->node, (TSFieldId) field_id), &node->tree);
}

ZEND_METHOD(TreeSitter_Node, getChildrenByFieldName)
{
    zend_string *name;

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    array_init(return_value);

    TSFieldId field_id = ts_language_field_id_for_name(ts_node_language(node->node), ZSTR_VAL(name), (uint32_t) ZSTR_LEN(name));

    if (field_id == 0) {
        return;
    }

    TSTreeCursor cursor = ts_tree_cursor_new(node->node);

    if (ts_tree_cursor_goto_first_child(&cursor)) {
        do {
            if (ts_tree_cursor_current_field_id(&cursor) != field_id) {
                continue;
            }

            zval child_zv;
            php_treesitter_node_wrap(&child_zv, ts_tree_cursor_current_node(&cursor), &node->tree);
            add_next_index_zval(return_value, &child_zv);
        } while (ts_tree_cursor_goto_next_sibling(&cursor));
    }

    ts_tree_cursor_delete(&cursor);
}

ZEND_METHOD(TreeSitter_Node, getRange)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_range_create(return_value, node->node);
}

static void php_treesitter_node_object_free(zend_object *obj)
{
    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(obj);

    zval_ptr_dtor(&node->tree);

    zend_object_std_dtor(obj);
}

/*
 * TreeCursor
 */

static zend_class_entry *php_treesitter_cursor_ce = NULL;
static zend_object_handlers php_treesitter_cursor_handlers;

typedef struct {
    TSTreeCursor cursor;
    bool initialized;
    zval tree; /* the TreeSitter\Tree the cursor walks */
    zend_object std;
} php_treesitter_cursor_object;

static inline php_treesitter_cursor_object *php_treesitter_cursor_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_cursor_object *)((char *)obj - XtOffsetOf(php_treesitter_cursor_object, std));
}

static zend_object *php_treesitter_cursor_object_create(zend_class_entry *ce)
{
    php_treesitter_cursor_object *obj = zend_object_alloc(sizeof(php_treesitter_cursor_object), ce);

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

static void php_treesitter_cursor_create(zval *return_value, TSNode ts_node, zval *tree)
{
    object_init_ex(return_value, php_treesitter_cursor_ce);

    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(return_value));

    cursor->cursor = ts_tree_cursor_new(ts_node);
    cursor->initialized = true;
    ZVAL_COPY(&cursor->tree, tree);
}

typedef bool (*php_treesitter_cursor_movement)(TSTreeCursor *);

static void php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAMETERS, php_treesitter_cursor_movement move)
{
    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(move(&cursor->cursor));
}

ZEND_METHOD(TreeSitter_TreeCursor, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

ZEND_METHOD(TreeSitter_TreeCursor, gotoFirstChild)
{
    php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_tree_cursor_goto_first_child);
}

ZEND_METHOD(TreeSitter_TreeCursor, gotoLastChild)
{
    php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_tree_cursor_goto_last_child);
}

ZEND_METHOD(TreeSitter_TreeCursor, gotoNextSibling)
{
    php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_tree_cursor_goto_next_sibling);
}

ZEND_METHOD(TreeSitter_TreeCursor, gotoPreviousSibling)
{
    php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_tree_cursor_goto_previous_sibling);
}

ZEND_METHOD(TreeSitter_TreeCursor, gotoParent)
{
    php_treesitter_cursor_move(INTERNAL_FUNCTION_PARAM_PASSTHRU, ts_tree_cursor_goto_parent);
}

ZEND_METHOD(TreeSitter_TreeCursor, getCurrentNode)
{
    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_node_wrap(return_value, ts_tree_cursor_current_node(&cursor->cursor), &cursor->tree);
}

ZEND_METHOD(TreeSitter_TreeCursor, getCurrentFieldName)
{
    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    const char *field_name = ts_tree_cursor_current_field_name(&cursor->cursor);

    if (field_name == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(field_name);
}

ZEND_METHOD(TreeSitter_TreeCursor, getDepth)
{
    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(ts_tree_cursor_current_depth(&cursor->cursor));
}

ZEND_METHOD(TreeSitter_TreeCursor, reset)
{
    zval *node_zv;

    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(node_zv, php_treesitter_node_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(node_zv));

    /* the node may belong to another tree than the one the cursor was created for */
    zval_ptr_dtor(&cursor->tree);
    ZVAL_COPY(&cursor->tree, &node->tree);

    ts_tree_cursor_reset(&cursor->cursor, node->node);
}

static void php_treesitter_cursor_object_free(zend_object *obj)
{
    php_treesitter_cursor_object *cursor = php_treesitter_cursor_object_from_zend_object(obj);

    if (cursor->initialized) {
        ts_tree_cursor_delete(&cursor->cursor);
    }

    zval_ptr_dtor(&cursor->tree);

    zend_object_std_dtor(obj);
}

/*
 * Tree methods (they need Node and TreeCursor)
 */

ZEND_METHOD(TreeSitter_Tree, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

ZEND_METHOD(TreeSitter_Tree, getRootNode)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_node_wrap(return_value, ts_tree_root_node(tree->tree), ZEND_THIS);
}

ZEND_METHOD(TreeSitter_Tree, getSource)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_STR_COPY(tree->source);
}

ZEND_METHOD(TreeSitter_Tree, walk)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_cursor_create(return_value, ts_tree_root_node(tree->tree), ZEND_THIS);
}

ZEND_METHOD(TreeSitter_Tree, edit)
{
    zval *edit_zv;
    TSInputEdit edit;

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(edit_zv, php_treesitter_input_edit_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_input_edit_read(edit_zv, &edit)) {
        RETURN_THROWS();
    }

    /* nodes and cursors created before the edit must keep pointing at valid, unchanged subtrees */
    if (tree->retired_count == tree->retired_capacity) {
        tree->retired_capacity = tree->retired_capacity ? tree->retired_capacity * 2 : 4;
        tree->retired = safe_erealloc(tree->retired, tree->retired_capacity, sizeof(TSTree *), 0);
    }

    tree->retired[tree->retired_count++] = ts_tree_copy(tree->tree);

    ts_tree_edit(tree->tree, &edit);
    tree->edited = true;
}

ZEND_METHOD(TreeSitter_Tree, copy)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_tree_create(return_value, ts_tree_copy(tree->tree), tree->source, tree->edited);
}

ZEND_METHOD(TreeSitter_Tree, getIncludedRanges)
{
    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    uint32_t length = 0;
    TSRange *ranges = ts_tree_included_ranges(tree->tree, &length);

    array_init_size(return_value, length);

    for (uint32_t i = 0; i < length; i++) {
        zval range_zv;

        php_treesitter_range_create_from_range(&range_zv, ranges[i]);
        add_next_index_zval(return_value, &range_zv);
    }

    free(ranges);
}

ZEND_METHOD(TreeSitter_Tree, getChangedRanges)
{
    zval *old_zv;

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(old_zv, php_treesitter_tree_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_tree_object *old = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(old_zv));

    if (ts_tree_language(old->tree) != ts_tree_language(tree->tree)) {
        zend_argument_value_error(1, "must have been parsed with the same grammar");
        RETURN_THROWS();
    }

    uint32_t length = 0;
    TSRange *ranges = ts_tree_get_changed_ranges(old->tree, tree->tree, &length);

    array_init_size(return_value, length);

    for (uint32_t i = 0; i < length; i++) {
        zval range_zv;

        php_treesitter_range_create_from_range(&range_zv, ranges[i]);
        add_next_index_zval(return_value, &range_zv);
    }

    free(ranges);
}

/**
 * Finds the byte range [start, end) of the given row in the source, without
 * the terminating "\n". Rows are counted like tree-sitter does: by "\n" only.
 */
static bool php_treesitter_source_line(zend_string *source, zend_long row, size_t *start, size_t *end)
{
    const char *data = ZSTR_VAL(source);
    size_t length = ZSTR_LEN(source);
    size_t line_start = 0;

    for (zend_long current = 0; current < row; current++) {
        const char *newline = memchr(data + line_start, '\n', length - line_start);

        if (newline == NULL) {
            return false;
        }

        line_start = (size_t) (newline - data) + 1;
    }

    const char *newline = memchr(data + line_start, '\n', length - line_start);

    *start = line_start;
    *end = newline == NULL ? length : (size_t) (newline - data);

    return true;
}

/**
 * Length in bytes of the UTF-8 sequence that starts with the given lead byte.
 * Stray continuation and invalid bytes count as a single byte.
 */
static inline size_t php_treesitter_utf8_sequence_length(unsigned char lead)
{
    if (lead >= 0xF0) {
        return 4;
    }

    if (lead >= 0xE0) {
        return 3;
    }

    if (lead >= 0xC2 && lead < 0xE0) {
        return 2;
    }

    return 1;
}

/**
 * Walks a line until the given number of UTF-16 code units, and returns the byte offset reached
 * (at the end of the line if the line is shorter). *reached_units tells how many units it covers.
 */
static size_t php_treesitter_utf16_to_byte_column(const unsigned char *line, size_t line_length, zend_long utf16_column, zend_long *reached_units)
{
    size_t i = 0;
    zend_long units = 0;

    while (units < utf16_column && i < line_length) {
        size_t length = php_treesitter_utf8_sequence_length(line[i]);

        /* a sequence cut by the end of the line is counted as single bytes */
        if (i + length > line_length) {
            length = 1;
        }

        units += length == 4 ? 2 : 1;
        i += length;
    }

    *reached_units = units;

    return i;
}

ZEND_METHOD(TreeSitter_InputEdit, fromContentChange)
{
    zend_string *old_text, *new_text;
    zend_long start_line, start_character, end_line, end_character;

    ZEND_PARSE_PARAMETERS_START(6, 6)
        Z_PARAM_STR(old_text)
        Z_PARAM_LONG(start_line)
        Z_PARAM_LONG(start_character)
        Z_PARAM_LONG(end_line)
        Z_PARAM_LONG(end_character)
        Z_PARAM_STR(new_text)
    ZEND_PARSE_PARAMETERS_END();

    if (start_line < 0 || start_character < 0) {
        zend_argument_value_error(start_line < 0 ? 2 : 3, "must not be negative");
        RETURN_THROWS();
    }

    if (end_line < 0 || end_character < 0) {
        zend_argument_value_error(end_line < 0 ? 4 : 5, "must not be negative");
        RETURN_THROWS();
    }

    if (ZSTR_LEN(old_text) > UINT32_MAX || ZSTR_LEN(new_text) > UINT32_MAX) {
        zend_value_error("The texts must be smaller than 4 GiB");
        RETURN_THROWS();
    }

    size_t start_line_start, start_line_end, end_line_start, end_line_end;

    if (!php_treesitter_source_line(old_text, start_line, &start_line_start, &start_line_end)) {
        zend_argument_value_error(2, "must be an existing line of the old text");
        RETURN_THROWS();
    }

    if (!php_treesitter_source_line(old_text, end_line, &end_line_start, &end_line_end)) {
        zend_argument_value_error(4, "must be an existing line of the old text");
        RETURN_THROWS();
    }

    zend_long reached;

    /* LSP: a character past the end of the line means the end of the line */
    size_t start_column = php_treesitter_utf16_to_byte_column(
        (const unsigned char *) ZSTR_VAL(old_text) + start_line_start, start_line_end - start_line_start, start_character, &reached);
    size_t end_column = php_treesitter_utf16_to_byte_column(
        (const unsigned char *) ZSTR_VAL(old_text) + end_line_start, end_line_end - end_line_start, end_character, &reached);

    size_t start_byte = start_line_start + start_column;
    size_t old_end_byte = end_line_start + end_column;

    if (old_end_byte < start_byte) {
        zend_argument_value_error(4, "and argument #5 ($end_character) must not be before the start of the range");
        RETURN_THROWS();
    }

    if (start_byte + ZSTR_LEN(new_text) > UINT32_MAX) {
        zend_value_error("The edited text must be smaller than 4 GiB");
        RETURN_THROWS();
    }

    /* where the new text ends: after its last line feed, or after the start of the edit */
    uint32_t new_rows = 0;
    const char *last_newline = NULL;
    const char *cursor = ZSTR_VAL(new_text);
    const char *text_end = cursor + ZSTR_LEN(new_text);
    const char *newline;

    while (cursor < text_end && (newline = memchr(cursor, '\n', (size_t) (text_end - cursor))) != NULL) {
        new_rows++;
        last_newline = newline;
        cursor = newline + 1;
    }

    TSInputEdit edit;

    edit.start_byte = (uint32_t) start_byte;
    edit.old_end_byte = (uint32_t) old_end_byte;
    edit.new_end_byte = (uint32_t) (start_byte + ZSTR_LEN(new_text));
    edit.start_point.row = (uint32_t) start_line;
    edit.start_point.column = (uint32_t) start_column;
    edit.old_end_point.row = (uint32_t) end_line;
    edit.old_end_point.column = (uint32_t) end_column;
    edit.new_end_point.row = (uint32_t) start_line + new_rows;
    edit.new_end_point.column = last_newline == NULL
        ? (uint32_t) (start_column + ZSTR_LEN(new_text))
        : (uint32_t) (text_end - last_newline - 1);

    php_treesitter_input_edit_create(return_value, &edit);
}

ZEND_METHOD(TreeSitter_Tree, byteToUtf16Column)
{
    zend_long row, byte_column;

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(row)
        Z_PARAM_LONG(byte_column)
    ZEND_PARSE_PARAMETERS_END();

    size_t line_start, line_end;

    if (!php_treesitter_tree_ensure_current(tree)) {
        RETURN_THROWS();
    }

    if (row < 0 || tree->source == NULL || !php_treesitter_source_line(tree->source, row, &line_start, &line_end)) {
        zend_argument_value_error(1, "must be an existing row of the source");
        RETURN_THROWS();
    }

    if (byte_column < 0 || (size_t) byte_column > line_end - line_start) {
        zend_argument_value_error(2, "must be within the row");
        RETURN_THROWS();
    }

    const unsigned char *line = (const unsigned char *) ZSTR_VAL(tree->source) + line_start;
    size_t i = 0;
    zend_long units = 0;

    while (i < (size_t) byte_column) {
        size_t length = php_treesitter_utf8_sequence_length(line[i]);

        /* a sequence cut by the end of the line is counted as single bytes */
        if (i + length > line_end - line_start) {
            length = 1;
        }

        units += length == 4 ? 2 : 1;
        i += length;
    }

    RETURN_LONG(units);
}

ZEND_METHOD(TreeSitter_Tree, utf16ToByteColumn)
{
    zend_long row, utf16_column;

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(row)
        Z_PARAM_LONG(utf16_column)
    ZEND_PARSE_PARAMETERS_END();

    size_t line_start, line_end;

    if (!php_treesitter_tree_ensure_current(tree)) {
        RETURN_THROWS();
    }

    if (row < 0 || tree->source == NULL || !php_treesitter_source_line(tree->source, row, &line_start, &line_end)) {
        zend_argument_value_error(1, "must be an existing row of the source");
        RETURN_THROWS();
    }

    if (utf16_column < 0) {
        zend_argument_value_error(2, "must not be negative");
        RETURN_THROWS();
    }

    const unsigned char *line = (const unsigned char *) ZSTR_VAL(tree->source) + line_start;
    zend_long units;
    size_t i = php_treesitter_utf16_to_byte_column(line, line_end - line_start, utf16_column, &units);

    if (units < utf16_column) {
        zend_argument_value_error(2, "must be within the row");
        RETURN_THROWS();
    }

    /* a column pointing into the middle of a surrogate pair rounds up to the end of the character */
    RETURN_LONG((zend_long) i);
}

/*
 * Parser
 */

static zend_class_entry *php_treesitter_parser_ce = NULL;
static zend_object_handlers php_treesitter_parser_handlers;

typedef struct {
    TSParser *parser;
    zend_ulong timeout_micros; /* 0: no limit */
    zend_object std;
} php_treesitter_parser_object;

static inline php_treesitter_parser_object *php_treesitter_parser_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_parser_object *)((char *)obj - XtOffsetOf(php_treesitter_parser_object, std));
}

static zend_object *php_treesitter_parser_object_create(zend_class_entry *ce)
{
    php_treesitter_parser_object *obj = zend_object_alloc(sizeof(php_treesitter_parser_object), ce);

    obj->parser = ts_parser_new();

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

ZEND_METHOD(TreeSitter_Parser, __construct)
{
    zval *grammar_zv;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(grammar_zv, php_treesitter_grammar_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    const TSLanguage *grammar = php_treesitter_language_for_grammar(php_treesitter_grammar_value(Z_OBJ_P(grammar_zv)));

    if (grammar == NULL) {
        zend_throw_error(NULL, "Unknown grammar");
        RETURN_THROWS();
    }

    if (!ts_parser_set_language(parser->parser, grammar)) {
        zend_throw_error(NULL, "Grammar ABI version %u is incompatible with the tree-sitter runtime", ts_language_abi_version(grammar));
        RETURN_THROWS();
    }
}

/**
 * Finds the first ERROR or MISSING node in document order below (or at) the given node.
 * Iterative with a single cursor: deeply nested sources must not exhaust the C stack.
 */
static bool php_treesitter_find_first_error(TSNode root, TSNode *error)
{
    if (!ts_node_has_error(root)) {
        return false;
    }

    TSTreeCursor cursor = ts_tree_cursor_new(root);

    for (;;) {
        TSNode node = ts_tree_cursor_current_node(&cursor);

        /* if no deeper error is found the innermost node that has one is reported */
        *error = node;

        if (ts_node_is_error(node) || ts_node_is_missing(node)) {
            break;
        }

        if (!ts_tree_cursor_goto_first_child(&cursor)) {
            break;
        }

        bool descended = false;

        do {
            if (ts_node_has_error(ts_tree_cursor_current_node(&cursor))) {
                descended = true;
                break;
            }
        } while (ts_tree_cursor_goto_next_sibling(&cursor));

        if (!descended) {
            break;
        }
    }

    ts_tree_cursor_delete(&cursor);

    return true;
}

typedef struct {
    zend_string *source;
    zend_hrtime_t deadline; /* nanoseconds on the monotonic clock */
} php_treesitter_parse_job;

static const char *php_treesitter_parse_read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read)
{
    zend_string *source = ((php_treesitter_parse_job *) payload)->source;

    (void) position;

    if (byte_index >= ZSTR_LEN(source)) {
        *bytes_read = 0;
        return "";
    }

    *bytes_read = (uint32_t) (ZSTR_LEN(source) - byte_index);

    return ZSTR_VAL(source) + byte_index;
}

/* the parser calls this regularly; returning true cancels the parse */
static bool php_treesitter_parse_progress(TSParseState *state)
{
    return zend_hrtime() > ((php_treesitter_parse_job *) state->payload)->deadline;
}

static void php_treesitter_parser_parse(INTERNAL_FUNCTION_PARAMETERS, bool strict)
{
    zend_string *source_code;
    zval *old_tree_zv = NULL;

    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(source_code)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_OR_NULL(old_tree_zv, php_treesitter_tree_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (ZSTR_LEN(source_code) > UINT32_MAX) {
        zend_argument_value_error(1, "must be smaller than 4 GiB");
        RETURN_THROWS();
    }

    const TSTree *old_tree = NULL;

    if (old_tree_zv != NULL) {
        php_treesitter_tree_object *old = php_treesitter_tree_object_from_zend_object(Z_OBJ_P(old_tree_zv));

        if (ts_tree_language(old->tree) != ts_parser_language(parser->parser)) {
            zend_argument_value_error(2, "must have been parsed with the same grammar");
            RETURN_THROWS();
        }

        /* an unedited tree with a different source would produce a tree that does not match the source */
        if (!old->edited && !zend_string_equals(old->source, source_code)) {
            zend_argument_value_error(2, "must be edited with Tree::edit() to match the new source");
            RETURN_THROWS();
        }

        old_tree = old->tree;
    }

    TSTree *ts_tree;

    if (parser->timeout_micros == 0) {
        ts_tree = ts_parser_parse_string(parser->parser, old_tree, ZSTR_VAL(source_code), (uint32_t) ZSTR_LEN(source_code));
    } else {
        /* saturate: a huge timeout means "practically no limit", it must not wrap around into the past */
        zend_hrtime_t now = zend_hrtime();
        zend_hrtime_t room = UINT64_MAX - now;
        zend_hrtime_t delta = (zend_hrtime_t) parser->timeout_micros > room / 1000
            ? room
            : (zend_hrtime_t) parser->timeout_micros * 1000;
        php_treesitter_parse_job job = {
            .source = source_code,
            .deadline = now + delta,
        };
        TSInput input = {
            .payload = &job,
            .read = php_treesitter_parse_read,
            .encoding = TSInputEncodingUTF8,
            .decode = NULL,
        };
        TSParseOptions options = {
            .payload = &job,
            .progress_callback = php_treesitter_parse_progress,
        };

        ts_tree = ts_parser_parse_with_options(parser->parser, old_tree, input, options);
    }

    if (ts_tree == NULL) {
        if (ts_parser_language(parser->parser) == NULL) {
            zend_throw_error(NULL, "Parsing failed, the parser has no grammar set");
        } else {
            /* a cancelled parse could be resumed by the next call, which would parse the wrong source */
            ts_parser_reset(parser->parser);
            zend_throw_exception_ex(
                php_treesitter_exception_ce, 0,
                "Parsing was cancelled, it took longer than " ZEND_ULONG_FMT " microseconds", parser->timeout_micros
            );
        }

        RETURN_THROWS();
    }

    if (strict) {
        TSNode error;

        if (php_treesitter_find_first_error(ts_tree_root_node(ts_tree), &error)) {
            TSPoint point = ts_node_start_point(error);

            if (ts_node_is_missing(error)) {
                zend_throw_exception_ex(
                    php_treesitter_syntax_error_ce, 0,
                    "Syntax error: missing %s at line %u, column %u",
                    ts_node_type(error), point.row + 1, point.column + 1
                );
            } else {
                zend_throw_exception_ex(
                    php_treesitter_syntax_error_ce, 0,
                    "Syntax error at line %u, column %u",
                    point.row + 1, point.column + 1
                );
            }

            ts_tree_delete(ts_tree);
            RETURN_THROWS();
        }
    }

    php_treesitter_tree_create(return_value, ts_tree, source_code, false);
}

ZEND_METHOD(TreeSitter_Parser, parse)
{
    php_treesitter_parser_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(TreeSitter_Parser, parseWithErrors)
{
    php_treesitter_parser_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(TreeSitter_Parser, setIncludedRanges)
{
    HashTable *ranges_ht;
    zval *range_zv;

    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(ranges_ht)
    ZEND_PARSE_PARAMETERS_END();

    uint32_t count = zend_hash_num_elements(ranges_ht);

    if (count == 0) {
        /* no ranges: parse the whole source again */
        ts_parser_set_included_ranges(parser->parser, NULL, 0);
        return;
    }

    TSRange *ranges = safe_emalloc(count, sizeof(TSRange), 0);
    uint32_t index = 0;

    ZEND_HASH_FOREACH_VAL(ranges_ht, range_zv) {
        ZVAL_DEREF(range_zv);

        if (Z_TYPE_P(range_zv) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(range_zv), php_treesitter_range_ce)) {
            efree(ranges);
            zend_argument_type_error(1, "must be a list of TreeSitter\\Range objects");
            RETURN_THROWS();
        }

        if (!php_treesitter_range_read(range_zv, &ranges[index])) {
            efree(ranges);
            RETURN_THROWS();
        }

        index++;
    } ZEND_HASH_FOREACH_END();

    bool accepted = ts_parser_set_included_ranges(parser->parser, ranges, count);

    efree(ranges);

    if (!accepted) {
        zend_argument_value_error(1, "must be sorted by position and must not overlap");
        RETURN_THROWS();
    }
}

ZEND_METHOD(TreeSitter_Parser, getIncludedRanges)
{
    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    uint32_t length = 0;
    const TSRange *ranges = ts_parser_included_ranges(parser->parser, &length);

    array_init_size(return_value, length);

    for (uint32_t i = 0; i < length; i++) {
        zval range_zv;

        php_treesitter_range_create_from_range(&range_zv, ranges[i]);
        add_next_index_zval(return_value, &range_zv);
    }
}

ZEND_METHOD(TreeSitter_Parser, reset)
{
    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    ts_parser_reset(parser->parser);
}

ZEND_METHOD(TreeSitter_Parser, setTimeoutMicros)
{
    zend_long timeout;

    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    if (timeout < 0) {
        zend_argument_value_error(1, "must not be negative");
        RETURN_THROWS();
    }

    parser->timeout_micros = (zend_ulong) timeout;
}

ZEND_METHOD(TreeSitter_Parser, getTimeoutMicros)
{
    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG((zend_long) parser->timeout_micros);
}

ZEND_METHOD(TreeSitter_Parser, getLanguage)
{
    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    const TSLanguage *language = ts_parser_language(parser->parser);

    if (language == NULL) {
        zend_throw_error(NULL, "The parser has no grammar set");
        RETURN_THROWS();
    }

    php_treesitter_language_wrap(return_value, language);
}

static void php_treesitter_parser_object_free(zend_object *obj)
{
    php_treesitter_parser_object *parser = php_treesitter_parser_object_from_zend_object(obj);

    ts_parser_delete(parser->parser);

    zend_object_std_dtor(&parser->std);
}

/*
 * Query
 *
 * The tree-sitter core does not evaluate the text predicates of a query, this
 * extension does: #eq? #match? #any-of? with their not- and any- variants, and
 * collects the properties of #set!. Other predicates (#is?, #is-not?, editor
 * specific ones) are accepted and ignored. The predicates are parsed once when
 * the Query is created, and checked for every match by the QueryCursor.
 */

static zend_class_entry *php_treesitter_query_ce = NULL;
static zend_object_handlers php_treesitter_query_handlers;
static zend_class_entry *php_treesitter_query_cursor_ce = NULL;
static zend_object_handlers php_treesitter_query_cursor_handlers;
static zend_class_entry *php_treesitter_query_match_ce = NULL;
static zend_class_entry *php_treesitter_query_capture_ce = NULL;
static zend_class_entry *php_treesitter_query_exception_ce = NULL;

typedef enum {
    PHP_TREESITTER_PREDICATE_EQ,
    PHP_TREESITTER_PREDICATE_MATCH,
    PHP_TREESITTER_PREDICATE_ANY_OF
} php_treesitter_predicate_kind;

typedef struct {
    php_treesitter_predicate_kind kind;
    bool negate;                /* not-eq? not-match? not-any-of? */
    bool any;                   /* any-eq? any-match?: one node has to pass instead of all of them */
    uint32_t capture_id;
    bool rhs_is_capture;        /* eq? with a second capture */
    uint32_t rhs_capture_id;
    zend_string *string;        /* eq?: the literal, match?: the delimited regular expression */
    HashTable *strings;         /* any-of?: the literals as keys */
} php_treesitter_predicate;

typedef struct {
    php_treesitter_predicate *predicates;
    uint32_t predicate_count;
    HashTable *properties;      /* #set! key => value or null, NULL if the pattern has none */
} php_treesitter_pattern;

typedef struct {
    TSQuery *query;
    php_treesitter_pattern *patterns;
    uint32_t pattern_count;
    zend_object std;
} php_treesitter_query_object;

static inline php_treesitter_query_object *php_treesitter_query_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_query_object *)((char *)obj - XtOffsetOf(php_treesitter_query_object, std));
}

static zend_object *php_treesitter_query_object_create(zend_class_entry *ce)
{
    php_treesitter_query_object *obj = zend_object_alloc(sizeof(php_treesitter_query_object), ce);

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

static void php_treesitter_query_object_free(zend_object *obj)
{
    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(obj);

    if (query->patterns) {
        for (uint32_t i = 0; i < query->pattern_count; i++) {
            php_treesitter_pattern *pattern = &query->patterns[i];

            for (uint32_t j = 0; j < pattern->predicate_count; j++) {
                if (pattern->predicates[j].string) {
                    zend_string_release(pattern->predicates[j].string);
                }

                if (pattern->predicates[j].strings) {
                    zend_hash_destroy(pattern->predicates[j].strings);
                    FREE_HASHTABLE(pattern->predicates[j].strings);
                }
            }

            if (pattern->predicates) {
                efree(pattern->predicates);
            }

            if (pattern->properties) {
                zend_hash_destroy(pattern->properties);
                FREE_HASHTABLE(pattern->properties);
            }
        }

        efree(query->patterns);
    }

    if (query->query) {
        ts_query_delete(query->query);
    }

    zend_object_std_dtor(obj);
}

static bool php_treesitter_query_ensure_initialized(php_treesitter_query_object *query)
{
    if (query->query == NULL) {
        zend_throw_error(NULL, "The query is not initialized");
        return false;
    }

    return true;
}

/**
 * Throws a QueryException with the position of the problem in the query source.
 */
static void php_treesitter_query_throw(zend_string *source, uint32_t offset, const char *error_type, const char *description, bool name_the_token)
{
    const char *data = ZSTR_VAL(source);
    size_t length = ZSTR_LEN(source);

    if (offset > length) {
        offset = (uint32_t) length;
    }

    uint32_t line = 1, column = 1;

    for (size_t i = 0; i < offset; i++) {
        if (data[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }

    char token[65] = "";

    if (name_the_token) {
        size_t token_length = 0;

        while (offset + token_length < length && token_length < sizeof(token) - 1) {
            char c = data[offset + token_length];

            if (!isalnum((unsigned char) c) && c != '_' && c != '-' && c != '.' && c != '?' && c != '!' && c != '#' && c != '@') {
                break;
            }

            token[token_length++] = c;
        }

        token[token_length] = '\0';
    }

    zend_object *exception;

    if (token[0] != '\0') {
        exception = zend_throw_exception_ex(
            php_treesitter_query_exception_ce, 0,
            "Invalid query: %s '%s' at line %u, column %u", description, token, line, column
        );
    } else {
        exception = zend_throw_exception_ex(
            php_treesitter_query_exception_ce, 0,
            "Invalid query: %s at line %u, column %u", description, line, column
        );
    }

    zend_update_property_long(php_treesitter_query_exception_ce, exception, "offset", sizeof("offset") - 1, offset);
    zend_update_property_string(php_treesitter_query_exception_ce, exception, "errorType", sizeof("errorType") - 1, error_type);
}

static zend_function *php_treesitter_preg_match_function(void)
{
    static zend_function *function = NULL;

    if (function == NULL) {
        function = zend_hash_str_find_ptr(CG(function_table), "preg_match", sizeof("preg_match") - 1);
    }

    return function;
}

static bool php_treesitter_regex_call(zend_string *regex, const char *text, size_t length, bool *valid)
{
    zend_function *preg_match = php_treesitter_preg_match_function();
    zval params[2], retval;
    bool matched = false;

    if (preg_match == NULL) {
        *valid = false;
        return false;
    }

    ZVAL_STR_COPY(&params[0], regex);
    ZVAL_STRINGL(&params[1], text, length);
    ZVAL_UNDEF(&retval);

    zend_call_known_function(preg_match, NULL, NULL, &retval, 2, params, NULL);

    /* preg_match returns 1 or 0, false means a runtime problem (invalid pattern, invalid UTF-8 subject) */
    *valid = Z_TYPE(retval) == IS_LONG;

    if (Z_TYPE(retval) == IS_LONG) {
        matched = Z_LVAL(retval) == 1;
    }

    zval_ptr_dtor(&params[0]);
    zval_ptr_dtor(&params[1]);
    zval_ptr_dtor(&retval);

    return matched;
}

/**
 * Whether the delimited regular expression is accepted by preg_match. The
 * warning of an invalid expression is silenced, the caller reports it.
 */
static bool php_treesitter_regex_is_valid(zend_string *regex)
{
    bool valid = false;
    int old_error_reporting = EG(error_reporting);

    EG(error_reporting) = 0;
    php_treesitter_regex_call(regex, "", 0, &valid);
    EG(error_reporting) = old_error_reporting;

    return valid && !EG(exception);
}

static bool php_treesitter_str_equals(const char *a, size_t a_length, const char *literal)
{
    return a_length == strlen(literal) && memcmp(a, literal, a_length) == 0;
}

/**
 * Parses one predicate (the steps up to the Done sentinel) into the pattern. Throws a
 * QueryException for a malformed predicate of the supported kinds.
 */
static bool php_treesitter_query_add_predicate(
    php_treesitter_query_object *query, php_treesitter_pattern *pattern, uint32_t pattern_index,
    const TSQueryPredicateStep *steps, uint32_t step_count, zend_string *source)
{
    uint32_t pattern_offset = ts_query_start_byte_for_pattern(query->query, pattern_index);

    if (step_count == 0 || steps[0].type != TSQueryPredicateStepTypeString) {
        return true;
    }

    uint32_t name_length;
    const char *name = ts_query_string_value_for_id(query->query, steps[0].value_id, &name_length);
    const TSQueryPredicateStep *args = steps + 1;
    uint32_t arg_count = step_count - 1;

    if (php_treesitter_str_equals(name, name_length, "set!")) {
        if (arg_count < 1 || arg_count > 2 || args[0].type != TSQueryPredicateStepTypeString
            || (arg_count == 2 && args[1].type != TSQueryPredicateStepTypeString)) {
            php_treesitter_query_throw(source, pattern_offset, "predicate", "the #set! predicate needs a key and an optional value", false);
            return false;
        }

        uint32_t key_length, value_length;
        const char *key = ts_query_string_value_for_id(query->query, args[0].value_id, &key_length);
        zval value;

        if (arg_count == 2) {
            const char *value_data = ts_query_string_value_for_id(query->query, args[1].value_id, &value_length);
            ZVAL_STRINGL(&value, value_data, value_length);
        } else {
            ZVAL_NULL(&value);
        }

        if (pattern->properties == NULL) {
            ALLOC_HASHTABLE(pattern->properties);
            zend_hash_init(pattern->properties, 4, NULL, ZVAL_PTR_DTOR, 0);
        }

        zend_symtable_str_update(pattern->properties, key, key_length, &value);

        return true;
    }

    php_treesitter_predicate predicate;
    memset(&predicate, 0, sizeof(predicate));

    const char *base = name;
    size_t base_length = name_length;

    if (php_treesitter_str_equals(name, name_length, "any-of?") || php_treesitter_str_equals(name, name_length, "not-any-of?")) {
        predicate.kind = PHP_TREESITTER_PREDICATE_ANY_OF;
        predicate.negate = name[0] == 'n';

        if (arg_count < 2 || args[0].type != TSQueryPredicateStepTypeCapture) {
            php_treesitter_query_throw(source, pattern_offset, "predicate", "the #any-of? predicate needs a capture and at least one string", false);
            return false;
        }

        for (uint32_t i = 1; i < arg_count; i++) {
            if (args[i].type != TSQueryPredicateStepTypeString) {
                php_treesitter_query_throw(source, pattern_offset, "predicate", "the #any-of? predicate accepts strings after the capture", false);
                return false;
            }
        }

        predicate.capture_id = args[0].value_id;
        ALLOC_HASHTABLE(predicate.strings);
        zend_hash_init(predicate.strings, arg_count - 1, NULL, NULL, 0);

        for (uint32_t i = 1; i < arg_count; i++) {
            uint32_t value_length;
            const char *value = ts_query_string_value_for_id(query->query, args[i].value_id, &value_length);
            zval marker;

            ZVAL_TRUE(&marker);
            zend_hash_str_update(predicate.strings, value, value_length, &marker);
        }
    } else {
        if (base_length > 4 && memcmp(base, "any-", 4) == 0) {
            predicate.any = true;
            base += 4;
            base_length -= 4;
        }

        if (base_length > 4 && memcmp(base, "not-", 4) == 0) {
            predicate.negate = true;
            base += 4;
            base_length -= 4;
        }

        if (php_treesitter_str_equals(base, base_length, "eq?")) {
            predicate.kind = PHP_TREESITTER_PREDICATE_EQ;
        } else if (php_treesitter_str_equals(base, base_length, "match?")) {
            predicate.kind = PHP_TREESITTER_PREDICATE_MATCH;
        } else {
            /* not a predicate this extension evaluates (#is?, #is-not?, ...) */
            return true;
        }

        if (arg_count != 2 || args[0].type != TSQueryPredicateStepTypeCapture) {
            php_treesitter_query_throw(source, pattern_offset, "predicate", "the predicate needs a capture and one more argument", false);
            return false;
        }

        predicate.capture_id = args[0].value_id;

        if (predicate.kind == PHP_TREESITTER_PREDICATE_EQ && args[1].type == TSQueryPredicateStepTypeCapture) {
            predicate.rhs_is_capture = true;
            predicate.rhs_capture_id = args[1].value_id;
        } else if (args[1].type != TSQueryPredicateStepTypeString) {
            php_treesitter_query_throw(source, pattern_offset, "predicate", "the second argument of the predicate must be a string", false);
            return false;
        } else {
            uint32_t value_length;
            const char *value = ts_query_string_value_for_id(query->query, args[1].value_id, &value_length);

            if (predicate.kind == PHP_TREESITTER_PREDICATE_MATCH) {
                /* delimiter 0x01 cannot be part of a pattern, "u" makes it behave like a Unicode aware regex */
                smart_str regex = {0};

                smart_str_appendc(&regex, '\x01');
                smart_str_appendl(&regex, value, value_length);
                smart_str_appendl(&regex, "\x01u", 2);
                predicate.string = smart_str_extract(&regex);

                if (!php_treesitter_regex_is_valid(predicate.string)) {
                    zend_string_release(predicate.string);

                    if (!EG(exception)) {
                        php_treesitter_query_throw(source, pattern_offset, "predicate", "invalid regular expression in a #match? predicate", false);
                    }

                    return false;
                }
            } else {
                predicate.string = zend_string_init(value, value_length, 0);
            }
        }
    }

    pattern->predicates[pattern->predicate_count++] = predicate;

    return true;
}

static bool php_treesitter_query_compile_predicates(php_treesitter_query_object *query, zend_string *source)
{
    query->pattern_count = ts_query_pattern_count(query->query);
    query->patterns = ecalloc(query->pattern_count > 0 ? query->pattern_count : 1, sizeof(php_treesitter_pattern));

    for (uint32_t i = 0; i < query->pattern_count; i++) {
        uint32_t step_count;
        const TSQueryPredicateStep *steps = ts_query_predicates_for_pattern(query->query, i, &step_count);
        php_treesitter_pattern *pattern = &query->patterns[i];
        uint32_t predicate_total = 0;

        for (uint32_t s = 0; s < step_count; s++) {
            if (steps[s].type == TSQueryPredicateStepTypeDone) {
                predicate_total++;
            }
        }

        if (predicate_total == 0) {
            continue;
        }

        pattern->predicates = ecalloc(predicate_total, sizeof(php_treesitter_predicate));

        uint32_t start = 0;

        for (uint32_t s = 0; s < step_count; s++) {
            if (steps[s].type != TSQueryPredicateStepTypeDone) {
                continue;
            }

            if (!php_treesitter_query_add_predicate(query, pattern, i, steps + start, s - start, source)) {
                return false;
            }

            start = s + 1;
        }
    }

    return true;
}

ZEND_METHOD(TreeSitter_Query, __construct)
{
    zval *grammar_zv;
    zend_string *source;

    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(grammar_zv, php_treesitter_grammar_ce)
        Z_PARAM_STR(source)
    ZEND_PARSE_PARAMETERS_END();

    if (query->query != NULL) {
        zend_throw_error(NULL, "The query is already initialized");
        RETURN_THROWS();
    }

    const TSLanguage *language = php_treesitter_language_for_grammar(php_treesitter_grammar_value(Z_OBJ_P(grammar_zv)));

    if (language == NULL) {
        zend_throw_error(NULL, "Unknown grammar");
        RETURN_THROWS();
    }

    if (ZSTR_LEN(source) > UINT32_MAX) {
        zend_argument_value_error(2, "must be smaller than 4 GiB");
        RETURN_THROWS();
    }

    uint32_t error_offset = 0;
    TSQueryError error_type = TSQueryErrorNone;

    query->query = ts_query_new(language, ZSTR_VAL(source), (uint32_t) ZSTR_LEN(source), &error_offset, &error_type);

    if (query->query == NULL) {
        switch (error_type) {
            case TSQueryErrorNodeType:
                php_treesitter_query_throw(source, error_offset, "node_type", "unknown node type", true);
                break;
            case TSQueryErrorField:
                php_treesitter_query_throw(source, error_offset, "field", "unknown field", true);
                break;
            case TSQueryErrorCapture:
                php_treesitter_query_throw(source, error_offset, "capture", "invalid capture", true);
                break;
            case TSQueryErrorStructure:
                php_treesitter_query_throw(source, error_offset, "structure", "impossible pattern", false);
                break;
            case TSQueryErrorLanguage:
                php_treesitter_query_throw(source, error_offset, "language", "incompatible language", false);
                break;
            default:
                php_treesitter_query_throw(source, error_offset, "syntax", "syntax error", false);
                break;
        }

        RETURN_THROWS();
    }

    if (!php_treesitter_query_compile_predicates(query, source)) {
        /* a subclass may catch the exception, the query must not stay usable without its predicates */
        ts_query_delete(query->query);
        query->query = NULL;
        RETURN_THROWS();
    }
}

ZEND_METHOD(TreeSitter_Query, getCaptureNames)
{
    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    if (!php_treesitter_query_ensure_initialized(query)) {
        RETURN_THROWS();
    }

    uint32_t count = ts_query_capture_count(query->query);

    array_init_size(return_value, count);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t length;
        const char *name = ts_query_capture_name_for_id(query->query, i, &length);

        add_next_index_stringl(return_value, name, length);
    }
}

ZEND_METHOD(TreeSitter_Query, getPatternCount)
{
    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    if (!php_treesitter_query_ensure_initialized(query)) {
        RETURN_THROWS();
    }

    RETURN_LONG(ts_query_pattern_count(query->query));
}

ZEND_METHOD(TreeSitter_Query, disablePattern)
{
    zend_long pattern_index;

    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(pattern_index)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_query_ensure_initialized(query)) {
        RETURN_THROWS();
    }

    if (pattern_index < 0 || (zend_ulong) pattern_index >= ts_query_pattern_count(query->query)) {
        zend_argument_value_error(1, "must be a valid pattern index");
        RETURN_THROWS();
    }

    ts_query_disable_pattern(query->query, (uint32_t) pattern_index);
}

ZEND_METHOD(TreeSitter_Query, disableCapture)
{
    zend_string *name;

    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_query_ensure_initialized(query)) {
        RETURN_THROWS();
    }

    ts_query_disable_capture(query->query, ZSTR_VAL(name), (uint32_t) ZSTR_LEN(name));
}

/*
 * QueryCapture and QueryMatch: plain value objects, created by the QueryCursor
 */

ZEND_METHOD(TreeSitter_QueryCapture, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

ZEND_METHOD(TreeSitter_QueryMatch, __construct)
{
    zend_throw_error(NULL, "%s class is non-instantiable", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
    RETURN_THROWS();
}

static void php_treesitter_query_match_nodes(INTERNAL_FUNCTION_PARAMETERS, bool first_only)
{
    zend_string *name;
    zval rv, *captures, *capture;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    captures = zend_read_property(php_treesitter_query_match_ce, Z_OBJ_P(ZEND_THIS), "captures", sizeof("captures") - 1, 1, &rv);

    if (!first_only) {
        array_init(return_value);
    }

    if (Z_TYPE_P(captures) != IS_ARRAY) {
        if (first_only) {
            RETURN_NULL();
        }

        return;
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(captures), capture) {
        zval capture_rv, node_rv;

        if (Z_TYPE_P(capture) != IS_OBJECT) {
            continue;
        }

        zval *capture_name = zend_read_property(php_treesitter_query_capture_ce, Z_OBJ_P(capture), "name", sizeof("name") - 1, 1, &capture_rv);

        if (Z_TYPE_P(capture_name) != IS_STRING || !zend_string_equals(Z_STR_P(capture_name), name)) {
            continue;
        }

        zval *node = zend_read_property(php_treesitter_query_capture_ce, Z_OBJ_P(capture), "node", sizeof("node") - 1, 1, &node_rv);

        if (first_only) {
            RETURN_COPY(node);
        }

        Z_TRY_ADDREF_P(node);
        add_next_index_zval(return_value, node);
    } ZEND_HASH_FOREACH_END();

    if (first_only) {
        RETURN_NULL();
    }
}

ZEND_METHOD(TreeSitter_QueryMatch, getNode)
{
    php_treesitter_query_match_nodes(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(TreeSitter_QueryMatch, getNodes)
{
    php_treesitter_query_match_nodes(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

/*
 * QueryCursor
 */

typedef struct {
    TSQueryCursor *cursor;
    zval query; /* the TreeSitter\Query, it must outlive the cursor */
    zval tree;  /* the TreeSitter\Tree of the node passed to exec() */
    bool executed;
    zend_object std;
} php_treesitter_query_cursor_object;

static inline php_treesitter_query_cursor_object *php_treesitter_query_cursor_object_from_zend_object(zend_object *obj) {
    return (php_treesitter_query_cursor_object *)((char *)obj - XtOffsetOf(php_treesitter_query_cursor_object, std));
}

static zend_object *php_treesitter_query_cursor_object_create(zend_class_entry *ce)
{
    php_treesitter_query_cursor_object *obj = zend_object_alloc(sizeof(php_treesitter_query_cursor_object), ce);

    obj->cursor = ts_query_cursor_new();

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    return &obj->std;
}

static void php_treesitter_query_cursor_object_free(zend_object *obj)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(obj);

    ts_query_cursor_delete(cursor->cursor);

    zval_ptr_dtor(&cursor->query);
    zval_ptr_dtor(&cursor->tree);

    zend_object_std_dtor(obj);
}

static php_treesitter_query_object *php_treesitter_query_cursor_query(php_treesitter_query_cursor_object *cursor)
{
    if (Z_TYPE(cursor->query) != IS_OBJECT) {
        zend_throw_error(NULL, "The query cursor is not initialized");
        return NULL;
    }

    php_treesitter_query_object *query = php_treesitter_query_object_from_zend_object(Z_OBJ(cursor->query));

    return php_treesitter_query_ensure_initialized(query) ? query : NULL;
}

ZEND_METHOD(TreeSitter_QueryCursor, __construct)
{
    zval *query_zv;

    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(query_zv, php_treesitter_query_ce)
    ZEND_PARSE_PARAMETERS_END();

    /* the underlying cursor may still run against the previous query */
    if (Z_TYPE(cursor->query) != IS_UNDEF) {
        zend_throw_error(NULL, "The query cursor is already initialized");
        RETURN_THROWS();
    }

    ZVAL_COPY(&cursor->query, query_zv);
}

ZEND_METHOD(TreeSitter_QueryCursor, exec)
{
    zval *node_zv;

    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(node_zv, php_treesitter_node_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_treesitter_query_object *query = php_treesitter_query_cursor_query(cursor);

    if (query == NULL) {
        RETURN_THROWS();
    }

    php_treesitter_node_object *node = php_treesitter_node_object_from_zend_object(Z_OBJ_P(node_zv));

    /* the predicates compare the text of the nodes, which an edited tree cannot provide */
    if (!php_treesitter_tree_ensure_current(php_treesitter_tree_object_from_zend_object(Z_OBJ(node->tree)))) {
        RETURN_THROWS();
    }

    /* the cursor keeps pointers into the tree, so it has to keep the tree alive */
    zval_ptr_dtor(&cursor->tree);
    ZVAL_COPY(&cursor->tree, &node->tree);

    ts_query_cursor_exec(cursor->cursor, query->query, node->node);
    cursor->executed = true;
}

ZEND_METHOD(TreeSitter_QueryCursor, setByteRange)
{
    zend_long start_byte, end_byte;

    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(start_byte)
        Z_PARAM_LONG(end_byte)
    ZEND_PARSE_PARAMETERS_END();

    if (start_byte < 0 || start_byte > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    if (end_byte < start_byte || end_byte > UINT32_MAX) {
        zend_argument_value_error(2, "must be greater than or equal to argument #1 ($start_byte)");
        RETURN_THROWS();
    }

    if (!ts_query_cursor_set_byte_range(cursor->cursor, (uint32_t) start_byte, (uint32_t) end_byte)) {
        zend_argument_value_error(1, "and argument #2 ($end_byte) do not form a valid range");
        RETURN_THROWS();
    }
}

ZEND_METHOD(TreeSitter_QueryCursor, setPointRange)
{
    zval *start_zv, *end_zv;
    TSPoint start, end;

    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(start_zv, php_treesitter_point_ce)
        Z_PARAM_OBJECT_OF_CLASS(end_zv, php_treesitter_point_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!php_treesitter_point_read(start_zv, &start) || !php_treesitter_point_read(end_zv, &end)) {
        RETURN_THROWS();
    }

    if (php_treesitter_point_compare(end, start) < 0) {
        zend_argument_value_error(2, "must not be before argument #1 ($start)");
        RETURN_THROWS();
    }

    if (!ts_query_cursor_set_point_range(cursor->cursor, start, end)) {
        zend_argument_value_error(1, "and argument #2 ($end) do not form a valid range");
        RETURN_THROWS();
    }
}

ZEND_METHOD(TreeSitter_QueryCursor, setMatchLimit)
{
    zend_long limit;

    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(limit)
    ZEND_PARSE_PARAMETERS_END();

    if (limit < 1 || limit > UINT32_MAX) {
        zend_argument_value_error(1, "must be between 1 and %u", UINT32_MAX);
        RETURN_THROWS();
    }

    ts_query_cursor_set_match_limit(cursor->cursor, (uint32_t) limit);
}

ZEND_METHOD(TreeSitter_QueryCursor, didExceedMatchLimit)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(ts_query_cursor_did_exceed_match_limit(cursor->cursor));
}

/* the source text of a node, empty if the node lies outside of the source */
static void php_treesitter_node_source(TSNode node, zend_string *source, const char **text, size_t *length)
{
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);

    if (source == NULL || start > end || end > ZSTR_LEN(source)) {
        *text = "";
        *length = 0;
        return;
    }

    *text = ZSTR_VAL(source) + start;
    *length = end - start;
}

static bool php_treesitter_predicate_text_passes(const php_treesitter_predicate *predicate, const char *text, size_t length)
{
    bool result;

    switch (predicate->kind) {
        case PHP_TREESITTER_PREDICATE_EQ:
            result = ZSTR_LEN(predicate->string) == length && memcmp(ZSTR_VAL(predicate->string), text, length) == 0;
            break;
        case PHP_TREESITTER_PREDICATE_MATCH: {
            bool valid;

            result = php_treesitter_regex_call(predicate->string, text, length, &valid);
            break;
        }
        case PHP_TREESITTER_PREDICATE_ANY_OF:
            result = zend_hash_str_exists(predicate->strings, text, length);
            break;
        default:
            result = false;
    }

    return result != predicate->negate;
}

static bool php_treesitter_predicate_passes(
    const php_treesitter_predicate *predicate, const TSQueryCapture *captures, uint32_t capture_count, zend_string *source)
{
    if (predicate->kind == PHP_TREESITTER_PREDICATE_EQ && predicate->rhs_is_capture) {
        /* the nodes of the two captures are compared pairwise */
        uint32_t left = 0, right = 0;

        for (;;) {
            while (left < capture_count && captures[left].index != predicate->capture_id) {
                left++;
            }

            while (right < capture_count && captures[right].index != predicate->rhs_capture_id) {
                right++;
            }

            if (left >= capture_count || right >= capture_count) {
                break;
            }

            const char *left_text, *right_text;
            size_t left_length, right_length;

            php_treesitter_node_source(captures[left].node, source, &left_text, &left_length);
            php_treesitter_node_source(captures[right].node, source, &right_text, &right_length);

            bool equal = left_length == right_length && memcmp(left_text, right_text, left_length) == 0;
            bool pass = equal != predicate->negate;

            if (!pass && !predicate->any) {
                return false;
            }

            if (pass && predicate->any) {
                return true;
            }

            left++;
            right++;
        }

        return !predicate->any;
    }

    for (uint32_t i = 0; i < capture_count; i++) {
        if (captures[i].index != predicate->capture_id) {
            continue;
        }

        const char *text;
        size_t length;

        php_treesitter_node_source(captures[i].node, source, &text, &length);

        bool pass = php_treesitter_predicate_text_passes(predicate, text, length);

        if (!pass && !predicate->any) {
            return false;
        }

        if (pass && predicate->any) {
            return true;
        }
    }

    /* every node passed, or no node passed of an any- predicate */
    return !predicate->any;
}

static bool php_treesitter_match_passes(
    php_treesitter_query_object *query, const TSQueryMatch *match, zend_string *source)
{
    if (match->pattern_index >= query->pattern_count) {
        return true;
    }

    php_treesitter_pattern *pattern = &query->patterns[match->pattern_index];

    for (uint32_t i = 0; i < pattern->predicate_count; i++) {
        if (!php_treesitter_predicate_passes(&pattern->predicates[i], match->captures, match->capture_count, source)) {
            return false;
        }

        if (EG(exception)) {
            return false;
        }
    }

    return true;
}

static void php_treesitter_query_capture_create(
    zval *return_value, php_treesitter_query_object *query, TSQueryCapture capture, zval *tree)
{
    uint32_t name_length;
    const char *name = ts_query_capture_name_for_id(query->query, capture.index, &name_length);
    zval node_zv;

    object_init_ex(return_value, php_treesitter_query_capture_ce);

    php_treesitter_node_wrap(&node_zv, capture.node, tree);

    zend_update_property_stringl(php_treesitter_query_capture_ce, Z_OBJ_P(return_value), "name", sizeof("name") - 1, name, name_length);
    zend_update_property(php_treesitter_query_capture_ce, Z_OBJ_P(return_value), "node", sizeof("node") - 1, &node_zv);
    zend_update_property_long(php_treesitter_query_capture_ce, Z_OBJ_P(return_value), "index", sizeof("index") - 1, capture.index);

    zval_ptr_dtor(&node_zv);
}

static void php_treesitter_query_match_create(
    zval *return_value, php_treesitter_query_object *query, const TSQueryMatch *match, zval *tree)
{
    zval captures_zv, properties_zv;

    object_init_ex(return_value, php_treesitter_query_match_ce);

    array_init_size(&captures_zv, match->capture_count);

    for (uint16_t i = 0; i < match->capture_count; i++) {
        zval capture_zv;

        php_treesitter_query_capture_create(&capture_zv, query, match->captures[i], tree);
        add_next_index_zval(&captures_zv, &capture_zv);
    }

    if (match->pattern_index < query->pattern_count && query->patterns[match->pattern_index].properties) {
        ZVAL_ARR(&properties_zv, zend_array_dup(query->patterns[match->pattern_index].properties));
    } else {
        array_init(&properties_zv);
    }

    zend_update_property_long(php_treesitter_query_match_ce, Z_OBJ_P(return_value), "patternIndex", sizeof("patternIndex") - 1, match->pattern_index);
    zend_update_property(php_treesitter_query_match_ce, Z_OBJ_P(return_value), "captures", sizeof("captures") - 1, &captures_zv);
    zend_update_property(php_treesitter_query_match_ce, Z_OBJ_P(return_value), "properties", sizeof("properties") - 1, &properties_zv);

    zval_ptr_dtor(&captures_zv);
    zval_ptr_dtor(&properties_zv);
}

/**
 * Produces the next match whose predicates pass, or null when there are no more.
 * The result is left in return_value; a pending exception means "stop".
 */
static void php_treesitter_query_cursor_next_match(zval *return_value, php_treesitter_query_cursor_object *cursor)
{
    php_treesitter_query_object *query = php_treesitter_query_cursor_query(cursor);

    if (query == NULL) {
        return;
    }

    if (!cursor->executed) {
        zend_throw_error(NULL, "exec() has to be called before iterating the matches");
        return;
    }

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ(cursor->tree));
    TSQueryMatch match;

    while (ts_query_cursor_next_match(cursor->cursor, &match)) {
        if (!php_treesitter_match_passes(query, &match, tree->source)) {
            if (EG(exception)) {
                return;
            }

            ts_query_cursor_remove_match(cursor->cursor, match.id);
            continue;
        }

        php_treesitter_query_match_create(return_value, query, &match, &cursor->tree);
        return;
    }

    ZVAL_NULL(return_value);
}

static void php_treesitter_query_cursor_next_capture(zval *return_value, php_treesitter_query_cursor_object *cursor)
{
    php_treesitter_query_object *query = php_treesitter_query_cursor_query(cursor);

    if (query == NULL) {
        return;
    }

    if (!cursor->executed) {
        zend_throw_error(NULL, "exec() has to be called before iterating the captures");
        return;
    }

    php_treesitter_tree_object *tree = php_treesitter_tree_object_from_zend_object(Z_OBJ(cursor->tree));
    TSQueryMatch match;
    uint32_t capture_index;

    while (ts_query_cursor_next_capture(cursor->cursor, &match, &capture_index)) {
        if (!php_treesitter_match_passes(query, &match, tree->source)) {
            if (EG(exception)) {
                return;
            }

            ts_query_cursor_remove_match(cursor->cursor, match.id);
            continue;
        }

        php_treesitter_query_capture_create(return_value, query, match.captures[capture_index], &cursor->tree);
        return;
    }

    ZVAL_NULL(return_value);
}

ZEND_METHOD(TreeSitter_QueryCursor, nextMatch)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_query_cursor_next_match(return_value, cursor);
}

ZEND_METHOD(TreeSitter_QueryCursor, nextCapture)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    php_treesitter_query_cursor_next_capture(return_value, cursor);
}

ZEND_METHOD(TreeSitter_QueryCursor, getMatches)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    array_init(return_value);

    for (;;) {
        zval match_zv;

        ZVAL_UNDEF(&match_zv);
        php_treesitter_query_cursor_next_match(&match_zv, cursor);

        if (EG(exception) || Z_TYPE(match_zv) != IS_OBJECT) {
            zval_ptr_dtor(&match_zv);
            break;
        }

        add_next_index_zval(return_value, &match_zv);
    }

    if (EG(exception)) {
        /* the engine also releases the return value of a throwing call, so it must not stay a dangling array */
        zval_ptr_dtor(return_value);
        ZVAL_UNDEF(return_value);
        RETURN_THROWS();
    }
}

ZEND_METHOD(TreeSitter_QueryCursor, getCaptures)
{
    php_treesitter_query_cursor_object *cursor = php_treesitter_query_cursor_object_from_zend_object(Z_OBJ_P(ZEND_THIS));

    ZEND_PARSE_PARAMETERS_NONE();

    array_init(return_value);

    for (;;) {
        zval capture_zv;

        ZVAL_UNDEF(&capture_zv);
        php_treesitter_query_cursor_next_capture(&capture_zv, cursor);

        if (EG(exception) || Z_TYPE(capture_zv) != IS_OBJECT) {
            zval_ptr_dtor(&capture_zv);
            break;
        }

        add_next_index_zval(return_value, &capture_zv);
    }

    if (EG(exception)) {
        /* the engine also releases the return value of a throwing call, so it must not stay a dangling array */
        zval_ptr_dtor(return_value);
        ZVAL_UNDEF(return_value);
        RETURN_THROWS();
    }
}

PHP_MINIT_FUNCTION(treesitter)
{
    php_treesitter_grammar_ce = register_class_TreeSitter_Grammar();

    php_treesitter_parser_ce = register_class_TreeSitter_Parser();
    php_treesitter_parser_ce->default_object_handlers = &php_treesitter_parser_handlers;
    php_treesitter_parser_ce->create_object = php_treesitter_parser_object_create;

    memcpy(&php_treesitter_parser_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_parser_handlers.offset = XtOffsetOf(php_treesitter_parser_object, std);
    php_treesitter_parser_handlers.free_obj = php_treesitter_parser_object_free;
    php_treesitter_parser_handlers.compare = zend_objects_not_comparable;
    php_treesitter_parser_handlers.clone_obj = NULL;

    php_treesitter_tree_ce = register_class_TreeSitter_Tree();
    php_treesitter_tree_ce->default_object_handlers = &php_treesitter_tree_handlers;
    php_treesitter_tree_ce->create_object = php_treesitter_tree_object_create;

    memcpy(&php_treesitter_tree_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_tree_handlers.offset = XtOffsetOf(php_treesitter_tree_object, std);
    php_treesitter_tree_handlers.free_obj = php_treesitter_tree_object_free;
    php_treesitter_tree_handlers.compare = zend_objects_not_comparable;
    php_treesitter_tree_handlers.clone_obj = NULL;

    php_treesitter_node_ce = register_class_TreeSitter_Node();
    php_treesitter_node_ce->default_object_handlers = &php_treesitter_node_handlers;
    php_treesitter_node_ce->create_object = php_treesitter_node_object_create;

    memcpy(&php_treesitter_node_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_node_handlers.offset = XtOffsetOf(php_treesitter_node_object, std);
    php_treesitter_node_handlers.free_obj = php_treesitter_node_object_free;
    php_treesitter_node_handlers.compare = php_treesitter_node_compare;
    php_treesitter_node_handlers.clone_obj = NULL;

    php_treesitter_cursor_ce = register_class_TreeSitter_TreeCursor();
    php_treesitter_cursor_ce->default_object_handlers = &php_treesitter_cursor_handlers;
    php_treesitter_cursor_ce->create_object = php_treesitter_cursor_object_create;

    memcpy(&php_treesitter_cursor_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_cursor_handlers.offset = XtOffsetOf(php_treesitter_cursor_object, std);
    php_treesitter_cursor_handlers.free_obj = php_treesitter_cursor_object_free;
    php_treesitter_cursor_handlers.compare = zend_objects_not_comparable;
    php_treesitter_cursor_handlers.clone_obj = NULL;

    php_treesitter_exception_ce = register_class_TreeSitter_Exception(zend_ce_exception);
    php_treesitter_syntax_error_ce = register_class_TreeSitter_SyntaxError(php_treesitter_exception_ce);

    php_treesitter_range_ce = register_class_TreeSitter_Range();
    php_treesitter_input_edit_ce = register_class_TreeSitter_InputEdit();

    php_treesitter_query_exception_ce = register_class_TreeSitter_QueryException(php_treesitter_exception_ce);

    php_treesitter_query_ce = register_class_TreeSitter_Query();
    php_treesitter_query_ce->default_object_handlers = &php_treesitter_query_handlers;
    php_treesitter_query_ce->create_object = php_treesitter_query_object_create;

    memcpy(&php_treesitter_query_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_query_handlers.offset = XtOffsetOf(php_treesitter_query_object, std);
    php_treesitter_query_handlers.free_obj = php_treesitter_query_object_free;
    php_treesitter_query_handlers.compare = zend_objects_not_comparable;
    php_treesitter_query_handlers.clone_obj = NULL;

    php_treesitter_query_cursor_ce = register_class_TreeSitter_QueryCursor();
    php_treesitter_query_cursor_ce->default_object_handlers = &php_treesitter_query_cursor_handlers;
    php_treesitter_query_cursor_ce->create_object = php_treesitter_query_cursor_object_create;

    memcpy(&php_treesitter_query_cursor_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_query_cursor_handlers.offset = XtOffsetOf(php_treesitter_query_cursor_object, std);
    php_treesitter_query_cursor_handlers.free_obj = php_treesitter_query_cursor_object_free;
    php_treesitter_query_cursor_handlers.compare = zend_objects_not_comparable;
    php_treesitter_query_cursor_handlers.clone_obj = NULL;

    php_treesitter_query_match_ce = register_class_TreeSitter_QueryMatch();
    php_treesitter_query_capture_ce = register_class_TreeSitter_QueryCapture();

    php_treesitter_language_ce = register_class_TreeSitter_Language();
    php_treesitter_language_ce->default_object_handlers = &php_treesitter_language_handlers;
    php_treesitter_language_ce->create_object = php_treesitter_language_object_create;

    memcpy(&php_treesitter_language_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_language_handlers.offset = XtOffsetOf(php_treesitter_language_object, std);
    php_treesitter_language_handlers.free_obj = php_treesitter_language_object_free;
    php_treesitter_language_handlers.compare = zend_objects_not_comparable;
    php_treesitter_language_handlers.clone_obj = NULL;

    php_treesitter_point_ce = register_class_TreeSitter_Point();
    php_treesitter_point_ce->default_object_handlers = &php_treesitter_point_handlers;
    php_treesitter_point_ce->create_object = php_treesitter_point_object_create;

    memcpy(&php_treesitter_point_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_treesitter_point_handlers.free_obj = php_treesitter_point_object_free;
    php_treesitter_point_handlers.compare = zend_objects_not_comparable; // TODO: should be, though :D
    php_treesitter_point_handlers.clone_obj = NULL;

    return SUCCESS;
}

/*
 * The cases of an internal enum are constants that are evaluated on first use, which is not possible
 * in MINIT. The first request checks them once.
 */
PHP_RINIT_FUNCTION(treesitter)
{
    static bool checked = false;

#if defined(ZTS) && defined(COMPILE_DL_TREESITTER)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    if (!checked) {
        checked = true;
        php_treesitter_check_grammar_values();
    }

    return SUCCESS;
}

PHP_MINFO_FUNCTION(treesitter)
{
    char ts_abi_version[16];
    snprintf(ts_abi_version, sizeof(ts_abi_version), "%d", TREE_SITTER_LANGUAGE_VERSION);

    php_info_print_table_start();
    php_info_print_table_row(2, "Version", PHP_TREESITTER_VERSION "-unstable");
    php_info_print_table_row(2, "Tree-sitter ABI", ts_abi_version);
    php_info_print_table_row(2, "Authors", "Dávid Tamás with Claude");
    php_info_print_table_end();

    /* Versions of the bundled grammars, TS_VERSION_* come from config.m4 / config.w32 */
    php_info_print_table_start();
    php_info_print_table_header(2, "Supported language", "Parser version");
    php_info_print_table_row(2, "PHP", TS_VERSION_PHP);
    php_info_print_table_row(2, "PHP_ONLY", TS_VERSION_PHP);
    php_info_print_table_row(2, "PHPDOC", TS_VERSION_PHPDOC);
    php_info_print_table_row(2, "HTML", TS_VERSION_HTML);
    php_info_print_table_row(2, "CSS", TS_VERSION_CSS);
    php_info_print_table_row(2, "JAVASCRIPT", TS_VERSION_JAVASCRIPT);
    php_info_print_table_row(2, "TYPESCRIPT", TS_VERSION_TYPESCRIPT);
    php_info_print_table_row(2, "PYTHON", TS_VERSION_PYTHON);
    php_info_print_table_row(2, "JSON", TS_VERSION_JSON);
    php_info_print_table_end();
}

zend_module_entry treesitter_module_entry = {
    STANDARD_MODULE_HEADER,
    "treesitter",                   /* Extension name */
    NULL,                           /* zend_function_entry */
    PHP_MINIT(treesitter),          /* PHP_MINIT - Module initialization */
    NULL,                           /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(treesitter),          /* PHP_RINIT - Request initialization */
    NULL,                           /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(treesitter),          /* PHP_MINFO - Module info */
    PHP_TREESITTER_VERSION,         /* Version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TREESITTER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(treesitter)
#endif
