#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void trim_trailing(char *str);
int  visual_strlen(const char *str);
void set_buf(char *buf, size_t max, const char *s);
void build_bar(char *out, size_t out_len, double pct, int bar_width, int invert);
void get_exe_directory(char *dir_buf, size_t max_len);
char **load_ascii_file(const char *path, int *num_lines);
void free_ascii(char **lines, int num_lines);

#ifndef __APPLE__
int  read_meminfo_kb(long *total_kb, long *used_kb);
#endif

#endif
