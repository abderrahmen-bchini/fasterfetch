#include "sysinfo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <time.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <libproc.h>
#endif

#ifdef __APPLE__
void get_os(char *buf, size_t max_len) {
    char version[64] = "Unknown";
    size_t len = sizeof(version);
    sysctlbyname("kern.osproductversion", version, &len, NULL, 0);
    snprintf(buf, max_len, "macOS %s", version);
}
#else
void get_os(char *buf, size_t max_len) {
    FILE *fp = fopen("/etc/os-release", "r");
    if (!fp) { set_buf(buf, max_len, "Linux"); return; }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
            char *val = line + 12;
            if (*val == '"') val++;
            char *end = val + strlen(val) - 1;
            while (end >= val && (*end == '\n' || *end == '\r' || *end == '"')) *end-- = '\0';
            set_buf(buf, max_len, val);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
    set_buf(buf, max_len, "Linux");
}
#endif

void get_kernel(char *buf, size_t max_len) {
    struct utsname u;
    if (uname(&u) == 0) snprintf(buf, max_len, "%s %s", u.sysname, u.release);
    else set_buf(buf, max_len, "Linux");
}

#ifdef __APPLE__
void get_uptime(char *buf, size_t max_len) {
    struct timeval boottime;
    size_t len = sizeof(boottime);
    if (sysctlbyname("kern.boottime", &boottime, &len, NULL, 0) != 0) {
        set_buf(buf, max_len, "Unknown"); return;
    }
    double secs = difftime(time(NULL), boottime.tv_sec);
    int days  = (int)(secs / 86400);
    int hours = (int)((secs - days * 86400) / 3600);
    int mins  = (int)((secs - days * 86400 - hours * 3600) / 60);
    if (days > 0)       snprintf(buf, max_len, "%d days, %d hours, %d mins", days, hours, mins);
    else if (hours > 0) snprintf(buf, max_len, "%d hours, %d mins", hours, mins);
    else                snprintf(buf, max_len, "%d mins", mins);
}
#else
void get_uptime(char *buf, size_t max_len) {
    FILE *fp = fopen("/proc/uptime", "r");
    if (!fp) { set_buf(buf, max_len, "Unknown"); return; }
    double secs;
    int ok = (fscanf(fp, "%lf", &secs) == 1);
    fclose(fp);
    if (!ok) { set_buf(buf, max_len, "Unknown"); return; }

    int days  = (int)(secs / 86400);
    int hours = (int)((secs - days * 86400) / 3600);
    int mins  = (int)((secs - days * 86400 - hours * 3600) / 60);

    if (days > 0)  snprintf(buf, max_len, "%d days, %d hours, %d mins", days, hours, mins);
    else if (hours > 0) snprintf(buf, max_len, "%d hours, %d mins", hours, mins);
    else snprintf(buf, max_len, "%d mins", mins);
}
#endif

void get_shell(char *buf, size_t max_len) {
    char *path = getenv("SHELL");
    if (!path) {
        struct passwd *pw = getpwuid(getuid());
        path = pw ? pw->pw_shell : NULL;
    }
    if (!path) path = "/bin/sh";
    char *base = strrchr(path, '/');
    set_buf(buf, max_len, base ? base + 1 : path);
}

#ifdef __APPLE__
static pid_t get_process_info(pid_t pid, char *comm, size_t comm_len) {
    struct proc_bsdinfo info;
    if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &info, sizeof(info)) <= 0)
        return -1;
    snprintf(comm, comm_len, "%s", info.pbi_name);
    return (pid_t)info.pbi_ppid;
}
#else
static pid_t get_process_info(pid_t pid, char *comm, size_t comm_len) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    char buf[1024];
    int ok = (fgets(buf, sizeof(buf), fp) != NULL);
    fclose(fp);
    if (!ok) return -1;
    char *last_paren = strrchr(buf, ')');
    if (!last_paren) return -1;
    char *first_paren = strchr(buf, '(');
    if (first_paren && first_paren < last_paren) {
        size_t name_len = (size_t)(last_paren - (first_paren + 1));
        if (name_len >= comm_len) name_len = comm_len - 1;
        strncpy(comm, first_paren + 1, name_len);
        comm[name_len] = '\0';
    } else {
        set_buf(comm, comm_len, "unknown");
    }
    pid_t ppid = -1;
    char state;
    sscanf(last_paren + 1, " %c %d", &state, &ppid);
    return ppid;
}
#endif

void get_terminal(char *buf, size_t max_len) {
    const char *tp = getenv("TERM_PROGRAM");
    if (tp) {
        if      (strcmp(tp, "vscode")        == 0) { set_buf(buf, max_len, "Visual Studio Code"); return; }
        else if (strcmp(tp, "Apple_Terminal") == 0) { set_buf(buf, max_len, "Apple Terminal");    return; }
        else if (strcmp(tp, "iTerm.app")      == 0) { set_buf(buf, max_len, "iTerm2");            return; }
        else if (strcmp(tp, "WarpTerminal")   == 0) { set_buf(buf, max_len, "Warp");              return; }
        else if (strcmp(tp, "WezTerm")        == 0) { set_buf(buf, max_len, "WezTerm");           return; }
        else if (strcmp(tp, "Tabby")          == 0) { set_buf(buf, max_len, "Tabby");             return; }
        else if (strcmp(tp, "Hyper")          == 0) { set_buf(buf, max_len, "Hyper");             return; }
        else                                        { set_buf(buf, max_len, tp);                  return; }
    }
    const char *te = getenv("TERMINAL_EMULATOR");
    if (te) { set_buf(buf, max_len, te); return; }
    if (getenv("KONSOLE_VERSION") || getenv("KONSOLE_INFO"))         { set_buf(buf, max_len, "Konsole");        return; }
    if (getenv("GNOME_TERMINAL_SCREEN") || getenv("GNOME_TERMINAL_SERVICE")) { set_buf(buf, max_len, "GNOME Terminal"); return; }
    if (getenv("MATE_TERMINAL_SCREEN"))                              { set_buf(buf, max_len, "MATE Terminal");  return; }
    if (getenv("KITTY_WINDOW_ID"))                                   { set_buf(buf, max_len, "kitty");          return; }
    if (getenv("ALACRITTY_WINDOW_ID") || getenv("ALACRITTY_LOG"))   { set_buf(buf, max_len, "Alacritty");      return; }
    if (getenv("WT_SESSION"))                                        { set_buf(buf, max_len, "Windows Terminal"); return; }
    const char *term = getenv("TERM");
    if (term) {
        if      (strcmp(term, "xterm-kitty") == 0) { set_buf(buf, max_len, "kitty");     return; }
        else if (strcmp(term, "alacritty")   == 0) { set_buf(buf, max_len, "Alacritty"); return; }
        else if (strcmp(term, "foot")        == 0) { set_buf(buf, max_len, "foot");      return; }
    }
    static const char *known_terminals[] = {
        "gnome-terminal", "gnome-terminal-", "gnome-terminal-server",
        "konsole", "konsolebin", "xfce4-terminal", "lxterminal",
        "mate-terminal", "terminator", "alacritty", "kitty",
        "wezterm", "wezterm-gui", "urxvt", "rxvt", "rxvt-unicode",
        "xterm", "st", "cool-retro-term", "guake", "tilda", "tilix",
        "terminology", "qterminal", "deepin-terminal", "pantheon-terminal",
        "foot", "blackbox", "tabby", "hyper", "rio", "contour", "ghostty",
        "footclient", NULL
    };
    static const char *skip_list[] = {
        "bash", "zsh", "sh", "fish", "orig-fish", "ksh", "dash", "csh", "tcsh",
        "sudo", "su", "doas", "systemd", "init", "login",
        "fasterfetch", "sh.exe", "bash.exe", NULL
    };
    char matched[256] = "";
    char fallback[256] = "";
    pid_t ppid = getppid();
    for (int depth = 0; depth < 10 && ppid > 1; depth++) {
        char comm[256] = "";
        pid_t next = get_process_info(ppid, comm, sizeof(comm));
        if (next <= 0 || ppid == next) break;
        int found = 0;
        for (int i = 0; known_terminals[i]; i++) {
            if (strcmp(comm, known_terminals[i]) == 0 || strstr(comm, known_terminals[i])) {
                snprintf(matched, sizeof(matched), "%s", comm);
                found = 1; break;
            }
        }
        if (found) break;
        if (fallback[0] == '\0') {
            int skip = 0;
            for (int i = 0; skip_list[i]; i++) {
                if (strcmp(comm, skip_list[i]) == 0) { skip = 1; break; }
            }
            if (!skip) snprintf(fallback, sizeof(fallback), "%s", comm);
        }
        ppid = next;
    }
    char *det = matched[0] ? matched : fallback;
    if (det[0]) {
        if      (strstr(det, "gnome-terminal"))                              set_buf(buf, max_len, "GNOME Terminal");
        else if (strcmp(det, "konsole")   == 0 || strcmp(det, "konsolebin") == 0) set_buf(buf, max_len, "Konsole");
        else if (strcmp(det, "xfce4-terminal") == 0)                         set_buf(buf, max_len, "XFCE Terminal");
        else if (strcmp(det, "mate-terminal")  == 0)                         set_buf(buf, max_len, "MATE Terminal");
        else if (strcmp(det, "lxterminal")     == 0)                         set_buf(buf, max_len, "LXTerminal");
        else if (strcmp(det, "urxvt")  == 0 || strcmp(det, "rxvt-unicode") == 0) set_buf(buf, max_len, "URxvt");
        else if (strcmp(det, "alacritty")  == 0)                             set_buf(buf, max_len, "Alacritty");
        else if (strcmp(det, "kitty")      == 0)                             set_buf(buf, max_len, "kitty");
        else if (strcmp(det, "wezterm") == 0 || strcmp(det, "wezterm-gui") == 0) set_buf(buf, max_len, "WezTerm");
        else if (strcmp(det, "terminator") == 0)                             set_buf(buf, max_len, "Terminator");
        else if (strcmp(det, "tilix")      == 0)                             set_buf(buf, max_len, "Tilix");
        else if (strcmp(det, "tilda")      == 0)                             set_buf(buf, max_len, "Tilda");
        else if (strcmp(det, "guake")      == 0)                             set_buf(buf, max_len, "Guake");
        else if (strcmp(det, "qterminal")  == 0)                             set_buf(buf, max_len, "QTerminal");
        else if (strcmp(det, "foot") == 0 || strcmp(det, "footclient") == 0) set_buf(buf, max_len, "foot");
        else if (strcmp(det, "xterm")      == 0)                             set_buf(buf, max_len, "Xterm");
        else if (strcmp(det, "ghostty")    == 0)                             set_buf(buf, max_len, "Ghostty");
        else {
            if (det[0] >= 'a' && det[0] <= 'z') det[0] = det[0] - 'a' + 'A';
            set_buf(buf, max_len, det);
        }
        return;
    }
    if (term) set_buf(buf, max_len, term);
    else      set_buf(buf, max_len, "Unknown");
}

static void clean_cpu_string(char *cpu) {
    char *p;
    while ((p = strstr(cpu, "(R)"))   != NULL) memmove(p, p + 3, strlen(p + 3) + 1);
    while ((p = strstr(cpu, "(TM)"))  != NULL) memmove(p, p + 4, strlen(p + 4) + 1);
    while ((p = strstr(cpu, "CPU"))   != NULL) memmove(p, p + 3, strlen(p + 3) + 1);
    while ((p = strstr(cpu, "Processor")) != NULL) memmove(p, p + 9, strlen(p + 9) + 1);
    int i = 0, j = 0;
    char last = '\0';
    while (cpu[i]) {
        if (isspace((unsigned char)cpu[i])) {
            if (last != ' ' && j > 0) { cpu[j++] = ' '; last = ' '; }
        } else {
            cpu[j++] = cpu[i]; last = cpu[i];
        }
        i++;
    }
    if (j > 0 && cpu[j-1] == ' ') cpu[j-1] = '\0';
    else                           cpu[j]   = '\0';
}

#ifdef __APPLE__
void get_cpu(char *buf, size_t max_len) {
    char tmp[256] = "";
    size_t len = sizeof(tmp);
    if (sysctlbyname("machdep.cpu.brand_string", tmp, &len, NULL, 0) == 0) {
        clean_cpu_string(tmp); set_buf(buf, max_len, tmp);
    } else set_buf(buf, max_len, "Unknown CPU");
}
#else
void get_cpu(char *buf, size_t max_len) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) { set_buf(buf, max_len, "Unknown"); return; }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0 || strncmp(line, "Model", 5) == 0) {
            char *val = strchr(line, ':');
            if (val) {
                val++;
                while (isspace((unsigned char)*val)) val++;
                char tmp[256];
                set_buf(tmp, sizeof(tmp), val);
                trim_trailing(tmp);
                clean_cpu_string(tmp);
                set_buf(buf, max_len, tmp);
                fclose(fp); return;
            }
        }
    }
    fclose(fp);
    set_buf(buf, max_len, "Unknown CPU");
}
#endif

#ifdef __APPLE__
void get_memory_bar(char *buf, size_t max_len) {
    uint64_t total_bytes = 0;
    size_t len = sizeof(total_bytes);
    if (sysctlbyname("hw.memsize", &total_bytes, &len, NULL, 0) != 0) {
        set_buf(buf, max_len, "Unknown"); return;
    }
    vm_size_t page_size = 0;
    host_page_size(mach_host_self(), &page_size);
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmstat, &count) != KERN_SUCCESS) {
        set_buf(buf, max_len, "Unknown"); return;
    }
    uint64_t used_bytes = (uint64_t)(vmstat.active_count + vmstat.wire_count) * page_size;
    double pct = total_bytes > 0 ? ((double)used_bytes / total_bytes) * 100.0 : 0.0;
    double total = total_bytes / 1073741824.0;
    double used = used_bytes / 1073741824.0;
    char bar[96]; build_bar(bar, sizeof(bar), pct, 10, 0);
    snprintf(buf, max_len, "%s %.0f%% (%.2f / %.2f GiB)", bar, pct, used, total);
}
#else
void get_memory_bar(char *buf, size_t max_len) {
    long total_kb, used_kb;
    if (!read_meminfo_kb(&total_kb, &used_kb)) { set_buf(buf, max_len, "Unknown"); return; }
    double pct = total_kb > 0 ? ((double)used_kb / total_kb) * 100.0 : 0.0;
    double total = total_kb / 1048576.0;
    double used = used_kb / 1048576.0;
    char bar[96]; build_bar(bar, sizeof(bar), pct, 10, 0);
    snprintf(buf, max_len, "%s %.0f%% (%.2f / %.2f GiB)", bar, pct, used, total);
}
#endif

#ifdef __APPLE__
void get_battery(char *buf, size_t max_len) { (void)max_len; buf[0] = '\0'; }
#else
void get_battery(char *buf, size_t max_len) {
    static const char *bat_paths[] = { "/sys/class/power_supply/BAT0", "/sys/class/power_supply/BAT1", "/sys/class/power_supply/BAT2" };
    int capacity = -1; char status[64] = "";
    for (int d = 0; d < 3; d++) {
        char path[256]; snprintf(path, sizeof(path), "%s/capacity", bat_paths[d]);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;
        if (fscanf(fp, "%d", &capacity) != 1) { fclose(fp); capacity = -1; continue; }
        fclose(fp);
        snprintf(path, sizeof(path), "%s/status", bat_paths[d]);
        fp = fopen(path, "r");
        if (fp) { if (fgets(status, sizeof(status), fp)) trim_trailing(status); fclose(fp); }
        break;
    }
    if (capacity < 0) { buf[0] = '\0'; return; }
    char bar[96]; build_bar(bar, sizeof(bar), (double)capacity, 10, 1);
    const char *suffix = "";
    if      (strcmp(status, "Charging") == 0) suffix = " Charging";
    else if (strcmp(status, "Not charging") == 0) suffix = " Plugged in";
    else if (strcmp(status, "Full") == 0) suffix = " Full";
    snprintf(buf, max_len, "%s %d%%%s", bar, capacity, suffix);
}
#endif

#ifdef __APPLE__
void get_gpu(char *buf, size_t max_len) { (void)max_len; buf[0] = '\0'; }
#else
void get_gpu(char *buf, size_t max_len) {
    static const char *card_paths[] = { "/sys/class/drm/card0", "/sys/class/drm/card1", "/sys/class/drm/card2" };
    unsigned int vendor_id = 0, device_id = 0;
    for (int d = 0; d < 3; d++) {
        char path[256]; snprintf(path, sizeof(path), "%s/device/vendor", card_paths[d]);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;
        if (fscanf(fp, "%x", &vendor_id) != 1) { fclose(fp); vendor_id = 0; continue; }
        fclose(fp);
        snprintf(path, sizeof(path), "%s/device/device", card_paths[d]);
        fp = fopen(path, "r"); if (fp) { fscanf(fp, "%x", &device_id); fclose(fp); }
        break;
    }
    if (!vendor_id) { buf[0] = '\0'; return; }
    unsigned int vid = vendor_id & 0xFFFF;
    unsigned int did = device_id & 0xFFFF;
    char vendor_short[16] = "";
    if      (vid == 0x1002) set_buf(vendor_short, sizeof(vendor_short), "AMD");
    else if (vid == 0x10de) set_buf(vendor_short, sizeof(vendor_short), "NVIDIA");
    else if (vid == 0x8086) set_buf(vendor_short, sizeof(vendor_short), "Intel");
    else                    snprintf(vendor_short, sizeof(vendor_short), "%04x", vid);
    static const char *pci_ids_paths[] = { "/usr/share/hwdata/pci.ids", "/usr/share/misc/pci.ids", "/usr/share/pci.ids" };
    char device_name[256] = "";
    char target_vendor[8]; snprintf(target_vendor, sizeof(target_vendor), "%04x", vid);
    for (int p = 0; p < 3 && !device_name[0]; p++) {
        FILE *fp = fopen(pci_ids_paths[p], "r"); if (!fp) continue;
        char line[512]; int in_vendor = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (line[0] == '#' || line[0] == '\r' || line[0] == '\n') continue;
            if (!in_vendor) { if (strncmp(line, target_vendor, 4) == 0 && (line[4] == ' ' || line[4] == '\t')) in_vendor = 1; }
            else {
                if (line[0] != '\t') break;
                if (line[1] != '\t') {
                    unsigned int line_did = 0;
                    if (sscanf(line + 1, "%x", &line_did) == 1 && line_did == did) {
                        char *name = line + 5; while (*name == ' ' || *name == '\t') name++;
                        snprintf(device_name, sizeof(device_name), "%.*s", (int)(sizeof(device_name) - 1), name);
                        trim_trailing(device_name); break;
                    }
                }
            }
        }
        fclose(fp);
    }
    if (device_name[0]) snprintf(buf, max_len, "%s", device_name);
    else                snprintf(buf, max_len, "%s [%04x]", vendor_short, did);
}
#endif

#ifdef __APPLE__
void get_cpu_temp(char *buf, size_t max_len) { (void)max_len; buf[0] = '\0'; }
#else
void get_cpu_temp(char *buf, size_t max_len) {
    for (int i = 0; i < 10; i++) {
        char path[256]; snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%d/temp", i);
        FILE *fp = fopen(path, "r"); if (!fp) continue;
        int millic = 0;
        if (fscanf(fp, "%d", &millic) == 1 && millic > 0) { snprintf(buf, max_len, "%d°C", millic / 1000); fclose(fp); return; }
        fclose(fp);
    }
    buf[0] = '\0';
}
#endif

#ifdef __APPLE__
void get_packages(char *buf, size_t max_len) {
    static const char *brew_paths[] = { "/opt/homebrew/Cellar", "/usr/local/Cellar", NULL };
    for (int i = 0; brew_paths[i]; i++) {
        DIR *dir = opendir(brew_paths[i]); if (!dir) continue;
        int count = 0; struct dirent *e;
        while ((e = readdir(dir))) if (e->d_name[0] != '.') count++;
        closedir(dir); if (count > 0) { snprintf(buf, max_len, "%d (brew)", count); return; }
    }
    DIR *dir = opendir("/opt/local/var/macports/registry/installed");
    if (dir) {
        int count = 0; struct dirent *e;
        while ((e = readdir(dir))) if (e->d_name[0] != '.') count++;
        closedir(dir); if (count > 0) { snprintf(buf, max_len, "%d (ports)", count); return; }
    }
    buf[0] = '\0';
}
#else
void get_packages(char *buf, size_t max_len) {
    int count = 0; DIR *dir = opendir("/var/lib/pacman/local");
    if (dir) {
        struct dirent *e; while ((e = readdir(dir))) if (e->d_name[0] != '.') count++;
        closedir(dir); if (count > 0) { snprintf(buf, max_len, "%d (pacman)", count); return; }
    }
    dir = opendir("/var/lib/dpkg/info");
    if (dir) {
        struct dirent *e; while ((e = readdir(dir))) { size_t len = strlen(e->d_name); if (len > 5 && strcmp(e->d_name + len - 5, ".list") == 0) count++; }
        closedir(dir); if (count > 0) { snprintf(buf, max_len, "%d (dpkg)", count); return; }
    }
    FILE *fp = fopen("/lib/apk/db/installed", "r");
    if (!fp) fp = fopen("/var/lib/apk/db/installed", "r");
    if (fp) { char line[256]; while (fgets(line, sizeof(line), fp)) if (strncmp(line, "P:", 2) == 0) count++; fclose(fp); if (count > 0) { snprintf(buf, max_len, "%d (apk)", count); return; } }
    buf[0] = '\0';
}
#endif

void get_disk(char *buf, size_t max_len) {
    struct statvfs vfs; if (statvfs("/", &vfs) != 0) { buf[0] = '\0'; return; }
    unsigned long long total = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
    unsigned long long free  = (unsigned long long)vfs.f_bfree  * vfs.f_frsize;
    unsigned long long used  = total - free;
    if (!total) { buf[0] = '\0'; return; }
    double pct = ((double)used / total) * 100.0;
    double total_gib = total / 1073741824.0;
    double used_gib  = used  / 1073741824.0;
    char bar[96]; build_bar(bar, sizeof(bar), pct, 10, 0);
    snprintf(buf, max_len, "%s %.0f%% (%.1f / %.1f GiB)", bar, pct, used_gib, total_gib);
}

void get_local_ip(char *buf, size_t max_len) {
    struct ifaddrs *ifaddr; if (getifaddrs(&ifaddr) == -1) { buf[0] = '\0'; return; }
    for (struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_flags & IFF_LOOPBACK) continue;
        if (ifa->ifa_addr->sa_family != AF_INET) continue;
        struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
        inet_ntop(AF_INET, &addr->sin_addr, buf, max_len);
        freeifaddrs(ifaddr); return;
    }
    freeifaddrs(ifaddr); buf[0] = '\0';
}
