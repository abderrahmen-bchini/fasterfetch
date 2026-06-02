#include "logos.h"
#include <string.h>

const char *default_ascii[] = {
    "███████╗ █████╗ ███████╗████████╗███████╗██████╗ ███████╗███████╗████████╗ ██████╗██╗  ██╗",
    "██╔════╝██╔══██╗██╔════╝╚══██╔══╝██╔════╝██╔══██╗██╔════╝██╔════╝╚══██╔══╝██╔════╝██║  ██║",
    "█████╗  ███████║███████╗   ██║   █████╗  ██████╔╝█████╗  █████╗     ██║   ██║     ███████║",
    "██╔══╝  ██╔══██║╚════██║   ██║   ██╔══╝  ██╔══██╗██╔══╝  ██╔══╝     ██║   ██║     ██╔══██║",
    "██║     ██║  ██║███████║   ██║   ███████╗██║  ██║██║     ███████╗   ██║   ╚██████╗██║  ██║",
    "╚═╝     ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚══════╝   ╚═╝    ╚═════╝╚═╝  ╚═╝"
};
const int default_ascii_lines = 6;

static const char *arch_logo[] = {
    "\033[38;5;39m                   -`\033[0m",
    "\033[38;5;39m                  .o+`\033[0m",
    "\033[38;5;39m                 `ooo/\033[0m",
    "\033[38;5;39m                `+oooo:\033[0m",
    "\033[38;5;39m               `+oooooo:\033[0m",
    "\033[38;5;39m               -+oooooo+:\033[0m",
    "\033[38;5;39m             `/:-:++oooo+:\033[0m",
    "\033[38;5;39m            `/++++/+++++++:\033[0m",
    "\033[38;5;39m           `/++++++++++++++:\033[0m",
    "\033[38;5;39m          `/+++o\033[38;5;254moooooooo\033[38;5;39moooo/`\033[0m",
    "\033[38;5;254m         \033[38;5;39m./\033[38;5;254mooosssso++osssssso\033[38;5;39m+`\033[0m",
    "\033[38;5;254m        .oossssso-````/ossssss+`\033[0m",
    "\033[38;5;254m       -osssssso.      :ssssssso.\033[0m",
    "\033[38;5;254m      :osssssss/        osssso+++.\033[0m",
    "\033[38;5;254m     /ossssssss/        +ssssooo/-\033[0m",
    "\033[38;5;254m   `/ossssso+/:-        -:/+osssso+-\033[0m",
    "\033[38;5;254m  `+sso+:-`                 `.-/+oso:\033[0m",
    "\033[38;5;254m `++:.                           `-/+/\033[0m",
    "\033[38;5;254m .`                                 `/\033[0m"
};
static const int arch_logo_lines = 19;

static const char *debian_logo[] = {
    "\033[38;5;251m       _,met$$$$$gg.\033[0m",
    "\033[38;5;251m    ,g$$$$$$$$$$$$$$$P.\033[0m",
    "\033[38;5;251m  ,g$$P\\\"        \\\"\\\"\\\"Y$$\\\".\".\033[0m",
    "\033[38;5;251m ,$$P'              `$$$.\033[0m",
    "\033[38;5;251m',$$P       ,ggs.     `$$b:\033[0m",
    "\033[38;5;251m`d$$'     ,$P\\\"'   \033[38;5;161m.\033[38;5;251m    $$$\033[0m",
    "\033[38;5;251m $$P      d$'     \033[38;5;161m,\033[38;5;251m    $$P\033[0m",
    "\033[38;5;251m $$:      $$.   \033[38;5;161m-\033[38;5;251m    ,d$$'\033[0m",
    "\033[38;5;251m $$;      Y$b._   _,d$P'\033[0m",
    "\033[38;5;251m Y$$.    \033[38;5;161m`.\033[38;5;251m`\\\"Y$$$$P\\\"'\033[0m",
    "\033[38;5;251m `$$b      \033[38;5;161m\\\"-.__\033[0m",
    "\033[38;5;251m  `Y$$\033[0m",
    "\033[38;5;251m    `Y$$.\033[0m",
    "\033[38;5;251m      `$$b.\033[0m",
    "\033[38;5;251m        `Y$$b.\033[0m",
    "\033[38;5;251m           `\\\"Y$b._\033[0m",
    "\033[38;5;251m               `\\\"\\\"\\\"\033[0m"
};
static const int debian_logo_lines = 17;

static const char *ubuntu_logo[] = {
    "\033[38;5;202m            .-\\/+oossssoo+\\-.            \033[0m",
    "\033[38;5;202m         ´:+ssssssssssssssssss+:`         \033[0m",
    "\033[38;5;202m       -+ssssssssssssssssssyyssss+-       \033[0m",
    "\033[38;5;202m     .ossssssssssssssssss\033[38;5;254mdMMMNy\033[38;5;202msssso.     \033[0m",
    "\033[38;5;202m    /sssssssssss\033[38;5;254mhdmmNNmmyNMMMMh\033[38;5;202mssssss\\    \033[0m",
    "\033[38;5;202m   +sssssssss\033[38;5;254mhm\033[38;5;202myd\033[38;5;254mMMMMMMMNddddy\033[38;5;202mssssssss+   \033[0m",
    "\033[38;5;202m  /ssssssss\033[38;5;254mhNMMM\033[38;5;202myh\033[38;5;254mhyyyyhmNMMMNh\033[38;5;202mssssssss\\  \033[0m",
    "\033[38;5;202m .ssssssss\033[38;5;254mdMMMNh\033[38;5;202mssssssssss\033[38;5;254mhNMMMd\033[38;5;202mssssssss. \033[0m",
    "\033[38;5;202m +ssss\033[38;5;254mhhhyNMMNy\033[38;5;202mssssssssssss\033[38;5;254yNMMMy\033[38;5;202msssssss+ \033[0m",
    "\033[38;5;202m oss\033[38;5;254myNMMMNyMMh\033[38;5;202mssssssssssssss\033[38;5;254mhmmmh\033[38;5;202mssssssso \033[0m",
    "\033[38;5;202m oss\033[38;5;254myNMMMNyMMh\033[38;5;202msssssssssssssshmmmh\033[38;5;202mssssssso \033[0m",
    "\033[38;5;202m +ssss\033[38;5;254mhhhyNMMNy\033[38;5;202mssssssssssss\033[38;5;254myNMMMy\033[38;5;202msssssss+ \033[0m",
    "\033[38;5;202m .ssssssss\033[38;5;254mdMMMNh\033[38;5;202mssssssssss\033[38;5;254mhNMMMd\033[38;5;202mssssssss. \033[0m",
    "\033[38;5;202m  \\ssssssss\033[38;5;254mhNMMM\033[38;5;202myh\033[38;5;254mhyyyyhdNMMMNh\033[38;5;202mssssssss/  \033[0m",
    "\033[38;5;202m   +sssssssss\033[38;5;254mdm\033[38;5;202myd\033[38;5;254mMMMMMMMMddddy\033[38;5;202mssssssss+   \033[0m",
    "\033[38;5;202m    \\sssssssssss\033[38;5;254mhdmNNNNmyNMMMMh\033[38;5;202mssssss/    \033[0m",
    "\033[38;5;202m     .ossssssssssssssssss\033[38;5;254mdMMMNy\033[38;5;202msssso.     \033[0m",
    "\033[38;5;202m       -+sssssssssssssssss\033[38;5;254myyy\033[38;5;202mssss+-       \033[0m",
    "\033[38;5;202m         `:+ssssssssssssssssss+:`         \033[0m",
    "\033[38;5;202m             .-\\+oossssoo+/-.             \033[0m"
};
static const int ubuntu_logo_lines = 20;

static const char *fedora_logo[] = {
    "\033[38;5;27m             .',;::::;,'.\033[0m",
    "\033[38;5;27m          .';:cccccccccccc:;,.\033[0m",
    "\033[38;5;27m       .;cccccccccccccccccccccc;.\033[0m",
    "\033[38;5;27m     .:cccccccccccccccccccccc:.\033[0m",
    "\033[38;5;27m   .;ccccccccccccc;\033[38;5;254m.:dddl:.\033[38;5;27m;ccccccc;.\033[0m",
    "\033[38;5;27m  .:ccccccccccccc;\033[38;5;254mOWMKOOXMWd\033[38;5;27m;ccccccc:.\033[0m",
    "\033[38;5;27m .:ccccccccccccc;\033[38;5;254mKMMc\033[38;5;27m;cc;\033[38;5;254mxMMc\033[38;5;27m;ccccccc:.\033[0m",
    "\033[38;5;27m ,cccccccccccccc;\033[38;5;254mMMM.\033[38;5;27m;cc;\033[38;5;254m;WW:\033[38;5;27m;cccccccc,\033[0m",
    "\033[38;5;27m :cccccccccccccc;\033[38;5;254mMMM.\033[38;5;27m;cccccccccccccccc:\033[0m",
    "\033[38;5;27m :ccccccc;\033[38;5;254moxOOOo\033[38;5;27m;\033[38;5;254mMMM0OOk.\033[38;5;27m;cccccccccccc:\033[0m",
    "\033[38;5;27m cccccc;\033[38;5;254m0MMKxdd:\033[38;5;27m;\033[38;5;254mMMMkddc.\033[38;5;27m;cccccccccccc;\033[0m",
    "\033[38;5;27m ccccc;\033[38;5;254mXM0'\033[38;5;27m;cccc;\033[38;5;254mMMM.\033[38;5;27m;cccccccccccccccc'\033[0m",
    "\033[38;5;27m ccccc;\033[38;5;254mMMo\033[38;5;27m;ccccc;\033[38;5;254mMMW.\033[38;5;27m;ccccccccccccccc;\033[0m",
    "\033[38;5;27m ccccc;\033[38;5;254m0MNc.\033[38;5;27mccc\033[38;5;254m.xMMd\033[38;5;27m;ccccccccccccccc;\033[0m",
    "\033[38;5;27m cccccc;\033[38;5;254mdNMWXXXWM0:\033[38;5;27m;cccccccccccccc:,\033[0m",
    "\033[38;5;27m cccccccc;\033[38;5;254m.:odl:.\033[38;5;27m;cccccccccccccc:,.\033[0m",
    "\033[38;5;27m :cccccccccccccccccccccccccccc:'.\033[0m",
    "\033[38;5;27m .:cccccccccccccccccccccc:;,..\033[0m",
    "\033[38;5;27m   '::cccccccccccccc::;,.\033[0m"
};
static const int fedora_logo_lines = 19;

static const char *alpine_logo[] = {
    "\033[38;5;32m       .hddddddddddddddddddddddh.\033[0m",
    "\033[38;5;32m       :dddddddddddddddddddddddddd:\033[0m",
    "\033[38;5;32m      /dddddddddddddddddddddddddddd/\033[0m",
    "\033[38;5;32m     +dddddddddddddddddddddddddddddd+\033[0m",
    "\033[38;5;32m   `sdddddddddddddddddddddddddddddddds`\033[0m",
    "\033[38;5;32m  `ydddddddddddd++hdddddddddddddddddddy`\033[0m",
    "\033[38;5;32m .hddddddddddd+`  `+ddddh:-sdddddddddddh.\033[0m",
    "\033[38;5;32m hdddddddddd+`      `+y:    .sddddddddddh\033[0m",
    "\033[38;5;32m ddddddddh+`   `//`   `.`     -sddddddddd\033[0m",
    "\033[38;5;32m ddddddh+`   `/hddh/`   `:s-    -sddddddd\033[0m",
    "\033[38;5;32m ddddh+`   `/+/dddddh/`   `+s-    -sddddd\033[0m",
    "\033[38;5;32m ddd+`   `/o` :dddddddh/`   `oy-    .yddd\033[0m",
    "\033[38;5;32m hdddyo+ohddyosdddddddddho+oydddy++ohdddh\033[0m",
    "\033[38;5;32m .hddddddddddddddddddddddddddddddddddddh.\033[0m",
    "\033[38;5;32m  `yddddddddddddddddddddddddddddddddddy`\033[0m",
    "\033[38;5;32m   `sdddddddddddddddddddddddddddddddds`\033[0m",
    "\033[38;5;32m     +dddddddddddddddddddddddddddddd+\033[0m",
    "\033[38;5;32m      /dddddddddddddddddddddddddddd/\033[0m"
};
static const int alpine_logo_lines = 18;

static const char *kali_logo[] = {
    "\033[38;5;39m..............\033[0m",
    "\033[38;5;39m            ..,;:ccc,.\033[0m",
    "\033[38;5;39m          ......''';lxO.\033[0m",
    "\033[38;5;39m.....''''..........,:ld;\033[0m",
    "\033[38;5;39m            .';;;:::;,,.x,\033[0m",
    "\033[38;5;39m       ..'''.            0Xxoc:,.  ...\033[0m",
    "\033[38;5;39m   ....                ,ONkc;,;cokOdc',\033[0m",
    "\033[38;5;39m  .                   OMo           ':\033[38;5;254mdd\033[38;5;39mo.\033[0m",
    "\033[38;5;39m                     dMc               :OO;\033[0m",
    "\033[38;5;39m                     0M.                 .:o.\033[0m",
    "\033[38;5;39m                     ;Wd\033[0m",
    "\033[38;5;39m                      ;XO,\033[0m",
    "\033[38;5;39m                        ,d0Odlc;,..\033[0m",
    "\033[38;5;39m                            ..',;:cdOOd::,.\033[0m",
    "\033[38;5;39m                                     .:d;.\033[38;5;254m':;.\033[0m",
    "\033[38;5;39m                                        'd,  .'\033[0m",
    "\033[38;5;39m                                          ;l   ..\033[0m",
    "\033[38;5;39m                                           .o\033[0m",
    "\033[38;5;39m                                             c\033[0m",
    "\033[38;5;39m                                             .'\033[0m",
    "\033[38;5;39m                                              .\033[0m"
};
static const int kali_logo_lines = 21;

static const char *gentoo_logo[] = {
    "\033[38;5;99m         -/oyddmdhs+:.\033[0m",
    "\033[38;5;99m      -o\033[38;5;254mdNMMMMMMMMNNmhy+\033[38;5;99m-`\033[0m",
    "\033[38;5;99m    -y\033[38;5;254mNMMMMMMMMMMMNNNmmdhy\033[38;5;99m+-\033[0m",
    "\033[38;5;99m  `o\033[38;5;254mmMMMMMMMMMMMMNmdmmmmddhhy\033[38;5;99m/`\033[0m",
    "\033[38;5;99m  om\033[38;5;254mMMMMMMMMMMMN\033[38;5;99mhhyyyo\033[38;5;254mhmdddhhhd\033[38;5;99mo`\033[0m",
    "\033[38;5;99m .y\033[38;5;254mdMMMMMMMMMMd\033[38;5;99mhs++so/s\033[38;5;254mdmdddhhhhdm\033[38;5;99m+`\033[0m",
    "\033[38;5;99m  oy\033[38;5;254mhdmNMMMMMMMN\033[38;5;99mdyooy\033[38;5;254mdmddddhhhhyhN\033[38;5;99md.\033[0m",
    "\033[38;5;99m   :o\033[38;5;254myhhdNNMMMMMMMNNNmmdddhhhhhyym\033[38;5;99mMh\033[0m",
    "\033[38;5;99m     .:\033[38;5;254m+sydNMMMMMNNNmmmdddhhhhhhmM\033[38;5;99mmy\033[0m",
    "\033[38;5;99m        /m\033[38;5;254mMMMMMMNNNmmmdddhhhhhmMNh\033[38;5;99ms:\033[0m",
    "\033[38;5;99m     `o\033[38;5;254mNMMMMMMMNNNmmmddddhhdmMNhs\033[38;5;99m+`\033[0m",
    "\033[38;5;99m   `s\033[38;5;254mNMMMMMMMMNNNmmmdddddmNMmhs\033[38;5;99m/.\033[0m",
    "\033[38;5;99m  /N\033[38;5;254mMMMMMMMMNNNNmmmdddmNMNdso\033[38;5;99m:`\033[0m",
    "\033[38;5;99m +M\033[38;5;254mMMMMMMNNNNNmmmmdmNMNdso\033[38;5;99m/-\033[0m",
    "\033[38;5;99m yM\033[38;5;254mMNNNNNNmmmmmNNMmhs+/\033[38;5;99m-`\033[0m",
    "\033[38;5;99m /h\033[38;5;254mMMNNNNNNNNMNdhs++/\033[38;5;99m-`\033[0m",
    "\033[38;5;99m `/\033[38;5;254mohdmmddhys+++/:\033[38;5;99m.`\033[0m",
    "\033[38;5;99m   `-//////:--.\033[0m"
};
static const int gentoo_logo_lines = 18;

static const char *mint_logo[] = {
    "\033[38;5;10m             ...-:::::-...\033[0m",
    "\033[38;5;10m          .-MMMMMMMMMMMMMMM-.\033[0m",
    "\033[38;5;10m       .-MMMM\033[38;5;254m`..-:::::::-..`\033[38;5;10mMMMM-.\033[0m",
    "\033[38;5;10m     .:MMMM\033[38;5;254m.:MMMMMMMMMMMMMMM:.\033[38;5;10mMMMM:.\033[0m",
    "\033[38;5;10m    -MMM\033[38;5;254m-M---MMMMMMMMMMMMMMMMMMM.\033[38;5;10mMMM-\033[0m",
    "\033[38;5;10m  `:MMM\033[38;5;254m:MM`  :MMMM:....::-...-MMMM:\033[38;5;10mMMM:`\033[0m",
    "\033[38;5;10m  :MMM\033[38;5;254m:MMM`  :MM:`  ``    ``  `:MMM:\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m .MMM\033[38;5;254m.MMMM`  :MM.  -MM.  .MM-  `MMMM.\033[38;5;10mMMM.\033[0m",
    "\033[38;5;10m :MMM\033[38;5;254m:MMMM`  :MM.  -MM-  .MM:  `MMMM-\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m :MMM\033[38;5;254m:MMMM`  :MM.  -MM-  .MM:  `MMMM:\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m :MMM\033[38;5;254m:MMMM`  :MM.  -MM-  .MM:  `MMMM-\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m .MMM\033[38;5;254m.MMMM`  :MM:--:MM:--:MM:  `MMMM.\033[38;5;10mMMM.\033[0m",
    "\033[38;5;10m  :MMM\033[38;5;254m:MMM-  `-MMMMMMMMMMMM-`  -MMM-\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m   :MMM\033[38;5;254m:MMM:`                `:MMM:\033[38;5;10mMMM:\033[0m",
    "\033[38;5;10m    .MMM\033[38;5;254m.MMMM:--------------:MMMM.\033[38;5;10mMMM.\033[0m",
    "\033[38;5;10m      '-MMMM\033[38;5;254m.-MMMMMMMMMMMMMMM-.\033[38;5;10mMMMM-'\033[0m",
    "\033[38;5;10m        '.-MMMM\033[38;5;254m``--:::::--``\033[38;5;10mMMMM-.'\033[0m",
    "\033[38;5;10m            '-MMMMMMMMMMMMM-'\033[0m",
    "\033[38;5;10m               ``-:::::-``\033[0m"
};
static const int mint_logo_lines = 19;

static const char *nixos_logo[] = {
    "\033[38;5;33m          ▗▄▄▄       \033[38;5;110m▗▄▄▄▄    ▄▄▄▖\033[0m",
    "\033[38;5;33m          ▜███▙       \033[38;5;110m▜███▙  ▟███▛\033[0m",
    "\033[38;5;33m           ▜███▙       \033[38;5;110m▜███▙▟███▛\033[0m",
    "\033[38;5;33m            ▜███▙       \033[38;5;110m▜██████▛\033[0m",
    "\033[38;5;33m     ▟█████████████████▙ \033[38;5;110m▜████▛     \033[38;5;33m▟▙\033[0m",
    "\033[38;5;33m    ▟███████████████████▙ \033[38;5;110m▜███▙    \033[38;5;33m▟██▙\033[0m",
    "\033[38;5;110m           ▄▄▄▄▖           ▜███▙  \033[38;5;33m▟███▛\033[0m",
    "\033[38;5;110m          ▟███▛             ▜██▛ \033[38;5;33m▟███▛\033[0m",
    "\033[38;5;110m         ▟███▛               ▜▛ \033[38;5;33m▟███▛\033[0m",
    "\033[38;5;110m▟███████████▛                  \033[38;5;33m▟██████████▙\033[0m",
    "\033[38;5;110m▜██████████▛                  \033[38;5;33m▟███████████▛\033[0m",
    "\033[38;5;110m      ▟███▛ \033[38;5;33m▟▙               ▟███▛\033[0m",
    "\033[38;5;110m     ▟███▛ \033[38;5;33m▟██▙             ▟███▛\033[0m",
    "\033[38;5;110m    ▟███▛  \033[38;5;33m▜███▙           ▝▀▀▀▀\033[0m",
    "\033[38;5;110m    ▜██▛    \033[38;5;33m▜███▙ \033[38;5;110m▜██████████████████▛\033[0m",
    "\033[38;5;110m     ▜▛     \033[38;5;33m▟████▙ \033[38;5;110m▜████████████████▛\033[0m",
    "\033[38;5;33m           ▟██████▙       \033[38;5;110m▜███▙\033[0m",
    "\033[38;5;33m          ▟███▛▜███▙       \033[38;5;110m▜███▙\033[0m",
    "\033[38;5;33m         ▟███▛  ▜███▙       \033[38;5;110m▜███▙\033[0m",
    "\033[38;5;33m         ▝▀▀▀    ▀▀▀▀▘       \033[38;5;110m▀▀▘\033[0m"
};
static const int nixos_logo_lines = 20;

static const char *redhat_logo[] = {
    "\033[38;5;196m           .MMM..:MMMMMMM\033[0m",
    "\033[38;5;196m           MMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m           MMMMMMMMMMMMMMMMMMMM.\033[0m",
    "\033[38;5;196m          MMMMMMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m         ,MMMMMMMMMMMMMMMMMMMMMM:\033[0m",
    "\033[38;5;196m         MMMMMMMMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m   .MMMM'  MMMMMMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m  MMMMMM    `MMMMMMMMMMMMMMMMMMMM.\033[0m",
    "\033[38;5;196m MMMMMMMM      MMMMMMMMMMMMMMMMMM .\033[0m",
    "\033[38;5;196m MMMMMMMMM.       `MMMMMMMMMMMMM' MM.\033[0m",
    "\033[38;5;196m MMMMMMMMMMM.                     MMMM\033[0m",
    "\033[38;5;196m `MMMMMMMMMMMMM.                 ,MMMMM.\033[0m",
    "\033[38;5;196m  `MMMMMMMMMMMMMMMMM.          ,MMMMMMMM.\033[0m",
    "\033[38;5;196m     MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m       MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM:\033[0m",
    "\033[38;5;196m          MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\033[0m",
    "\033[38;5;196m             `MMMMMMMMMMMMMMMMMMMMMMMM:\033[0m",
    "\033[38;5;196m                 ``MMMMMMMMMMMMMMMMM'\033[0m"
};
static const int redhat_logo_lines = 18;

static const char *linux_logo[] = {
    "\033[38;5;220m     .---.       \033[0m",
    "\033[38;5;220m    | o o |      \033[0m",
    "\033[38;5;220m    | \\_/ |      \033[0m",
    "\033[38;5;220m   .-'___'-.     \033[0m",
    "\033[38;5;220m  /  `   `  \\    \033[0m",
    "\033[38;5;220m |   |   |   |   \033[0m",
    "\033[38;5;220m  \\__\\___/__/    \033[0m"
};
static const int linux_logo_lines = 7;

static const char *macos_logo[] = {
    "\033[38;5;34m                    c.'\033[0m",
    "\033[38;5;34m                 ,xNMM.\033[0m",
    "\033[38;5;34m               .OMMMMo\033[0m",
    "\033[38;5;34m               lMM\"\033[0m",
    "\033[38;5;34m     .;loddo:.  .olloddol;.\033[0m",
    "\033[38;5;34m   cKMMMMMMMMMMNWMMMMMMMMMM0:\033[0m",
    "\033[38;5;226m .KMMMMMMMMMMMMMMMMMMMMMMMWd.\033[0m",
    "\033[38;5;226m XMMMMMMMMMMMMMMMMMMMMMMMX.\033[0m",
    "\033[38;5;208m;MMMMMMMMMMMMMMMMMMMMMMMM:\033[0m",
    "\033[38;5;208m:MMMMMMMMMMMMMMMMMMMMMMMM:\033[0m",
    "\033[38;5;196m.MMMMMMMMMMMMMMMMMMMMMMMMX.\033[0m",
    "\033[38;5;196m kMMMMMMMMMMMMMMMMMMMMMMMMWd.\033[0m",
    " \033[38;5;93m'XMMMMMMMMMMMMMMMMMMMMMMMMMMk\033[0m",
    "\033[38;5;93m  'XMMMMMMMMMMMMMMMMMMMMMMMMK.\033[0m",
    "    \033[38;5;33mkMMMMMMMMMMMMMMMMMMMMMMd\033[0m",
    "\033[38;5;33m     ;KMMMMMMMWXXWMMMMMMMk.\033[0m",
    "\033[38;5;33m       \"cooc*\"    \"*coo'\"\033[0m",
};
static const int macos_logo_lines = 17;

const char **pick_logo(const char *lower, int *lines_out) {
#define LOGO(arr) do { *lines_out = arr##_lines; return arr; } while(0)
    if (strstr(lower, "arch"))   LOGO(arch_logo);
    if (strstr(lower, "debian")) LOGO(debian_logo);
    if (strstr(lower, "ubuntu")) LOGO(ubuntu_logo);
    if (strstr(lower, "fedora")) LOGO(fedora_logo);
    if (strstr(lower, "alpine")) LOGO(alpine_logo);
    if (strstr(lower, "kali"))   LOGO(kali_logo);
    if (strstr(lower, "gentoo")) LOGO(gentoo_logo);
    if (strstr(lower, "mint"))   LOGO(mint_logo);
    if (strstr(lower, "nixos"))  LOGO(nixos_logo);
    if (strstr(lower, "red hat") || strstr(lower, "redhat") ||
        strstr(lower, "rhel")    || strstr(lower, "centos")) LOGO(redhat_logo);
    if (strstr(lower, "linux"))  LOGO(linux_logo);
    if (strstr(lower, "macos") || strstr(lower, "darwin") || strstr(lower, "apple")) LOGO(macos_logo);
    if (strstr(lower, "default") || strstr(lower, "fasterfetch")) LOGO(default_ascii);
#undef LOGO
    return NULL;
}
