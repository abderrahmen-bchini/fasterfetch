#ifndef THEMES_H
#define THEMES_H

typedef struct {
    const char *name;
    const char *border;
    const char *label;
} Theme;

const Theme *find_theme(const char *name);
const char  *get_distro_color(const char *distro);

#endif
