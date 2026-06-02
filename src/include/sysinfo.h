#ifndef SYSINFO_H
#define SYSINFO_H

#include <stddef.h>

void get_os(char *buf, size_t max_len);
void get_kernel(char *buf, size_t max_len);
void get_uptime(char *buf, size_t max_len);
void get_shell(char *buf, size_t max_len);
void get_terminal(char *buf, size_t max_len);
void get_cpu(char *buf, size_t max_len);
void get_memory_bar(char *buf, size_t max_len);
void get_battery(char *buf, size_t max_len);
void get_gpu(char *buf, size_t max_len);
void get_cpu_temp(char *buf, size_t max_len);
void get_packages(char *buf, size_t max_len);
void get_disk(char *buf, size_t max_len);
void get_local_ip(char *buf, size_t max_len);

#endif
