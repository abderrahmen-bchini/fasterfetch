#include "themes.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>

static const Theme themes[] = {
    { "nord",       "\033[38;5;110m", "\033[38;5;153m" },
    { "dracula",    "\033[38;5;141m", "\033[38;5;212m" },
    { "gruvbox",    "\033[38;5;172m", "\033[38;5;214m" },
    { "catppuccin", "\033[38;5;147m", "\033[38;5;183m" },
    { "amber",      "\033[38;5;136m", "\033[38;5;220m" },
};
static const int num_themes = 5;

const Theme *find_theme(const char *name) {
    for (int i = 0; i < num_themes; i++)
        if (strcmp(themes[i].name, name) == 0) return &themes[i];
    return NULL;
}

const char *get_distro_color(const char *distro) {
    char lower[256];
    set_buf(lower, sizeof(lower), distro);
    for (int i = 0; lower[i]; i++) lower[i] = (char)tolower((unsigned char)lower[i]);

    if (strstr(lower, "ubuntu"))  return "\033[38;5;202m";
    if (strstr(lower, "debian"))  return "\033[38;5;161m";
    if (strstr(lower, "arch"))    return "\033[38;5;39m";
    if (strstr(lower, "fedora"))  return "\033[38;5;27m";
    if (strstr(lower, "mint"))    return "\033[38;5;10m";
    if (strstr(lower, "gentoo"))  return "\033[38;5;99m";
    if (strstr(lower, "alpine"))  return "\033[38;5;32m";
    if (strstr(lower, "kali"))    return "\033[38;5;32m";
    if (strstr(lower, "nixos"))   return "\033[38;5;110m";
    if (strstr(lower, "red hat") || strstr(lower, "rhel") ||
        strstr(lower, "centos")  || strstr(lower, "redhat")) return "\033[38;5;196m";
    if (strstr(lower, "macos")   || strstr(lower, "darwin"))  return "\033[38;5;33m";
    return "\033[1;36m";
}
