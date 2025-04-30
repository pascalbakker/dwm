/* See LICENSE file for copyright and license details. */

#include "fibonacci.c"
#include "movestack.c"
#include <X11/XF86keysym.h>

#define TERMCLASS "St"
#define TERMCLASS_COLOR "st-256color"
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                                          \
    {MODKEY, KEY, view, {.ui = 1 << TAG}},                                                         \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},                                 \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                                          \
        {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                                                 \
    {                                                                                              \
        .v = (const char*[]) { "/bin/sh", "-c", cmd, NULL }                                        \
    }

/* appearance */
static const unsigned int borderpx = 1; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const int swallowfloating = 0;   /* 1 means swallow floating windows by default */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
static const char* fonts[] = {"monospace:size=10"};
static const char dmenufont[] = "monospace:size=10";
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_cyan[] = "#005577";
static const char col_ivyred[] = "#770000";
static const char* colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_gray3, col_gray1, col_gray2},
    [SchemeSel] = {col_gray4, col_cyan, col_ivyred},
};

static const char* const autostart[] = {
    "st",
    NULL,
    "sh",
    "-c",
    "~/scripts/dwm-status-script.sh",
    NULL,
    "setxkbmap",
    "-option",
    "caps:super",
    NULL /* terminate */
};

/* tagging */
static const char* tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance,
     *class WM_NAME(STRING) = title
     */
    /* class            instance        title        tags_mask    isfloating
       isterminal    noswallow    monitor */
    {"Gimp", NULL, NULL, 1 << 8, 0, 0, 0, -1},
    {TERMCLASS, NULL, NULL, 0, 0, 1, 0, -1},
    {TERMCLASS_COLOR, NULL, NULL, 0, 0, 1, 0, -1},
    {NULL, NULL, "Event Tester", 0, 0, 0, 1, -1},
    {TERMCLASS, "floatterm", NULL, 0, 1, 1, 0, -1},
    {TERMCLASS, "bg", NULL, 1 << 7, 0, 1, 0, -1},
};

/* layout(s) */
static const float mfact = 0.55;     /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;        /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[T]", tile},    // Default master-stack layout
    {"[F]", NULL},    // Floating windows
    {"[M]", monocle}, // Fullscreen monocle
    {"[S]", spiral},  // Fibonacci spiral
    {"[D]", dwindle}, // Shrinking Fibonacci
};

/* key definitions */
/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */

static const char* dmenucmd[] = {"dmenu_run",
                                 "-m",
                                 dmenumon,
                                 "-fn",
                                 dmenufont,
                                 "-nb",
                                 col_gray1,
                                 "-nf",
                                 col_gray3,
                                 "-sb",
                                 col_cyan,
                                 "-sf",
                                 col_gray4,
                                 NULL};

/* PROGRAMS */
static const char* termcmd[] = {"st", NULL};

static const Key keys[] = {
    /* modifier                     key        function        argument */
    // Volume commands
    {0, XF86XK_AudioLowerVolume, spawn, SHCMD("amixer set Master 5%-")},
    {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("amixer set Master 5%+")},
    {0, XF86XK_AudioMute, spawn, SHCMD("amixer set Master toggle")},
    // Spawn commands
    {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    // Toggle commands
    {MODKEY, XK_b, togglebar, {0}},
    // Stack layout control commands
    {MODKEY, XK_l, focusstack, {.i = +1}},
    {MODKEY, XK_h, focusstack, {.i = -1}},
    {MODKEY, XK_k, movestack, {.i = +1}},
    {MODKEY, XK_j, movestack, {.i = -1}},
    // Change window size hotkeys
    {MODKEY | ControlMask, XK_minus, setmfact, {.f = -0.05}},
    {MODKEY | ControlMask, XK_equal, setmfact, {.f = +0.05}},
    // Layout hotkeys
    {MODKEY | ControlMask, XK_comma, cyclelayout, {.i = -1}},
    {MODKEY | ControlMask, XK_period, cyclelayout, {.i = +1}},
    {MODKEY, XK_f, togglefullscr, {0}},                  // fullscreen
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}}, // floating
    {MODKEY, XK_Tab, cycleview, {0}},                    // cycle
    // Kill commands
    {MODKEY, XK_q, killclient, {0}},
    {MODKEY | ShiftMask, XK_q, quit, {0}},
    {MODKEY, XK_space, setlayout, {0}},
    // Tag commands
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    // Tag keys
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3) TAGKEYS(XK_5, 4)
        TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_9, 8)};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
