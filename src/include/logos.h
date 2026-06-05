#ifndef LOGOS_H
#define LOGOS_H

extern const char *default_ascii[];
extern const int   default_ascii_lines;

const char **pick_logo(const char *lower, int *lines_out);

#endif
