#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>

#include "utils.h"
#include "themes.h"
#include "logos.h"
#include "sysinfo.h"

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
    printf("  -h, --help              Display this help menu\n\n");
}

int main(int argc, char **argv) {
    char *custom_ascii_path = NULL;
    char *force_logo_name   = NULL;
    char *theme_name        = NULL;
    int   center_v          = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ascii") == 0) {
            if (i + 1 < argc) custom_ascii_path = argv[++i];
            else { fprintf(stderr, "Error: %s requires a file path.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force-logo") == 0) {
            if (i + 1 < argc) force_logo_name = argv[++i];
            else { fprintf(stderr, "Error: %s requires a logo name.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--theme") == 0) {
            if (i + 1 < argc) theme_name = argv[++i];
            else { fprintf(stderr, "Error: %s requires a theme name.\n", argv[i]); return 1; }
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-center-v") == 0) {
            center_v = 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]); return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_help(argv[0]); return 1;
        }
    }

    char os_name[256];
    get_os(os_name, sizeof(os_name));

    /* Load ASCII art */
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

    /* Select embedded logo */
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

    /* Gather system info */
    char username[256], hostname[256];
    char kernel[256], uptime[256], shell[256], terminal[256], cpu[256];
    char memory_bar[256], battery[256], gpu[256], packages[256], disk[256];
    char cpu_temp[64], local_ip[64];

    char *user_env = getenv("USER");
    if (user_env) set_buf(username, sizeof(username), user_env);
    else {
        struct passwd *pw = getpwuid(getuid());
        set_buf(username, sizeof(username), pw ? pw->pw_name : "user");
    }
    if (gethostname(hostname, sizeof(hostname)) != 0) set_buf(hostname, sizeof(hostname), "linux");

    get_kernel(kernel, sizeof(kernel));
    get_uptime(uptime, sizeof(uptime));
    get_shell(shell, sizeof(shell));
    get_terminal(terminal, sizeof(terminal));
    get_cpu(cpu, sizeof(cpu));
    get_memory_bar(memory_bar, sizeof(memory_bar));
    get_battery(battery, sizeof(battery));
    get_gpu(gpu, sizeof(gpu));
    get_cpu_temp(cpu_temp, sizeof(cpu_temp));
    get_packages(packages, sizeof(packages));
    get_disk(disk, sizeof(disk));
    get_local_ip(local_ip, sizeof(local_ip));

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

    /* Build info rows */
    char raw_info[15][512];
    char cpu_display[384];
    if (cpu_temp[0]) snprintf(cpu_display, sizeof(cpu_display), "%s (%s)", cpu, cpu_temp);
    else snprintf(cpu_display, sizeof(cpu_display), "%s", cpu);

#define ROW(dst, key, val) snprintf(dst, sizeof(dst), "%s" key "%s%s│%s %s", label_color, reset, border_color, reset, val)
    ROW(raw_info[0], "OS     ", os_name);
    ROW(raw_info[1], "Kernel ", kernel);
    ROW(raw_info[2], "Uptime ", uptime);
    ROW(raw_info[3], "Shell  ", shell);
    ROW(raw_info[4], "Term   ", terminal);
    ROW(raw_info[5], "CPU    ", cpu_display);
    int num_raw = 6;
    if (gpu[0])      { ROW(raw_info[num_raw], "GPU    ", gpu);      num_raw++; }
    if (packages[0]) { ROW(raw_info[num_raw], "Pkgs   ", packages); num_raw++; }
    ROW(raw_info[num_raw], "Memory ", memory_bar); num_raw++;
    if (disk[0])     { ROW(raw_info[num_raw], "Disk   ", disk);     num_raw++; }
    if (battery[0])  { ROW(raw_info[num_raw], "Battery", battery);  num_raw++; }
    if (local_ip[0]) { ROW(raw_info[num_raw], "IP     ", local_ip); num_raw++; }
#undef ROW

    /* Card width */
    int max_content_w = 0;
    for (int i = 0; i < num_raw; i++) {
        int w = visual_strlen(raw_info[i]);
        if (w > max_content_w) max_content_w = w;
    }
    int header_w = (int)(strlen(username) + 1 + strlen(hostname));
    if (header_w + 4 > max_content_w) max_content_w = header_w + 4;
    int card_w = max_content_w;

    char info_lines[18][2048];
    char *p = info_lines[0];
    p += sprintf(p, "%s╭", border_color);
    for (int i = 0; i < card_w + 2; i++) p += sprintf(p, "─");
    sprintf(p, "╮%s", reset);

    int pad_l = (card_w - header_w) / 2;
    int pad_r = card_w - header_w - pad_l;
    p = info_lines[1];
    p += sprintf(p, "%s│ %s", border_color, reset);
    for (int i = 0; i < pad_l; i++) *p++ = ' ';
    p += sprintf(p, "\033[1m%s%s%s@\033[1m%s%s%s", label_color, username, reset, label_color, hostname, reset);
    for (int i = 0; i < pad_r; i++) *p++ = ' ';
    sprintf(p, " %s│%s", border_color, reset);

    p = info_lines[2];
    p += sprintf(p, "%s├", border_color);
    for (int i = 0; i < card_w + 2; i++) p += sprintf(p, "─");
    sprintf(p, "┤%s", reset);

    for (int i = 0; i < num_raw; i++) {
        int pad = card_w - visual_strlen(raw_info[i]);
        char row[2048]; p = row;
        int w = snprintf(p, sizeof(row), "%s│ %s%s", border_color, reset, raw_info[i]); p += w;
        for (int j = 0; j < pad && (size_t)(p - row) < sizeof(row) - 10; j++) *p++ = ' ';
        snprintf(p, sizeof(row) - (p - row), " %s│%s", border_color, reset);
        strcpy(info_lines[3 + i], row);
    }

    p = info_lines[3 + num_raw];
    p += sprintf(p, "%s╰", border_color);
    for (int i = 0; i < card_w + 2; i++) p += sprintf(p, "─");
    sprintf(p, "╯%s", reset);

    int num_info_lines = 4 + num_raw;
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
    int pad_blocks = (cols - 24) / 2;         if (pad_blocks < 0) pad_blocks = 0;

    if (center_v && rows > total_lines) {
        printf("\033[H\033[2J");
        for (int i = 0; i < top_padding; i++) printf("\n");
    }

    for (int i = 0; i < num_ascii_lines; i++) {
        const char *line = is_embedded ? embedded_logo[i] : ascii_lines[i];
        for (int j = 0; j < pad_ascii; j++) printf(" ");
        printf("%s\n", line);
    }
    printf("\n");

    for (int i = 0; i < num_info_lines; i++) {
        for (int j = 0; j < pad_info; j++) printf(" ");
        printf("%s\n", info_lines[i]);
    }

    for (int j = 0; j < pad_blocks; j++) printf(" ");
    printf("\033[40m   \033[41m   \033[42m   \033[43m   \033[44m   \033[45m   \033[46m   \033[47m   %s\n", reset);

    if (center_v && rows > total_lines) for (int i = 0; i < bot_padding; i++) printf("\n");

    free_ascii(ascii_lines, num_ascii_lines);
    return 0;
}
