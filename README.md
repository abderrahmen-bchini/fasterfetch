# myfetch

A retro fetch tool for Linux and macOS. Reads system info directly from kernel interfaces — no shell subprocesses, no bloat.

## Features

- OS, Kernel, Uptime, Shell, Terminal, CPU, GPU, Memory, Disk, Battery, Packages, Local IP
- CPU temperature via `/sys/class/thermal`
- GPU name resolved from `pci.ids`
- Named color themes: nord, dracula, gruvbox, catppuccin, amber
- Built-in logos for Arch, Debian, Ubuntu, Fedora, Alpine, Kali, Gentoo, Mint, NixOS, Red Hat, and macOS (rainbow Apple logo)
- Custom ASCII art support
- Full-screen vertical centering

## Installation

**Quick install (requires root for /usr/local):**

```bash
sudo bash install.sh
```

Or without sudo:

```bash
PREFIX=~/.local bash install.sh
```

**Manual:**

```bash
make
sudo make install
```

Works on macOS too, just swap `gcc` for `clang` if needed (though `gcc` usually works fine).

**Set your personal ASCII art:**

```bash
mkdir -p ~/.config/myfetch
cp ascii.txt ~/.config/myfetch/ascii.txt
```

## Usage

```bash
myfetch                        # full-screen centered output
myfetch -n                     # inline/scroll mode
myfetch -t nord                # Nord color theme
myfetch -t dracula -n          # Dracula theme, no vertical centering
myfetch -f arch                # force Arch logo
myfetch -f macos               # force the macOS Apple logo
myfetch -a ~/my-art.txt        # custom ASCII art file
myfetch -h                     # help
```

## Themes

| Name          | Description                  |
|---------------|------------------------------|
| `default`     | Auto-detected from distro    |
| `nord`        | Arctic blue palette          |
| `dracula`     | Purple/pink palette          |
| `gruvbox`     | Warm orange/yellow palette   |
| `catppuccin`  | Soft lavender palette        |
| `amber`       | Golden amber palette         |

## ASCII Art Search Order

1. `-a <file>` — explicit path
2. `~/.config/myfetch/ascii.txt` — user config
3. `./ascii.txt` — current directory
4. `<exe_dir>/ascii.txt` — next to binary
5. `/usr/share/myfetch/ascii.txt` — system default
6. Built-in distro logo (auto-detected or forced with `-f`)
7. Built-in "MY FETCH" banner (last resort)

## Build from Source

**Linux:**
```bash
gcc -O2 -Wall -Wextra myfetch.c -o myfetch
```

**macOS:**
```bash
clang -O2 -Wall -Wextra myfetch.c -o myfetch
```

No external dependencies on either platform.

## Uninstall

```bash
sudo make uninstall
```

## License

idk everyone i guess , just mention me :)
