#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>

#include "utils.h"
#include "themes.h"
#include "border.h"
#include "logos.h"
#include "sysinfo.h"
#include "module_registry.h"
#include "config.h"

#define MAX_INFO_LINES (CONFIG_MAX_MODULES + 4)
#define MAX_REGISTRY_MODULES 32

static void print_help(const char *prog) {
    printf("\033[1;36m%s\033[0m - A retro distro fetch utility. ( fastfetch is still better lol )\n\n", prog);
    printf("Usage: %s [options]\n\n", prog);
    printf("Options:\n");
    printf("  -a, --ascii <file>      Path to custom ASCII art file\n");
    printf("  -f, --force-logo <name> Force a specific built-in distro logo\n");
    printf("                          (arch, debian, ubuntu, fedora, alpine, kali, gentoo, mint, nixos, redhat, linux, macos, default)\n");
    printf("  -t, --theme <name>      Color theme for borders and labels\n");
    printf("                          (nord, dracula, gruvbox, catppuccin, amber, default)\n");
    printf("  -n, --no-center-v       Disable full-screen vertical centering\n");
    printf("  -c, --config <file>     Path to a config.json file\n");
    printf("      --no-config         Ignore config.json entirely (use built-in defaults)\n");
    printf("  -h, --help              Display this help menu\n\n");
}

static void print_line_positioned(const char *line, int cols, const Config *cfg, int block_pad) {
    int pad;
    if (strcmp(cfg->position, "right") == 0) {
        pad = cols - cfg->margin - visual_strlen(line);
        if (pad < 0) pad = 0;
    } else if (strcmp(cfg->position, "left") == 0) {
        pad = cfg->margin;
        if (pad < 0) pad = 0;
    } else {
        pad = block_pad;
    }
    for (int j = 0; j < pad; j++) putchar(' ');
    printf("%s\n", line);
}

int main(int argc, char **argv) {
    char *cli_ascii_path  = NULL;
    char *cli_force_logo  = NULL;
    char *cli_theme_name  = NULL;
    char *cli_config_path = NULL;
    int   cli_no_center   = 0;
    int   cli_no_config   = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ascii") == 0) {
            if (i + 1 < argc) cli_ascii_path = argv[++i];
            else { fprintf(stderr, "Error: %s requires a file path.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force-logo") == 0) {
            if (i + 1 < argc) cli_force_logo = argv[++i];
            else { fprintf(stderr, "Error: %s requires a logo name.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--theme") == 0) {
            if (i + 1 < argc) cli_theme_name = argv[++i];
            else { fprintf(stderr, "Error: %s requires a theme name.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-center-v") == 0) {
            cli_no_center = 1;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) cli_config_path = argv[++i];
            else { fprintf(stderr, "Error: %s requires a file path.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "--no-config") == 0) {
            cli_no_config = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]); return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_help(argv[0]); return 1;
        }
    }

    Config cfg;
    config_load_defaults(&cfg);
    config_load(&cfg, cli_config_path, cli_no_config);

    char *custom_ascii_path = cli_ascii_path ? cli_ascii_path : (cfg.ascii_path[0] ? cfg.ascii_path : NULL);
    char *force_logo_name   = cli_force_logo ? cli_force_logo : (cfg.force_logo[0] ? cfg.force_logo : NULL);
    char *theme_name        = cli_theme_name ? cli_theme_name : (cfg.theme_name[0] ? cfg.theme_name : NULL);
    int   center_v          = cli_no_center ? 0 : cfg.center_v;

    char os_name[256];
    get_os(os_name, sizeof(os_name));

    char **ascii_lines = NULL;
    int num_ascii_lines = 0;
    int is_embedded = 0;

    if (custom_ascii_path) {
        ascii_lines = load_ascii_file(custom_ascii_path, &num_ascii_lines);
        if (!ascii_lines)
            fprintf(stderr, "Warning: Could not open '%s'. Using fallback.\n", custom_ascii_path);
    }

    if (!ascii_lines && force_logo_name) {
        is_embedded = 1;
    }
    if (!ascii_lines && !is_embedded) {
        const char *home = getenv("HOME");
        if (!home) {
            struct passwd *pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        if (home) {
            char config_path[1024];
            snprintf(config_path, sizeof(config_path), "%s/.config/fasterfetch/ascii.txt", home);
            ascii_lines = load_ascii_file(config_path, &num_ascii_lines);
        }
    }
    if (!ascii_lines && !is_embedded) {
        ascii_lines = load_ascii_file("ascii.txt", &num_ascii_lines);
    }
    if (!ascii_lines && !is_embedded) {
        char exe_dir[512], local_path[1024];
        get_exe_directory(exe_dir, sizeof(exe_dir));
        snprintf(local_path, sizeof(local_path), "%s/ascii.txt", exe_dir);
        ascii_lines = load_ascii_file(local_path, &num_ascii_lines);
    }
    if (!ascii_lines && !is_embedded) {
        ascii_lines = load_ascii_file("/usr/share/fasterfetch/ascii.txt", &num_ascii_lines);
    }
    if (!ascii_lines) is_embedded = 1;

    const char **embedded_logo       = default_ascii;
    int          embedded_logo_lines = default_ascii_lines;

    if (is_embedded) {
        const char *match_name = force_logo_name ? force_logo_name : os_name;
        char lower[256];
        set_buf(lower, sizeof(lower), match_name);
        for (int i = 0; lower[i]; i++) lower[i] = (char)tolower((unsigned char)lower[i]);

        const char **logo = pick_logo(lower, &embedded_logo_lines);
        if (!logo && force_logo_name) {
            fprintf(stderr, "Warning: Unknown forced logo '%s'. Falling back.\n", force_logo_name);
            char os_lower[256];
            set_buf(os_lower, sizeof(os_lower), os_name);
            for (int i = 0; os_lower[i]; i++) os_lower[i] = (char)tolower((unsigned char)os_lower[i]);
            logo = pick_logo(os_lower, &embedded_logo_lines);
        }
        if (logo) embedded_logo = logo;
        num_ascii_lines = embedded_logo_lines;
    }

    /* gather system info */
    char username[256], hostname[256];
    char cpu_temp[64];

    char *user_env = getenv("USER");
    if (user_env) set_buf(username, sizeof(username), user_env);
    else {
        struct passwd *pw = getpwuid(getuid());
        set_buf(username, sizeof(username), pw ? pw->pw_name : "user");
    }
    if (gethostname(hostname, sizeof(hostname)) != 0) set_buf(hostname, sizeof(hostname), "linux");

    char reg_buf[MAX_REGISTRY_MODULES][512];
    for (int i = 0; i < module_registry_count; i++)
        module_registry[i].getter(reg_buf[i], sizeof(reg_buf[i]));
    get_cpu_temp(cpu_temp, sizeof(cpu_temp));

    for (int i = 0; i < module_registry_count; i++) {
        if (strcmp(module_registry[i].key, "cpu") == 0 && cpu_temp[0]) {
            char cpu_raw[256];
            set_buf(cpu_raw, sizeof(cpu_raw), reg_buf[i]);
            snprintf(reg_buf[i], sizeof(reg_buf[i]), "%s (%s)", cpu_raw, cpu_temp);
            break;
        }
    }

    const char *reset = "\033[0m";
    const char *border_color, *label_color;
    if (theme_name && strcmp(theme_name, "default") != 0) {
        const Theme *t = find_theme(theme_name);
        if (!t) {
            fprintf(stderr, "Warning: Unknown theme '%s'. Using default.\n", theme_name);
            border_color = label_color = get_distro_color(os_name);
        } else {
            border_color = t->border; label_color = t->label;
        }
    } else border_color = label_color = get_distro_color(os_name);

    if (cfg.border_color[0]) border_color = cfg.border_color;
    if (cfg.label_color[0]) label_color = cfg.label_color;

    const BorderStyle *border = find_border_style(cfg.border_style);
    if (!border) border = find_border_style("rounded");
    int show_outer = strcmp(cfg.border_style, "none") != 0;

    /* build info rows, in config-selected order */
    char raw_info[CONFIG_MAX_MODULES][768];
    int num_raw = 0;
    for (int i = 0; i < cfg.module_count && num_raw < CONFIG_MAX_MODULES; i++) {
        const ModuleEntry *entry = find_module(cfg.module_order[i]);
        if (!entry) {
            fprintf(stderr, "Warning: unknown module '%s' in config, skipping.\n", cfg.module_order[i]);
            continue;
        }
        int idx = (int)(entry - module_registry);
        if (!entry->always_show && reg_buf[idx][0] == '\0') continue;

        snprintf(raw_info[num_raw], sizeof(raw_info[num_raw]), "%s%s%s%s%s%s %s",
                  label_color, entry->label, reset, border_color, cfg.separator, reset, reg_buf[idx]);
        num_raw++;
    }

    /* card width */
    int max_content_w = 0;
    for (int i = 0; i < num_raw; i++) {
        int w = visual_strlen(raw_info[i]);
        if (w > max_content_w) max_content_w = w;
    }
    int header_w = (int)(strlen(username) + 1 + strlen(hostname));
    if (header_w + 4 > max_content_w) max_content_w = header_w + 4;
    int card_w = max_content_w;

    char info_lines[MAX_INFO_LINES][2048];
    int li = 0;
    char *p;

    if (show_outer) {
        p = info_lines[li];
        p += sprintf(p, "%s%s", border_color, border->top_left);
        for (int i = 0; i < card_w + cfg.pad_left + cfg.pad_right; i++) p += sprintf(p, "%s", border->horizontal);
        sprintf(p, "%s%s", border->top_right, reset);
        li++;
    }

    int pad_l = (card_w - header_w) / 2;
    int pad_r = card_w - header_w - pad_l;
    p = info_lines[li];
    p += sprintf(p, "%s%s", border_color, border->vertical);
    for (int i = 0; i < cfg.pad_left; i++) *p++ = ' ';
    p += sprintf(p, "%s", reset);
    for (int i = 0; i < pad_l; i++) *p++ = ' ';
    p += sprintf(p, "\033[1m%s%s%s@\033[1m%s%s%s", label_color, username, reset, label_color, hostname, reset);
    for (int i = 0; i < pad_r + cfg.pad_right; i++) *p++ = ' ';
    sprintf(p, "%s%s%s", border_color, border->vertical, reset);
    li++;

    p = info_lines[li];
    p += sprintf(p, "%s%s", border_color, border->tee_left);
    for (int i = 0; i < card_w + cfg.pad_left + cfg.pad_right; i++) p += sprintf(p, "%s", border->horizontal);
    sprintf(p, "%s%s", border->tee_right, reset);
    li++;

    for (int i = 0; i < num_raw; i++) {
        int pad = card_w - visual_strlen(raw_info[i]);
        char row[2048]; p = row;
        int w = snprintf(p, sizeof(row), "%s%s", border_color, border->vertical); p += w;
        for (int j = 0; j < cfg.pad_left && (size_t)(p - row) < sizeof(row) - 10; j++) *p++ = ' ';
        w = snprintf(p, sizeof(row) - (p - row), "%s", reset); p += w;
        w = snprintf(p, sizeof(row) - (p - row), "%s", raw_info[i]); p += w;
        for (int j = 0; j < pad && (size_t)(p - row) < sizeof(row) - 10; j++) *p++ = ' ';
        for (int j = 0; j < cfg.pad_right && (size_t)(p - row) < sizeof(row) - 10; j++) *p++ = ' ';
        snprintf(p, sizeof(row) - (p - row), "%s%s%s", border_color, border->vertical, reset);
        strcpy(info_lines[li], row);
        li++;
    }

    if (show_outer) {
        p = info_lines[li];
        p += sprintf(p, "%s%s", border_color, border->bottom_left);
        for (int i = 0; i < card_w + cfg.pad_left + cfg.pad_right; i++) p += sprintf(p, "%s", border->horizontal);
        sprintf(p, "%s%s", border->bottom_right, reset);
        li++;
    }

    int num_info_lines = li;
    int max_ascii_w = 0;
    for (int i = 0; i < num_ascii_lines; i++) {
        const char *line = is_embedded ? embedded_logo[i] : ascii_lines[i];
        int w = visual_strlen(line);
        if (w > max_ascii_w) max_ascii_w = w;
    }
    int max_info_w = 0;
    for (int i = 0; i < num_info_lines; i++) {
        int w = visual_strlen(info_lines[i]);
        if (w > max_info_w) max_info_w = w;
    }

    struct winsize winsz;
    int cols = 80, rows = 24;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz) == 0) {
        if (winsz.ws_col > 0) cols = winsz.ws_col;
        if (winsz.ws_row > 0) rows = winsz.ws_row;
    }

    int total_lines = num_ascii_lines + 1 + num_info_lines;
    int top_padding = 0, bot_padding = 0;
    if (center_v && rows > total_lines) {
        top_padding = (rows - total_lines) / 2;
        bot_padding = rows - total_lines - top_padding - 1;
        if (bot_padding < 0) bot_padding = 0;
    }

    int pad_ascii = (cols - max_ascii_w) / 2; if (pad_ascii < 0) pad_ascii = 0;
    int pad_info  = (cols - max_info_w)  / 2; if (pad_info  < 0) pad_info  = 0;
    int pad_blocks;
    if (strcmp(cfg.position, "right") == 0) {
        pad_blocks = cols - cfg.margin - 24; if (pad_blocks < 0) pad_blocks = 0;
    } else if (strcmp(cfg.position, "left") == 0) {
        pad_blocks = cfg.margin; if (pad_blocks < 0) pad_blocks = 0;
    } else {
        pad_blocks = (cols - 24) / 2; if (pad_blocks < 0) pad_blocks = 0;
    }

    if (center_v && rows > total_lines) {
        printf("\033[H\033[2J");
        for (int i = 0; i < top_padding; i++) printf("\n");
    }

    for (int i = 0; i < num_ascii_lines; i++) {
        const char *line = is_embedded ? embedded_logo[i] : ascii_lines[i];
        print_line_positioned(line, cols, &cfg, pad_ascii);
    }
    printf("\n");

    for (int i = 0; i < num_info_lines; i++)
        print_line_positioned(info_lines[i], cols, &cfg, pad_info);

    for (int j = 0; j < pad_blocks; j++) printf(" ");
    printf("\033[40m   \033[41m   \033[42m   \033[43m   \033[44m   \033[45m   \033[46m   \033[47m   %s\n", reset);

    if (center_v && rows > total_lines) for (int i = 0; i < bot_padding; i++) printf("\n");

    free_ascii(ascii_lines, num_ascii_lines);
    return 0;
}
