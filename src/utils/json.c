#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *p;
    const char *begin;
    char *err_buf;
    size_t err_buf_len;
    int failed;
} Parser;

static char *dup_str(const char *s, size_t len) {
    char *out = malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}

static void set_error(Parser *ps, const char *msg) {
    if (ps->failed) return; /* keep the first error */
    ps->failed = 1;
    if (ps->err_buf && ps->err_buf_len > 0) {
        long offset = (long)(ps->p - ps->begin);
        snprintf(ps->err_buf, ps->err_buf_len, "%s at offset %ld", msg, offset);
    }
}

static JsonValue *new_value(JsonType type) {
    JsonValue *v = calloc(1, sizeof(JsonValue));
    if (v) v->type = type;
    return v;
}

void json_free(JsonValue *v) {
    if (!v) return;
    switch (v->type) {
        case JSON_STRING:
            free(v->as.string);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < v->as.array.count; i++)
                json_free(v->as.array.items[i]);
            free(v->as.array.items);
            break;
        case JSON_OBJECT:
            for (size_t i = 0; i < v->as.object.count; i++) {
                free(v->as.object.keys[i]);
                json_free(v->as.object.values[i]);
            }
            free(v->as.object.keys);
            free(v->as.object.values);
            break;
        default:
            break;
    }
    free(v);
}

static void skip_ws(Parser *ps) {
    while (*ps->p && isspace((unsigned char)*ps->p)) ps->p++;
}

static JsonValue *parse_value(Parser *ps);

static int parse_raw_string(Parser *ps, char **out) {
    if (*ps->p != '"') { set_error(ps, "expected string"); return 0; }
    ps->p++;

    size_t cap = 32, len = 0;
    char *buf = malloc(cap);
    if (!buf) { set_error(ps, "out of memory"); return 0; }

    while (*ps->p && *ps->p != '"') {
        unsigned char c = (unsigned char)*ps->p;
        char piece[5];
        size_t piece_len = 0;

        if (c == '\\') {
            ps->p++;
            switch (*ps->p) {
                case '"':  piece[0] = '"';  piece_len = 1; break;
                case '\\': piece[0] = '\\'; piece_len = 1; break;
                case '/':  piece[0] = '/';  piece_len = 1; break;
                case 'b':  piece[0] = '\b'; piece_len = 1; break;
                case 'f':  piece[0] = '\f'; piece_len = 1; break;
                case 'n':  piece[0] = '\n'; piece_len = 1; break;
                case 'r':  piece[0] = '\r'; piece_len = 1; break;
                case 't':  piece[0] = '\t'; piece_len = 1; break;
                case 'u': {
                    /* Minimal \uXXXX support: only handles the common
                     * single-unit BMP case (no surrogate pairs), which is
                     * enough for a config file's label/separator strings. */
                    if (!isxdigit((unsigned char)ps->p[1]) || !isxdigit((unsigned char)ps->p[2]) ||
                        !isxdigit((unsigned char)ps->p[3]) || !isxdigit((unsigned char)ps->p[4])) {
                        set_error(ps, "invalid \\u escape");
                        free(buf);
                        return 0;
                    }
                    unsigned int cp = (unsigned int)strtoul(ps->p + 1, NULL, 16);
                    ps->p += 4;
                    if (cp < 0x80) {
                        piece[0] = (char)cp; piece_len = 1;
                    } else if (cp < 0x800) {
                        piece[0] = (char)(0xC0 | (cp >> 6));
                        piece[1] = (char)(0x80 | (cp & 0x3F));
                        piece_len = 2;
                    } else {
                        piece[0] = (char)(0xE0 | (cp >> 12));
                        piece[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
                        piece[2] = (char)(0x80 | (cp & 0x3F));
                        piece_len = 3;
                    }
                    break;
                }
                default:
                    set_error(ps, "invalid escape sequence");
                    free(buf);
                    return 0;
            }
            ps->p++;
        } else if (c < 0x20) {
            set_error(ps, "control character in string");
            free(buf);
            return 0;
        } else {
            piece[0] = (char)c;
            piece_len = 1;
            ps->p++;
        }

        if (len + piece_len + 1 > cap) {
            cap *= 2;
            char *grown = realloc(buf, cap);
            if (!grown) { set_error(ps, "out of memory"); free(buf); return 0; }
            buf = grown;
        }
        memcpy(buf + len, piece, piece_len);
        len += piece_len;
    }

    if (*ps->p != '"') {
        set_error(ps, "unterminated string");
        free(buf);
        return 0;
    }
    ps->p++;
    buf[len] = '\0';
    *out = buf;
    return 1;
}

static JsonValue *parse_string(Parser *ps) {
    char *s;
    if (!parse_raw_string(ps, &s)) return NULL;
    JsonValue *v = new_value(JSON_STRING);
    if (!v) { free(s); set_error(ps, "out of memory"); return NULL; }
    v->as.string = s;
    return v;
}

static JsonValue *parse_number(Parser *ps) {
    const char *start = ps->p;
    if (*ps->p == '-') ps->p++;
    if (!isdigit((unsigned char)*ps->p)) { set_error(ps, "invalid number"); return NULL; }
    while (isdigit((unsigned char)*ps->p)) ps->p++;
    if (*ps->p == '.') {
        ps->p++;
        if (!isdigit((unsigned char)*ps->p)) { set_error(ps, "invalid number"); return NULL; }
        while (isdigit((unsigned char)*ps->p)) ps->p++;
    }
    if (*ps->p == 'e' || *ps->p == 'E') {
        ps->p++;
        if (*ps->p == '+' || *ps->p == '-') ps->p++;
        if (!isdigit((unsigned char)*ps->p)) { set_error(ps, "invalid number"); return NULL; }
        while (isdigit((unsigned char)*ps->p)) ps->p++;
    }
    JsonValue *v = new_value(JSON_NUMBER);
    if (!v) { set_error(ps, "out of memory"); return NULL; }
    v->as.number = strtod(start, NULL);
    return v;
}

static int match_literal(Parser *ps, const char *lit) {
    size_t len = strlen(lit);
    if (strncmp(ps->p, lit, len) == 0) { ps->p += len; return 1; }
    return 0;
}

static JsonValue *parse_array(Parser *ps) {
    ps->p++; /* consume '[' */
    JsonValue *v = new_value(JSON_ARRAY);
    if (!v) { set_error(ps, "out of memory"); return NULL; }

    size_t cap = 0;
    skip_ws(ps);
    if (*ps->p == ']') { ps->p++; return v; }

    while (1) {
        skip_ws(ps);
        JsonValue *item = parse_value(ps);
        if (!item) { json_free(v); return NULL; }

        if (v->as.array.count == cap) {
            cap = cap ? cap * 2 : 4;
            JsonValue **grown = realloc(v->as.array.items, cap * sizeof(JsonValue *));
            if (!grown) { set_error(ps, "out of memory"); json_free(item); json_free(v); return NULL; }
            v->as.array.items = grown;
        }
        v->as.array.items[v->as.array.count++] = item;

        skip_ws(ps);
        if (*ps->p == ',') { ps->p++; continue; }
        if (*ps->p == ']') { ps->p++; break; }
        set_error(ps, "expected ',' or ']'");
        json_free(v);
        return NULL;
    }
    return v;
}

static JsonValue *parse_object(Parser *ps) {
    ps->p++; /* consume '{' */
    JsonValue *v = new_value(JSON_OBJECT);
    if (!v) { set_error(ps, "out of memory"); return NULL; }

    size_t cap = 0;
    skip_ws(ps);
    if (*ps->p == '}') { ps->p++; return v; }

    while (1) {
        skip_ws(ps);
        char *key;
        if (!parse_raw_string(ps, &key)) { json_free(v); return NULL; }

        skip_ws(ps);
        if (*ps->p != ':') { set_error(ps, "expected ':'"); free(key); json_free(v); return NULL; }
        ps->p++;
        skip_ws(ps);

        JsonValue *val = parse_value(ps);
        if (!val) { free(key); json_free(v); return NULL; }

        if (v->as.object.count == cap) {
            cap = cap ? cap * 2 : 4;
            char **grown_keys = realloc(v->as.object.keys, cap * sizeof(char *));
            JsonValue **grown_vals = realloc(v->as.object.values, cap * sizeof(JsonValue *));
            if (!grown_keys || !grown_vals) {
                set_error(ps, "out of memory");
                free(grown_keys == NULL ? v->as.object.keys : grown_keys);
                free(key); json_free(val); json_free(v);
                return NULL;
            }
            v->as.object.keys = grown_keys;
            v->as.object.values = grown_vals;
        }
        v->as.object.keys[v->as.object.count] = key;
        v->as.object.values[v->as.object.count] = val;
        v->as.object.count++;

        skip_ws(ps);
        if (*ps->p == ',') { ps->p++; continue; }
        if (*ps->p == '}') { ps->p++; break; }
        set_error(ps, "expected ',' or '}'");
        json_free(v);
        return NULL;
    }
    return v;
}

static JsonValue *parse_value(Parser *ps) {
    skip_ws(ps);
    switch (*ps->p) {
        case '"': return parse_string(ps);
        case '{': return parse_object(ps);
        case '[': return parse_array(ps);
        case 't':
            if (match_literal(ps, "true")) {
                JsonValue *v = new_value(JSON_BOOL);
                if (v) v->as.boolean = 1;
                else set_error(ps, "out of memory");
                return v;
            }
            break;
        case 'f':
            if (match_literal(ps, "false")) {
                JsonValue *v = new_value(JSON_BOOL);
                if (v) v->as.boolean = 0;
                else set_error(ps, "out of memory");
                return v;
            }
            break;
        case 'n':
            if (match_literal(ps, "null")) {
                JsonValue *v = new_value(JSON_NULL);
                if (!v) set_error(ps, "out of memory");
                return v;
            }
            break;
        default:
            if (*ps->p == '-' || isdigit((unsigned char)*ps->p)) return parse_number(ps);
            break;
    }
    set_error(ps, "unexpected character");
    return NULL;
}

JsonValue *json_parse(const char *text, char *err_buf, size_t err_buf_len) {
    if (err_buf && err_buf_len > 0) err_buf[0] = '\0';
    if (!text) {
        if (err_buf && err_buf_len > 0) snprintf(err_buf, err_buf_len, "empty input");
        return NULL;
    }

    Parser ps = { .p = text, .begin = text, .err_buf = err_buf, .err_buf_len = err_buf_len, .failed = 0 };
    JsonValue *root = parse_value(&ps);
    if (!root) return NULL;

    skip_ws(&ps);
    if (*ps.p != '\0') {
        set_error(&ps, "trailing data after JSON value");
        json_free(root);
        return NULL;
    }
    return root;
}

const JsonValue *json_object_get(const JsonValue *obj, const char *key) {
    if (!obj || obj->type != JSON_OBJECT || !key) return NULL;
    for (size_t i = 0; i < obj->as.object.count; i++)
        if (strcmp(obj->as.object.keys[i], key) == 0) return obj->as.object.values[i];
    return NULL;
}

const char *json_get_string(const JsonValue *obj, const char *key, const char *def) {
    const JsonValue *v = json_object_get(obj, key);
    if (v && v->type == JSON_STRING) return v->as.string;
    return def;
}

int json_get_bool(const JsonValue *obj, const char *key, int def) {
    const JsonValue *v = json_object_get(obj, key);
    if (v && v->type == JSON_BOOL) return v->as.boolean;
    return def;
}

int json_get_int(const JsonValue *obj, const char *key, int def) {
    const JsonValue *v = json_object_get(obj, key);
    if (v && v->type == JSON_NUMBER) return (int)v->as.number;
    return def;
}

char **json_get_string_array(const JsonValue *obj, const char *key, int *out_count) {
    const JsonValue *v = json_object_get(obj, key);
    if (!v || v->type != JSON_ARRAY) { if (out_count) *out_count = 0; return NULL; }

    char **out = malloc(v->as.array.count * sizeof(char *));
    if (!out) { if (out_count) *out_count = 0; return NULL; }

    int n = 0;
    for (size_t i = 0; i < v->as.array.count; i++) {
        JsonValue *item = v->as.array.items[i];
        if (item->type != JSON_STRING) continue; /* skip non-string entries, don't fail the whole array */
        size_t len = strlen(item->as.string);
        char *copy = dup_str(item->as.string, len);
        if (!copy) continue;
        out[n++] = copy;
    }

    if (n == 0) { free(out); if (out_count) *out_count = 0; return NULL; }
    if (out_count) *out_count = n;
    return out;
}

void json_free_string_array(char **arr, int count) {
    if (!arr) return;
    for (int i = 0; i < count; i++) free(arr[i]);
    free(arr);
}
