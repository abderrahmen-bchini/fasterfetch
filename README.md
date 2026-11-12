<div align="center">

# fasterfetch

**A retro system-info fetch tool for Linux and macOS, written in C.**

Reads OS, CPU, memory, and more directly from `/proc`, `/sys`, and `sysctl` — no shell subprocesses, no runtime dependencies, no bloat.

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![Platforms](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey)
![Language](https://img.shields.io/badge/language-C-555.svg)

</div>

```
           ╭────────────────────────────────────────────────────────╮
           │                     user@hostname                      │
           ├────────────────────────────────────────────────────────┤
           │ OS     │ Arch Linux                                    │
           │ Kernel │ Linux 6.12.4                                  │
           │ Uptime │ 2 hours, 17 mins                              │
           │ Shell  │ zsh                                           │
           │ Term   │ kitty                                         │
           │ CPU    │ AMD Ryzen 5 5600H (42°C)                      │
           │ GPU    │ NVIDIA [25a0]                                 │
           │ Memory │ ━━━━━━━━━━ 27% (3.70 / 13.50 GiB)             │
           │ Disk   │ ━━━━━━━━━━ 20% (92.5 / 451.6 GiB)             │
           ╰────────────────────────────────────────────────────────╯
```

*(representative output — colors, logo, and fields depend on your distro, theme, and config)*

## Contents

- [Why fasterfetch](#why-fasterfetch)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Config file](#config-file)
- [Themes](#themes)
- [ASCII art search order](#ascii-art-search-order)
- [Building from source](#building-from-source)
- [Uninstall](#uninstall)
- [License](#license)

## Why fasterfetch

Most fetch tools (`neofetch`, `fastfetch`) shell out to other programs — `lspci`, `df`, `free` — to
gather system info. fasterfetch reads the same data straight from the kernel interfaces those tools
wrap around, so there's no process spawning and no parsing of other programs' output. The binary has
zero runtime dependencies, including its own JSON config parser.

## Features

- OS, Kernel, Uptime, Shell, Terminal, CPU (+ temperature), GPU, Memory, Disk, Battery, Packages, Local IP
- GPU name resolved from `pci.ids`; CPU temperature via `/sys/class/thermal`
- JSON config file — pick which modules show and in what order, theme/color overrides, border style, and layout position, all without recompiling
- Named color themes: `nord`, `dracula`, `gruvbox`, `catppuccin`, `amber`
- Built-in logos for Arch, Debian, Ubuntu, Fedora, Alpine, Kali, Gentoo, Mint, NixOS, Red Hat, and macOS
- Custom ASCII art support, with a configurable search path
- Full-screen vertical centering, or inline/scrollback mode
- Packaged for Arch (`PKGBUILD`), Debian/Ubuntu (`debian/`), and NixOS (`flake.nix`)

## Installation

<table>
<tr><th>Arch Linux</th><td>

```bash
makepkg -si
```

</td></tr>
<tr><th>Debian / Ubuntu</th><td>

```bash
sudo apt-get install build-essential devscripts debhelper
dpkg-buildpackage -us -uc -b
sudo dpkg -i ../fasterfetch_*.deb
```

</td></tr>
<tr><th>NixOS / Nix</th><td>

```bash
nix build .#default && ./result/bin/fasterfetch
# or install into your profile:
nix profile install .
```

</td></tr>
<tr><th>Any Linux / macOS</th><td>

```bash
sudo bash install.sh              # installs to /usr/local
PREFIX=~/.local bash install.sh   # or without sudo
```

</td></tr>
<tr><th>Manual / from source</th><td>

```bash
make
sudo make install
```

Works on macOS too — swap `gcc` for `clang` if needed (`make CC=clang`), though `gcc` usually works fine.

</td></tr>
</table>

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
fasterfetch -c ~/my-config.json    # use a specific config file
fasterfetch --no-config            # ignore config.json entirely
fasterfetch -h                     # help
```

CLI flags always override a config file, which always overrides the built-in defaults.

## Config file

fasterfetch reads an optional JSON config file to customize themes, which modules are shown and in
what order, ASCII art defaults, and layout — without recompiling.

**Search order** (first match wins):

| Order | Location |
|---|---|
| 1 | `-c`/`--config <path>` — explicit path |
| 2 | `~/.config/fasterfetch/config.json` — user config |
| 3 | `/etc/fasterfetch/config.json` — system-wide config |
| 4 | built-in defaults (today's look, unchanged) |

A malformed or unreadable config file never crashes fasterfetch — it prints a warning to stderr and
falls back to the next tier / built-in defaults. `--no-config` skips the search entirely.

<details>
<summary><strong>Example <code>config.json</code></strong></summary>

```json
{
  "theme": "nord",
  "colors": { "border": "", "label": "" },
  "modules": ["os", "kernel", "uptime", "shell", "term", "cpu", "memory", "disk", "battery", "ip"],
  "ascii": { "path": "", "force_logo": "arch" },
  "layout": {
    "center_vertical": true,
    "separator": "│",
    "pad_left": 1,
    "pad_right": 1,
    "border_style": "rounded",
    "position": "center",
    "margin": 0
  }
}
```

</details>

| Key | Description |
|---|---|
| `theme` | Named theme (`nord`, `dracula`, `gruvbox`, `catppuccin`, `amber`) |
| `colors.border` / `colors.label` | Raw ANSI escape overrides, win over `theme` when non-empty |
| `modules` | Which info rows to show, and in what order. Available keys: `os`, `kernel`, `uptime`, `shell`, `term`, `cpu`, `gpu`, `pkgs`, `memory`, `disk`, `battery`, `ip` |
| `ascii.path` | Default ASCII art file (same as `-a`) |
| `ascii.force_logo` | Default forced distro logo (same as `-f`) |
| `layout.center_vertical` | Default for `-n` (vertical centering) |
| `layout.separator` | Character between a label and its value (default `│`) |
| `layout.pad_left` / `layout.pad_right` | Spaces between the card border and its content |
| `layout.border_style` | `rounded` (default), `square`, `ascii`, or `none` |
| `layout.position` | `center` (default), `left`, or `right` — horizontal placement on screen |
| `layout.margin` | Left/right margin used by `left`/`right` position |

A runnable example lives at [`config.example.json`](config.example.json) —
try it with `fasterfetch -c config.example.json`.

## Themes

| Name | Description |
|---|---|
| `default` | Auto-detected from distro |
| `nord` | Arctic blue palette |
| `dracula` | Purple/pink palette |
| `gruvbox` | Warm orange/yellow palette |
| `catppuccin` | Soft lavender palette |
| `amber` | Golden amber palette |

## ASCII art search order

| Order | Source |
|---|---|
| 1 | `-a <file>` — explicit path |
| 2 | `ascii.path` in the config file |
| 3 | `~/.config/fasterfetch/ascii.txt` — user config |
| 4 | `./ascii.txt` — current directory |
| 5 | `<exe_dir>/ascii.txt` — next to binary |
| 6 | `/usr/share/fasterfetch/ascii.txt` — system default |
| 7 | Built-in distro logo (auto-detected, or forced with `-f` / `ascii.force_logo`) |
| 8 | Built-in "FASTERFETCH" banner (last resort) |

## Building from source

Works on Linux and macOS (requires a C compiler and `make`), with no external dependencies —
including the JSON config parser, which is a small hand-rolled parser in `src/utils/json.c` rather
than a vendored library.

```bash
make              # build ./fasterfetch
make CC=clang     # build with clang
make clean        # remove binary and .o files
```

## Uninstall

```bash
sudo bash uninstall.sh   # quick
sudo make uninstall      # manual
```

## License

MIT — see [LICENSE](LICENSE).
