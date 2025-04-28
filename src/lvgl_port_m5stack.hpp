#pragma once

#if defined(ARDUINO)
#endif
#include <M5GFX.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _lvgl_m5_dial {
    lv_display_t *display;
    lv_indev_t *touch;
    lv_indev_t *encoder;
} lvgl_m5_dial_t;

lvgl_m5_dial_t *lvgl_port_init(M5GFX &gfx);
bool lvgl_port_lock(void);
void lvgl_port_unlock(void);

#ifdef __cplusplus
}
#endif
