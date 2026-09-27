#ifndef BLASTER_H
#define BLASTER_H

#include "component/windows_system/win.h"

// Инициализация графического синтезатора
void blaster_init(win_context_t *ctx);

// Фоновый просчет физики волн и анимации
void blaster_tick(void);

// Обработчик клавиш пианино и кнопок интерфейса
void blaster_event_handler(win_context_t *ctx, wm_event_t *evt);

#endif