// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------




#include "program/pong/pong.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/grap.h"
#include "component/app_registry/app_registry.h"
#include "klib.h"

#define PADDLE_W        12
#define PADDLE_H        70
#define BALL_SIZE       10

typedef struct {
    int paddle_y;
    int ball_x;
    int ball_y;
    int ball_vx;
    int ball_vy;
    int hits;
} pong_game_t;

static pong_game_t game;

static void pong_draw_rect(win_context_t *ctx, int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        if (py < 0 || py >= ctx->height) continue;
        for (int px = x; px < x + w; px++) {
            if (px < 0 || px >= ctx->width) continue;
            ctx->buffer[py * ctx->width + px] = color;
        }
    }
}

static void pong_render(win_context_t *ctx) {
    for (int i = 0; i < ctx->width * ctx->height; i++) {
        ctx->buffer[i] = COLOR_BLACK;
    }

    pong_draw_rect(ctx, ctx->width - 12, 0, 12, ctx->height, 0x00777777);

    pong_draw_rect(ctx, 15, game.paddle_y, PADDLE_W, PADDLE_H, COLOR_WHITE);

    pong_draw_rect(ctx, game.ball_x, game.ball_y, BALL_SIZE, BALL_SIZE, COLOR_GREEN);

    char score_str[16];
    itoa(game.hits, score_str);
    draw_string_canvas(ctx, 20, 20, score_str, COLOR_WHITE);
}

static void pong_reset_ball(win_context_t *ctx) {
    game.ball_x = ctx->width / 2;
    game.ball_y = ctx->height / 2;
    game.ball_vx = 3;
    game.ball_vy = 2;
}

void pong_init(win_context_t *ctx) {
    game.paddle_y = (ctx->height / 2) - (PADDLE_H / 2);
    game.hits = 0;
    pong_reset_ball(ctx);
}

static void pong_update(win_context_t *ctx) {
    game.ball_x += game.ball_vx;
    game.ball_y += game.ball_vy;

    if (game.ball_y <= 0) {
        game.ball_y = 0;
        game.ball_vy = -game.ball_vy;
    } else if (game.ball_y + BALL_SIZE >= ctx->height) {
        game.ball_y = ctx->height - BALL_SIZE;
        game.ball_vy = -game.ball_vy;
    }

    if (game.ball_x + BALL_SIZE >= ctx->width - 12) {
        game.ball_x = ctx->width - 12 - BALL_SIZE;
        game.ball_vx = -game.ball_vx;
    }

    if (game.ball_x <= 15 + PADDLE_W && game.ball_x >= 10) {
        if (game.ball_y + BALL_SIZE >= game.paddle_y && game.ball_y <= game.paddle_y + PADDLE_H) {
            game.ball_x = 15 + PADDLE_W;
            game.ball_vx = -game.ball_vx;
            game.hits++; 
        }
    }

    if (game.ball_x < 0) {
        game.hits = 0;
        pong_reset_ball(ctx);
    }
}

void pong_tick(void) {
    win_context_t ctx = thiswindowname("PONG");
    if (ctx.id != -1 && ctx.buffer) {
        pong_update(&ctx);
        pong_render(&ctx);
        wm_update_window(&ctx);
    }
}

void pong_event_handler(win_context_t *ctx, wm_event_t *evt) {
    if (evt->type == WM_EVENT_KEY) {
        uint32_t k = evt->param1;
        if (k == 'w' || k == 'W' || k == 0x48) {
            game.paddle_y -= 16;
            if (game.paddle_y < 0) game.paddle_y = 0;
        } else if (k == 's' || k == 'S' || k == 0x50) {
            game.paddle_y += 16;
            if (game.paddle_y + PADDLE_H > ctx->height) {
                game.paddle_y = ctx->height - PADDLE_H;
            }
        }
    }

    if (ctx && ctx->id != -1) {
        pong_render(ctx);
    }
}