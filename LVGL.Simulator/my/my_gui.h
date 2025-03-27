#include <lvgl/src/misc/lv_timer.h>
#ifndef _MY_GUI_H_
#define _MY_GUI_H_
#include "my/my_gui.h"
void create_colorful_clock();


static void update_time(lv_timer_t* timer);
static void update_color(lv_timer_t* timer);
#endif
