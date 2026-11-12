#include "module_registry.h"
#include "sysinfo.h"
#include <string.h>

const ModuleEntry module_registry[] = {
    { "os",      "OS     ", get_os,         1 },
    { "kernel",  "Kernel ", get_kernel,     1 },
    { "uptime",  "Uptime ", get_uptime,     1 },
    { "shell",   "Shell  ", get_shell,      1 },
    { "term",    "Term   ", get_terminal,   1 },
    { "cpu",     "CPU    ", get_cpu,        1 },
    { "gpu",     "GPU    ", get_gpu,        0 },
    { "pkgs",    "Pkgs   ", get_packages,   0 },
    { "memory",  "Memory ", get_memory_bar, 1 },
    { "disk",    "Disk   ", get_disk,       0 },
    { "battery", "Battery", get_battery,    0 },
    { "ip",      "IP     ", get_local_ip,   0 },
};
const int module_registry_count = (int)(sizeof(module_registry) / sizeof(module_registry[0]));

const ModuleEntry *find_module(const char *key) {
    for (int i = 0; i < module_registry_count; i++)
        if (strcmp(module_registry[i].key, key) == 0) return &module_registry[i];
    return NULL;
}
