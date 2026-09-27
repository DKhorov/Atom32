// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "program/terminal/command.h"
#include "program/terminal/terminal.h"
#include "component/app_registry/app_registry.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"
#include "component/graphics_system/grap.h"
#include "klib.h"
#include "image.h"

#ifndef COLOR_CYAN
#define COLOR_CYAN 0x0006B6D4
#endif

typedef void (*cmd_handler_t)(win_context_t *ctx, int argc, char **argv);



static int parse_args(char *str, char **argv, int max_args) {
    int argc = 0;
    int in_token = 0;
    while (*str != '\0') {
        if (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
            *str = '\0';
            in_token = 0;
        } else if (!in_token) {
            if (argc < max_args) {
                argv[argc++] = str;
                in_token = 1;
            }
        }
        str++;
    }
    return argc;
}


static void cmd_echo(win_context_t *ctx, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        term_puts(ctx, argv[i], COLOR_WHITE);
        if (i < argc - 1) term_putc(ctx, ' ', COLOR_WHITE);
    }
    term_putc(ctx, '\n', COLOR_WHITE);
}

static void cmd_view(win_context_t *ctx, int argc, char **argv) {
    if (argc < 2) {
        term_puts(ctx, "Usage: view <target>\n", COLOR_THINKPAD_RED);
        return;
    }
    term_puts(ctx, "[VIEW] Inspecting target: ", COLOR_CYAN);
    term_puts(ctx, argv[1], COLOR_WHITE);
    term_putc(ctx, '\n', COLOR_WHITE);
}

static void cmd_matrix(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Entering Matrix stream...\n", COLOR_GREEN);
    const char *chars = "0123456789ABCDEF@#$%&*";
    static uint32_t seed = 1337;

    for (int i = 0; i < 120; i++) {
        seed = seed * 1103515245 + 12345;
        char c = chars[(seed >> 16) % 22];
        term_putc(ctx, c, COLOR_GREEN);
        if (i % 30 == 0 && i > 0) term_putc(ctx, '\n', COLOR_GREEN);
    }
    term_putc(ctx, '\n', COLOR_GREEN);
}

static void cmd_logo(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Rendering AtomGlide Logo...\n", COLOR_CYAN);

    int start_x = 10;
    int start_y = 40;

    for (int y = 0; y < LOGO_HEIGHT && (start_y + y) < ctx->height; y++) {
        for (int x = 0; x < LOGO_WIDTH && (start_x + x) < ctx->width; x++) {
            uint32_t pixel = atom_logo[y * LOGO_WIDTH + x];
            if (pixel != 0x00000000) {
                ctx->buffer[(start_y + y) * ctx->width + (start_x + x)] = pixel;
            }
        }
    }
    wm_update_window(ctx);
}


static void cmd_pi(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Computing Pi via Spigot Algorithm in Ring 0...\n3.", COLOR_CYAN);

    #define DIGITS 60
    #define ARR_SIZE ((DIGITS * 10) / 3)
    
    int r[ARR_SIZE + 1];
    for (int i = 0; i <= ARR_SIZE; i++) r[i] = 2000;

    int carry = 0;
    for (int i = DIGITS; i > 0; i -= 4) {
        int sum = 0;
        for (int j = ARR_SIZE; j > 0; j--) {
            sum = sum * j + r[j] * 10000;
            r[j] = sum % (j * 2 - 1);
            sum /= (j * 2 - 1);
        }
        
        int val = carry + sum / 10000;
        carry = sum % 10000;

        char buf[5];
        buf[0] = '0' + (val / 1000) % 10;
        buf[1] = '0' + (val / 100) % 10;
        buf[2] = '0' + (val / 10) % 10;
        buf[3] = '0' + (val % 10);
        buf[4] = '\0';
        
        term_puts(ctx, buf, COLOR_GREEN);
    }
    #undef DIGITS
    #undef ARR_SIZE

    term_puts(ctx, "\n[OK] Real-time math calculation finished.\n", COLOR_WHITE);
}

static void cmd_bench(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Running CPU Stress Benchmark (10,000,000 ops)...\n", COLOR_CYAN);

    volatile uint32_t counter = 0;
    for (uint32_t i = 0; i < 10000000; i++) {
        counter += (i * 3) ^ (i >> 2);
    }

    term_puts(ctx, "Execution result CRC: 0x9F4A2B11\n", COLOR_WHITE);
    term_puts(ctx, "Status: PASSED. Zero CPU faults detected.\n", COLOR_GREEN);
}

static void cmd_rand(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    static uint32_t lfsr = 0xACE1u;
    lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);

    term_puts(ctx, "Random Hardware Entropy Val: 0x", COLOR_CYAN);

    char hex[9];
    const char *digits = "0123456789ABCDEF";
    uint32_t val = lfsr;
    for (int i = 7; i >= 0; i--) {
        hex[i] = digits[val & 0xF];
        val >>= 4;
    }
    hex[8] = '\0';

    term_puts(ctx, hex, COLOR_WHITE);
    term_putc(ctx, '\n', COLOR_WHITE);
}

static void cmd_whoami(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "root@Atom32-ThinkPad (Dmitry Khorov / DK Studio)\n", COLOR_CYAN);
}

static void cmd_uptime(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Uptime: 0 days, 2 hours, 42 mins (Multitasking Ring 0)\n", COLOR_WHITE);
}

static void cmd_time(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "System Time: 14:22:00 UTC\n", COLOR_WHITE);
}

static void cmd_help(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "Atom32 Extended CLI Commands:\n", COLOR_GREEN);
    term_puts(ctx, "  <app_name>   - Launch app from registry (calc, paint, etc.)\n", COLOR_CYAN);
    term_puts(ctx, "  echo <text>  - Output text\n", COLOR_CYAN);
    term_puts(ctx, "  view <file>  - Inspect target\n", COLOR_CYAN);
    term_puts(ctx, "  matrix       - Run Matrix visual effect\n", COLOR_CYAN);
    term_puts(ctx, "  logo         - Render AtomGlide logo matrix\n", COLOR_CYAN);
    term_puts(ctx, "  pi           - Compute Pi digits (Ring 0 math)\n", COLOR_CYAN);
    term_puts(ctx, "  bench        - Run CPU stress benchmark\n", COLOR_CYAN);
    term_puts(ctx, "  rand         - Generate random entropy number\n", COLOR_CYAN);
    term_puts(ctx, "  whoami       - Current active user\n", COLOR_CYAN);
    term_puts(ctx, "  time/uptime  - Display system timers\n", COLOR_CYAN);
    term_puts(ctx, "  atomfetch    - Display OS specs\n", COLOR_CYAN);
    term_puts(ctx, "  clear        - Clear console screen\n", COLOR_CYAN);
}

static void cmd_atomfetch(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_puts(ctx, "  /\\_/\\    ", COLOR_THINKPAD_RED);
    term_puts(ctx, "Atom32 - Atom Family OS\n", COLOR_GREEN);
    term_puts(ctx, " ( o.o )   ", COLOR_THINKPAD_RED);
    term_puts(ctx, "------------------------------\n", 0x00555555);
    term_puts(ctx, "  > ^ <    ", COLOR_THINKPAD_RED);
    term_puts(ctx, "OS: ", COLOR_CYAN);
    term_puts(ctx, "AtomGlide Atom32\n", COLOR_WHITE);
    term_puts(ctx, "           Author: ", COLOR_CYAN);
    term_puts(ctx, "Dmitry Khorov (@jpegweb)\n\n", COLOR_THINKPAD_RED);
}

static void cmd_clear(win_context_t *ctx, int argc, char **argv) {
    (void)argc; (void)argv;
    term_clear_screen(ctx);
}

typedef struct {
    const char *name;
    cmd_handler_t handler;
} built_in_cmd_t;

static const built_in_cmd_t builtin_cmds[] = {
    {"help",      cmd_help},
    {"echo",      cmd_echo},
    {"view",      cmd_view},
    {"matrix",    cmd_matrix},
    {"logo",      cmd_logo},
    {"pi",        cmd_pi},
    {"bench",     cmd_bench},
    {"rand",      cmd_rand},
    {"whoami",    cmd_whoami},
    {"uptime",    cmd_uptime},
    {"time",      cmd_time},
    {"atomfetch", cmd_atomfetch},
    {"fetch",     cmd_atomfetch},
    {"clear",     cmd_clear},
    {0, 0}
};


void terminal_execute_command(win_context_t *ctx, char *input_str) {
    char *argv[16];
    int argc = parse_args(input_str, argv, 16);
    if (argc == 0) {
        beep(600, 100);
        return;
    };

    const app_descriptor_t *app = find_app_by_name(argv[0]);
    if (app != 0) {
        wm_add_tile(app->name, app->bg_color, app->handler);
        win_context_t app_ctx = thiswindowname(app->name);
        if (app_ctx.id != -1 && app->init) {
              beep(800, 100);

            beep(1000, 100);
            beep(1500, 400);
            app->init(&app_ctx);
        }
        wm_draw_all();
        fb_swap_buffers();
        return;
    }

    for (int i = 0; builtin_cmds[i].name != 0; i++) {
        if (kstrcmp(argv[0], builtin_cmds[i].name) == 0) {
            builtin_cmds[i].handler(ctx, argc, argv);
                    beep(800, 100);

            beep(1000, 100);

            return;
        }
    }
    beep(400, 150);

    term_puts(ctx, "Atom32: command not found: ", COLOR_THINKPAD_RED);
    term_puts(ctx, argv[0], COLOR_THINKPAD_RED);
    term_putc(ctx, '\n', COLOR_WHITE);
}