#ifndef MODULE_REGISTRY_H
#define MODULE_REGISTRY_H

#include <stddef.h>

typedef void (*GetterFn)(char *buf, size_t max_len);

typedef struct {
    const char *key;         /* config-facing id, e.g. "os" */
    const char *label;       /* display label, e.g. "OS" */
    GetterFn    getter;
    int         always_show; /* 1 = render even if getter produced "" */
} ModuleEntry;

extern const ModuleEntry module_registry[];
extern const int         module_registry_count;

const ModuleEntry *find_module(const char *key);

#endif
