/** @file       ctomat.c
 *  @brief      C library for encoding, decoding TOMl dATa.
 *
 *  @author     t-kenji <protect.2501@gmail.com>
 *  @date       2018-11-14 create new.
 *  @copyright  Copyright © 2018 t-kenji
 *
 *  This code is licensed under the MIT License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "collections.h"
#include "utils.h"
#include "ctomat.h"

enum toml_value_type {
    VAL_TYPE_NONE,
    VAL_TYPE_OBJECT,
    VAL_TYPE_BASIC_STRING,
    VAL_TYPE_LITERAL_STRING,
    VAL_TYPE_INTEGER,
    VAL_TYPE_FLOAT,
    VAL_TYPE_BOOLEAN,
};

struct toml_value {
    enum toml_value_type type;
    union {
        char string_value[256];
        int64_t integer_value;
    };
};

#define TOML_VALUE_INITIALIZER \
    (struct toml_value){       \
        .type = VAL_TYPE_NONE, \
        .string_value = {0},   \
    }

struct toml_key {
    char name[256];
    struct toml_value value;
};

#define TOML_KEY_INITIALIZER             \
    (struct toml_key){                   \
        .name = {0},                     \
        .value = TOML_VALUE_INITIALIZER, \
    }

struct toml {
    NTREE global;
    struct toml *root;
    struct toml_key key;
    _Atomic size_t ref_count;
};

#define TOML_INITIALIZER             \
    (struct toml){                   \
        .global = NULL,              \
        .root = NULL,                \
        .key = TOML_KEY_INITIALIZER, \
        .ref_count = 1,              \
    }

#define min(a, b) (((a) < (b)) ? (a) : (b))

static void strip(char *buf)
{
    char *head = buf;
    char *tail;
    char *cur;

    while ((*head == ' ') || (*head == '\t')) {
        ++head;
    }
    cur = tail = head;
    while (*cur != '\0') {
        if ((*cur != ' ') && (*cur != '\t') && (*cur != '\r') && (*cur != '\n')) {
            tail = cur + 1;
        }
        ++cur;
    }
    for (cur = head; cur < tail; ++cur, ++buf) {
        *buf = *cur;
    }
    *buf = '\0';
}

static void dequote(char *buf)
{
    char *head = buf;
    char *tail = buf + strlen(buf);
    char *cur;

    size_t quotes = 0;
    while ((*head == '"') || (*head == '\'')) {
        ++head;
        ++quotes;
    }
    if (*head == '\0') {
        if (quotes == 2) {
            head = buf + 1;
            tail -= 1;
        } else if (quotes == 6) {
            head = buf + 3;
            tail -= 3;
        } else {
            return;
        }
    } else {
        if ((quotes != 1) && (quotes != 3)) {
            return;
        }
        tail -= quotes;
    }

    for (cur = head; cur < tail; ++cur, ++buf) {
        *buf = *cur;
    }
    *buf = '\0';
}

static void remove_comment(char *buf)
{
    char *comment = strchr(buf, '#');
    if (comment != NULL) {
        *comment = '\0';
    }
}

static char *get_expr(char *buf, char *expr)
{
    char *cur = buf;

    while ((*cur != '\n') && (*cur != '\0')) {
        *expr++ = *cur++;
    }
    if (*cur != '\0') {
        ++cur;
    }
    *expr = '\0';

    return cur;
}

static int parse_sweeped_expr(char *buf, char *lv, size_t lv_len, char *rv, size_t rv_len)
{
    char *operator = strchr(buf, '=');
    size_t expr_len = strlen(buf);
    size_t count;

    if (operator == NULL) {
        errno = EINVAL;
        return -1;
    }

    count = min(operator - buf, lv_len - 1);
    strncpy(lv, buf, count);
    lv[count] = '\0';
    count = min(expr_len - (operator - buf) - 1, rv_len - 1);
    strncpy(rv, operator + 1, count);
    rv[count] = '\0';

    return 0;
}

static struct toml *toml_alloc(void)
{
    NTREE global = ntree_init(sizeof(struct toml), 10);
    if (global == NULL) {
        return NULL;
    }
    struct toml *root = (struct toml *)ntree_insert(global, NULL, &TOML_INITIALIZER);

    root->global = global;
    root->root   = root;

    return root;
}

static void toml_free(struct toml *obj)
{
    for (ITER iter = ntree_iter_all(obj->global); iter != NULL; iter = iter_next(iter)) {
        struct toml_key *key = &((struct toml *)iter_data(iter))->key;
        int age = ntree_iter_age(iter);
        switch (key->value.type) {
        case VAL_TYPE_OBJECT:
            DEBUG("[%d]%s: is object", age, key->name);
            break;
        case VAL_TYPE_BASIC_STRING:
        case VAL_TYPE_LITERAL_STRING:
            DEBUG("[%d]%s: '%s'", age, key->name, key->value.string_value);
            break;
        default:
            ERROR("[%d]%s: unknown value", age, key->name);
            break;
        }
    }
    ntree_release(obj->global);
}

toml_t toml_create(void)
{
    return (toml_t)toml_alloc();
}

int toml_delete(toml_t object, bool forced)
{
    if ((object->ref_count > 0) && (!forced)) {
        return -1;
    }

    toml_free((struct toml *)object);

    return 0;
}

toml_t toml_object_get(toml_t object, const char *key)
{
    for (ITER iter = ntree_iter_children(object->global); iter != NULL; iter = iter_next(iter)) {
        struct toml_key *iter_key = &((struct toml *)iter_data(iter))->key;
        DEBUG("key: %s <> %s", iter_key->name, key);
    }

    return NULL;
}

const char *toml_string_value(toml_t string)
{
    return NULL;
}

int toml_string_set(toml_t string, const char *value)
{
    return -1;
}

toml_t toml_load(const char *pathname)
{
    toml_t obj = toml_alloc();
    printf("%s:%d:%s Hello,World ref: %zu\n", __FILE__, __LINE__, __func__, obj->ref_count);

    return obj;
}

static bool is_bare_key(char *key)
{
    if (key[0] == '\0') {
        return false;
    }

    for (int i = 1; key[i] != '\0'; ++i) {
        char c = key[i];
        if (((c < 'A') || ('Z' < c))
            && ((c < 'a') || ('z' < c))
            && ((c < '0') || ('9' < c))
            && (c != '_') && (c != '-')) {

            return false;
        }
    }

    return true;
}

/**
 *  @warning    not support yet.
 */
static bool is_quoted_key(char *key)
{
    return false;
}

static bool is_dotted_key(char *key)
{
    char temp[256];
    char *head = key;
    char *tail = strchr(head, '.');

    if (tail == NULL) {
        return false;
    }
    do {
        strncpy(temp, head, tail - head);
        temp[tail - head] = '\0';
        if (!is_bare_key(temp)) {
            return false;
        }
        head = tail + 1;
        tail = strchr(head, '.');
    } while (tail != NULL);

    strncpy(temp, head, sizeof(temp));
    if (!is_bare_key(temp)) {
        return false;
    }

    return true;
}

static struct toml *parse_key(struct toml *obj, char *lval)
{
    if (is_bare_key(lval) || is_quoted_key(lval)) {
        DEBUG("%s: valid bare-key or quoted-key", lval);
        struct toml object = TOML_INITIALIZER;
        strncpy(object.key.name, lval, sizeof(object.key.name) - 1);
        return (struct toml *)ntree_insert(obj->global, obj->root, &object);
    } else if (is_dotted_key(lval)) {
        DEBUG("%s: valid dotted-key", lval);
        struct toml object;
        struct toml *parent = obj->root;
        char temp[256];
        char *head = lval;
        char *tail = strchr(head, '.');
        do {
            strncpy(temp, head, tail - head);
            temp[tail - head] = '\0';
            object = TOML_INITIALIZER;
            strncpy(object.key.name, temp, sizeof(object.key.name) - 1);
            object.key.value.type = VAL_TYPE_OBJECT;
            parent = ntree_insert(obj->global, parent, &object);
            head = tail + 1;
            tail = strchr(head, '.');
        } while (tail != NULL);

        object = TOML_INITIALIZER;
        strncpy(object.key.name, head, sizeof(object.key.name) - 1);
        return (struct toml *)ntree_insert(obj->global, parent, &object);
    } else {
        DEBUG("%s: invalid key", lval);
        return NULL;
    }
}

static bool is_basic_string_value(char *value)
{
    return string_match("\"+\"", value);
}

__attribute__((unused))
static bool is_multiline_basic_string_value(char *value)
{
    return string_match("\"\"\"+\"\"\"", value);
}

static bool is_literal_string_value(char *value)
{
    return string_match("'+'", value);
}

__attribute__((unused))
static bool is_multiline_literal_string_value(char *value)
{
    return string_match("'''+'''", value);
}

__attribute__((unused))
static bool is_string_value(char *value)
{
    return is_basic_string_value(value)
           || is_multiline_basic_string_value(value)
           || is_literal_string_value(value)
           || is_multiline_literal_string_value(value);
}

static int parse_value(struct toml_key *key, char *rval)
{
    if (is_basic_string_value(rval)) {
        key->value.type = VAL_TYPE_BASIC_STRING;
        dequote(rval);
        strncpy(key->value.string_value, rval, sizeof(key->value.string_value) - 1);
        return 0;
    } else if (is_literal_string_value(rval)) {
        key->value.type = VAL_TYPE_LITERAL_STRING;
        dequote(rval);
        strncpy(key->value.string_value, rval, sizeof(key->value.string_value) - 1);
        return 0;
    } else {
        ERROR("%s: unknown or unsuppoted value", rval);
        return -1;
    }
}

static int parse_expr(struct toml *obj, char *expr)
{
    char lval[256];
    char rval[256];
    int ret;

    remove_comment(expr);
    strip(expr);
    if (expr[0] != '\0') {
        ret = parse_sweeped_expr(expr, lval, sizeof(lval), rval, sizeof(rval));
        if (ret != 0) {
            ERROR("error: '%s'", expr);
            return -1;
        }

        strip(lval);
        strip(rval);
        DEBUG("lval: '%s', rval: '%s'", lval, rval);
        struct toml *object = parse_key(obj, lval);
        if (object == NULL) {
            ERROR("%s: invalid key", lval);
            return -1;
        }
        ret = parse_value(&object->key, rval);
        if (ret != 0) {
            ntree_remove(obj->global, object);
            ERROR("error: '%s'", rval);
            return -1;
        }
    }

    return 0;
}

toml_t toml_load_from_memory(const char *buf, size_t length)
{
    toml_t obj = toml_alloc();
    char expr[256];

    for (char *cur = get_expr((char *)buf, expr);
         *cur != '\0';
         cur = get_expr(cur, expr)) {

        parse_expr(obj, expr);
    }
    parse_expr(obj, expr);

    return obj;
}

int toml_save(toml_t object, const char *pathname)
{
    return 0;
}

int toml_save_to_memory(toml_t object, char *buf, size_t length)
{
    size_t remain = length;
    char line[256];
    buf[0] = '\0';

    for (ITER iter = ntree_iter_all(object->global); iter != NULL; iter = iter_next(iter)) {
        struct toml_key *key = &((struct toml *)iter_data(iter))->key;
        int age = ntree_iter_age(iter);
        switch (key->value.type) {
        case VAL_TYPE_OBJECT:
            ERROR("%s: object is not supported", key->name);
            break;
        case VAL_TYPE_BASIC_STRING:
            DEBUG("[%d]%s: '%s'", age, key->name, key->value.string_value);
            strncpy(line, key->name, sizeof(line));
            strncat(line, " = \"", sizeof(line) - strlen(line));
            strncat(line, key->value.string_value, sizeof(line) - strlen(line));
            strncat(line, "\"\n", sizeof(line) - strlen(line));
            if (remain < strlen(line)) {
                ERROR("%s: out of memory", key->name);
                break;
            }
            strncat(buf, line, length - strlen(buf));
            break;
        case VAL_TYPE_LITERAL_STRING:
            DEBUG("[%d]%s: '%s'", age, key->name, key->value.string_value);
            strncpy(line, key->name, sizeof(line));
            strncat(line, " = '", sizeof(line) - strlen(line));
            strncat(line, key->value.string_value, sizeof(line) - strlen(line));
            strncat(line, "'\n", sizeof(line) - strlen(line));
            if (remain < strlen(line)) {
                ERROR("%s: out of memory", key->name);
                break;
            }
            strncat(buf, line, length - strlen(buf));
            break;
        default:
            ERROR("[%d]%s: unknown value", age, key->name);
            break;
        }
    }

    return 0;
}
