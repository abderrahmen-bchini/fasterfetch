# fasterfetch

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
mkdir -p ~/.config/fasterfetch
cp ascii.txt ~/.config/fasterfetch/ascii.txt
```

## Usage

```bash
fasterfetch                        # full-screen centered output
fasterfetch -n                     # inline/scroll mode
fasterfetch -t nord                # Nord color theme
fasterfetch -t dracula -n          # Dracula theme, no vertical centering
fasterfetch -f arch                # force Arch logo
fasterfetch -f macos               # force the macOS Apple logo
fasterfetch -a ~/my-art.txt        # custom ASCII art file
fasterfetch -h                     # help
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
2. `~/.config/fasterfetch/ascii.txt` — user config
3. `./ascii.txt` — current directory
4. `<exe_dir>/ascii.txt` — next to binary
5. `/usr/share/fasterfetch/ascii.txt` — system default
6. Built-in distro logo (auto-detected or forced with `-f`)
7. Built-in "FASTERFETCH" banner (last resort)

## Build from Source

**Linux:**
```bash
gcc -O2 -Wall -Wextra fasterfetch.c -o fasterfetch
```

**macOS:**
```bash
clang -O2 -Wall -Wextra fasterfetch.c -o fasterfetch
```

No external dependencies on either platform.

## Uninstall

```bash
sudo make uninstall
```

## License

idk everyone i guess , just mention me :)
