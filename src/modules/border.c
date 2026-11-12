#include "border.h"
#include <string.h>

static const BorderStyle border_styles[] = {
    { "rounded", "╭", "╮", "╰", "╯", "─", "│", "├", "┤" },
    { "square",  "┌", "┐", "└", "┘", "─", "│", "├", "┤" },
    { "ascii",   "+", "+", "+", "+", "-", "|", "+", "+" },
    { "none",    "",  "",  "",  "",  "",  "",  "",  ""  },
};
static const int num_border_styles = (int)(sizeof(border_styles) / sizeof(border_styles[0]));

const BorderStyle *find_border_style(const char *name) {
    if (!name) return NULL;
    for (int i = 0; i < num_border_styles; i++)
        if (strcmp(border_styles[i].name, name) == 0) return &border_styles[i];
    return NULL;
}
