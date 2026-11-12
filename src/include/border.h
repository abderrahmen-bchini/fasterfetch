#ifndef BORDER_H
#define BORDER_H

typedef struct {
    const char *name;
    const char *top_left, *top_right, *bottom_left, *bottom_right;
    const char *horizontal, *vertical, *tee_left, *tee_right;
} BorderStyle;

/* Returns NULL if `name` doesn't match a known style; caller should fall
 * back to the "rounded" style (today's default glyphs) in that case. */
const BorderStyle *find_border_style(const char *name);

#endif
