#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_MAX_MODULES 16

typedef struct {
    char theme_name[64];
    char border_color[32];
    char label_color[32];

    char module_order[CONFIG_MAX_MODULES][16];
    int  module_count;

    char ascii_path[1024];
    char force_logo[64];

    int  center_v;
    char separator[8];
    int  pad_left;
    int  pad_right;

    char border_style[16];
    char position[8];
    int  margin;
} Config;

/* Fills `cfg` with exactly today's hardcoded behavior (same module order,
 * same border glyphs, centered layout, etc). Always call this before
 * config_load_from_file so unspecified fields have a sane value. */
void config_load_defaults(Config *cfg);

/* Overwrites only the fields actually present in the JSON file at `path`,
 * leaving everything else in `cfg` untouched (so partial configs merge
 * instead of resetting). Returns 1 if a config was found and applied, 0 if
 * the file didn't exist / couldn't be parsed. Never crashes and never
 * leaves `cfg` partially modified+invalid -- on any failure `cfg` is left
 * exactly as it was given to this function.
 *
 * `explicit_path` should be 1 when the path came from -c/--config (so the
 * caller can decide whether to warn about a missing/bad explicit path) and
 * 0 for the implicit search-path tiers (where a missing file is normal and
 * silent). Either way, failure falls back to whatever was already in cfg. */
int config_load_from_file(Config *cfg, const char *path, int explicit_path);

/* Runs the full search order: explicit path (if non-NULL) -> ~/.config/
 * fasterfetch/config.json -> /etc/fasterfetch/config.json. Does nothing
 * if `skip` is non-zero (the --no-config flag). */
void config_load(Config *cfg, const char *explicit_path, int skip);

#endif
