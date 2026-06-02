#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

void trim_trailing(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (isspace((unsigned char)str[len-1]) || str[len-1] == '\n' || str[len-1] == '\r'))
        str[--len] = '\0';
}

int visual_strlen(const char *str) {
    int len = 0;
    for (int i = 0; str[i]; ) {
        if (str[i] == '\033' || str[i] == '\x1B') {
            i++;
            if (str[i] == '[') {
                i++;
                while (str[i] && !isalpha((unsigned char)str[i])) i++;
                if (str[i]) i++;
            }
        } else {
            if (((unsigned char)str[i] & 0xC0) != 0x80) len++;
            i++;
        }
    }
    return len;
}

void set_buf(char *buf, size_t max, const char *s) {
    snprintf(buf, max, "%s", s);
}

void build_bar(char *out, size_t out_len, double pct, int bar_width, int invert) {
    int filled = (int)((pct / 100.0) * bar_width);
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;

    const char *bar_color = invert
        ? (pct > 50.0 ? "\033[38;5;110m" : pct > 20.0 ? "\033[38;5;179m" : "\033[38;5;167m")
        : (pct < 60.0 ? "\033[38;5;110m" : pct < 80.0 ? "\033[38;5;179m" : "\033[38;5;167m");

    char *bp = out;
    int room = (int)out_len;
    int w;
    if (filled > 0) {
        w = snprintf(bp, room, "%s", bar_color); bp += w; room -= w;
        for (int i = 0; i < filled; i++) { w = snprintf(bp, room, "━"); bp += w; room -= w; }
        w = snprintf(bp, room, "\033[0m"); bp += w; room -= w;
    }
    if (filled < bar_width) {
        w = snprintf(bp, room, "\033[38;5;238m"); bp += w; room -= w;
        for (int i = filled; i < bar_width; i++) { w = snprintf(bp, room, "━"); bp += w; room -= w; }
        snprintf(bp, room, "\033[0m");
    }
}

#ifndef __APPLE__
int read_meminfo_kb(long *total_kb, long *used_kb) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0;

    long total = -1, avail = -1, free_kb = -1, buffers = 0, cached = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if      (strncmp(line, "MemTotal:",     9)  == 0) sscanf(line + 9,  "%ld", &total);
        else if (strncmp(line, "MemAvailable:", 13) == 0) sscanf(line + 13, "%ld", &avail);
        else if (strncmp(line, "MemFree:",      8)  == 0) sscanf(line + 8,  "%ld", &free_kb);
        else if (strncmp(line, "Buffers:",      8)  == 0) sscanf(line + 8,  "%ld", &buffers);
        else if (strncmp(line, "Cached:",       7)  == 0) sscanf(line + 7,  "%ld", &cached);
    }
    fclose(fp);

    if (total < 0) return 0;
    if (avail < 0) avail = (free_kb >= 0) ? free_kb + buffers + cached : total;
    long u = total - avail;
    *total_kb = total;
    *used_kb  = u < 0 ? 0 : u;
    return 1;
}
#endif

#ifdef __APPLE__
void get_exe_directory(char *dir_buf, size_t max_len) {
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        char *slash = strrchr(path, '/');
        if (slash) { *slash = '\0'; set_buf(dir_buf, max_len, path); return; }
    }
    set_buf(dir_buf, max_len, ".");
}
#else
void get_exe_directory(char *dir_buf, size_t max_len) {
    char path[512];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        char *slash = strrchr(path, '/');
        if (slash) {
            *slash = '\0';
            set_buf(dir_buf, max_len, path);
            return;
        }
    }
    set_buf(dir_buf, max_len, ".");
}
#endif

char **load_ascii_file(const char *path, int *num_lines) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    char **lines = malloc(sizeof(char *) * 256);
    if (!lines) { fclose(fp); return NULL; }
    int count = 0;
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), fp) && count < 256) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r'))
            buffer[--len] = '\0';
        lines[count] = strdup(buffer);
        if (!lines[count]) break;
        count++;
    }
    fclose(fp);
    *num_lines = count;
    return lines;
}

void free_ascii(char **lines, int num_lines) {
    if (!lines) return;
    for (int i = 0; i < num_lines; i++) free(lines[i]);
    free(lines);
}
