#ifndef JSON_H
#define JSON_H

#include <stddef.h>

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

typedef struct JsonValue JsonValue;

struct JsonValue {
    JsonType type;
    union {
        int boolean;
        double number;
        char *string;
        struct {
            JsonValue **items;
            size_t count;
        } array;
        struct {
            char **keys;
            JsonValue **values;
            size_t count;
        } object;
    } as;
};

/* Parses `text` into a tree. On malformed input returns NULL and writes a
 * human-readable message into err_buf (if non-NULL) -- never returns a
 * partial tree, never aborts. */
JsonValue *json_parse(const char *text, char *err_buf, size_t err_buf_len);
void       json_free(JsonValue *v);

const JsonValue *json_object_get(const JsonValue *obj, const char *key);

/* Every accessor below treats "obj is NULL", "key missing", and "wrong type"
 * identically: falls back to `def`. Callers never need to special-case any
 * of those three conditions. */
const char *json_get_string(const JsonValue *obj, const char *key, const char *def);
int         json_get_bool(const JsonValue *obj, const char *key, int def);
int         json_get_int(const JsonValue *obj, const char *key, int def);

/* Returns a malloc'd array of malloc'd C strings, or NULL with *out_count=0
 * if the key is missing or not a string array. Free with json_free_string_array. */
char **json_get_string_array(const JsonValue *obj, const char *key, int *out_count);
void   json_free_string_array(char **arr, int count);

#endif
