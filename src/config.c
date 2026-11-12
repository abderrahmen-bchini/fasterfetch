#include "config.h"
#include "json.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

void config_load_defaults(Config *cfg) {
    memset(cfg, 0, sizeof(*cfg));

    static const char *default_modules[] = {
        "os", "kernel", "uptime", "shell", "term", "cpu",
        "gpu", "pkgs", "memory", "disk", "battery", "ip"
    };
    cfg->module_count = (int)(sizeof(default_modules) / sizeof(default_modules[0]));
    for (int i = 0; i < cfg->module_count; i++)
        set_buf(cfg->module_order[i], sizeof(cfg->module_order[i]), default_modules[i]);

    cfg->center_v = 1;
    set_buf(cfg->separator, sizeof(cfg->separator), "│"); /* "│" */
    cfg->pad_left = 1;
    cfg->pad_right = 1;

    set_buf(cfg->border_style, sizeof(cfg->border_style), "rounded");
    set_buf(cfg->position, sizeof(cfg->position), "center");
    cfg->margin = 0;
}

/* json_get_string's `def` argument is often the field being written (so a
 * missing key keeps the current value) -- snapshot it into a local buffer
 * first, since set_buf(field, ..., field) would alias snprintf's source and
 * destination, which is undefined behavior. */
static void merge_string_field(char *field, size_t field_size, const JsonValue *obj, const char *key) {
    char current[1024];
    set_buf(current, sizeof(current), field);
    set_buf(field, field_size, json_get_string(obj, key, current));
}

static char *read_whole_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }

    char *buf = malloc((size_t)size + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t read_n = fread(buf, 1, (size_t)size, f);
    fclose(f);
    buf[read_n] = '\0';
    return buf;
}

int config_load_from_file(Config *cfg, const char *path, int explicit_path) {
    char *text = read_whole_file(path);
    if (!text) {
        if (explicit_path)
            fprintf(stderr, "Warning: could not open config file '%s'. Using defaults.\n", path);
        return 0;
    }

    char err[256];
    JsonValue *root = json_parse(text, err, sizeof(err));
    free(text);

    if (!root) {
        fprintf(stderr, "Warning: config file '%s' is invalid (%s). Using defaults.\n", path, err);
        return 0;
    }
    if (root->type != JSON_OBJECT) {
        fprintf(stderr, "Warning: config file '%s' must be a JSON object. Using defaults.\n", path);
        json_free(root);
        return 0;
    }

    merge_string_field(cfg->theme_name, sizeof(cfg->theme_name), root, "theme");

    const JsonValue *colors = json_object_get(root, "colors");
    merge_string_field(cfg->border_color, sizeof(cfg->border_color), colors, "border");
    merge_string_field(cfg->label_color, sizeof(cfg->label_color), colors, "label");

    int mod_count = 0;
    char **mods = json_get_string_array(root, "modules", &mod_count);
    if (mods) {
        int n = mod_count < CONFIG_MAX_MODULES ? mod_count : CONFIG_MAX_MODULES;
        for (int i = 0; i < n; i++)
            set_buf(cfg->module_order[i], sizeof(cfg->module_order[i]), mods[i]);
        cfg->module_count = n;
        json_free_string_array(mods, mod_count);
    }

    const JsonValue *ascii = json_object_get(root, "ascii");
    merge_string_field(cfg->ascii_path, sizeof(cfg->ascii_path), ascii, "path");
    merge_string_field(cfg->force_logo, sizeof(cfg->force_logo), ascii, "force_logo");

    const JsonValue *layout = json_object_get(root, "layout");
    cfg->center_v = json_get_bool(layout, "center_vertical", cfg->center_v);
    merge_string_field(cfg->separator, sizeof(cfg->separator), layout, "separator");
    cfg->pad_left = json_get_int(layout, "pad_left", cfg->pad_left);
    cfg->pad_right = json_get_int(layout, "pad_right", cfg->pad_right);
    merge_string_field(cfg->border_style, sizeof(cfg->border_style), layout, "border_style");
    merge_string_field(cfg->position, sizeof(cfg->position), layout, "position");
    cfg->margin = json_get_int(layout, "margin", cfg->margin);

    json_free(root);
    return 1;
}

void config_load(Config *cfg, const char *explicit_path, int skip) {
    if (skip) return;

    if (explicit_path) {
        config_load_from_file(cfg, explicit_path, 1);
        return;
    }

    const char *home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }
    if (home) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/.config/fasterfetch/config.json", home);
        if (config_load_from_file(cfg, path, 0)) return;
    }

    config_load_from_file(cfg, "/etc/fasterfetch/config.json", 0);
}
