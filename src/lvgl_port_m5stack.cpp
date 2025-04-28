#include "lvgl_port_m5stack.hpp"

#ifdef ESP_PLATFORM
#include <M5Dial.h>
#endif

#if defined(ESP_PLATFORM)
static SemaphoreHandle_t xGuiSemaphore;
#elif !defined(ESP_PLATFORM) && (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
static SDL_mutex *xGuiMutex;
#endif

#ifndef LV_BUFFER_LINE
#define LV_BUFFER_LINE 120
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_PLATFORM) && defined(ESP_PLATFORM)
static void lvgl_tick_timer(void *arg) {
    (void)arg;
    lv_tick_inc(10);
}
static void lvgl_rtos_task(void *pvParameter) {
    (void)pvParameter;
    while (1) {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
#elif !defined(ESP_PLATFORM) && (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
static uint32_t lvgl_tick_timer(uint32_t interval, void *param) {
    (void)interval;
    (void)param;
    lv_tick_inc(10);
    return 10;
}

[[noreturn]] static int lvgl_sdl_thread(void *data) {
    (void)data;
    while (true) {
        if (SDL_LockMutex(xGuiMutex) == 0) {
            lv_timer_handler();
            SDL_UnlockMutex(xGuiMutex);
        }
        SDL_Delay(10);
    }
}
#endif

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    M5GFX &gfx = *static_cast<M5GFX *>(lv_display_get_driver_data(disp));

    const int32_t w = (area->x2 - area->x1 + 1);
    const int32_t h = (area->y2 - area->y1 + 1);

    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.writePixels(reinterpret_cast<lgfx::rgb565_t *>(px_map), w * h);
    gfx.endWrite();

    lv_display_flush_ready(disp);
}

static void lvgl_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    M5GFX &gfx = *static_cast<M5GFX *>(lv_indev_get_driver_data(indev));
    uint16_t touchX, touchY;

    bool touched = gfx.getTouch(&touchX, &touchY);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state   = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}
static void lvgl_read_encoder_cb(lv_indev_t *indev, lv_indev_data_t *data) {
#ifdef ESP_PLATFORM
    M5Dial.update();
    static int32_t oldPosition = 0;

    const int32_t newPosition = M5Dial.Encoder.read();
    const int32_t diff        = newPosition - oldPosition;
    data->enc_diff            = diff;
    oldPosition               = newPosition;
    data->state               = LV_INDEV_STATE_RELEASED;
    data->key                 = diff > 0 ? LV_KEY_RIGHT : diff < 0 ? LV_KEY_LEFT : 0;
    // data->state         = M5.BtnA.isPressed() ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
#else
    // Implement left and right arrows for encoder
    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT]) {
        data->key      = LV_KEY_RIGHT;
        data->enc_diff = 1;
    } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LEFT]) {
        data->key      = LV_KEY_LEFT;
        data->enc_diff = -1;
    } else {
        data->key      = 0;
        data->enc_diff = 0;
    }

    data->state = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RETURN] ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
#endif
}
lvgl_m5_dial_t *lvgl_port_init(M5GFX &gfx) {
    lv_init();

    static lv_display_t *disp = lv_display_create(gfx.width(), gfx.height());
    if (disp == nullptr) {
        LV_LOG_ERROR("lv_display_create failed");
        return nullptr;
    }

    lv_display_set_driver_data(disp, &gfx);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);

#if defined(ESP_PLATFORM)
#if defined(BOARD_HAS_PSRAM)
    static uint8_t *buf1 = (uint8_t *)heap_caps_malloc(gfx.width() * LV_BUFFER_LINE, MALLOC_CAP_SPIRAM);
    static uint8_t *buf2 = (uint8_t *)heap_caps_malloc(gfx.width() * LV_BUFFER_LINE, MALLOC_CAP_SPIRAM);
    lv_display_set_buffers(disp, (void *)buf1, (void *)buf2, gfx.width() * LV_BUFFER_LINE,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
#else
    static auto *buf1 = static_cast<uint8_t *>(malloc(gfx.width() * LV_BUFFER_LINE));
    lv_display_set_buffers(disp, buf1, nullptr, gfx.width() * LV_BUFFER_LINE, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif
#elif !defined(ARDUINO) && (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
    static auto *buf1 = static_cast<uint8_t *>(malloc(gfx.width() * LV_BUFFER_LINE * 2));
    static auto *buf2 = static_cast<uint8_t *>(malloc(gfx.width() * LV_BUFFER_LINE * 2));
    lv_display_set_buffers(disp, (void *)buf1, (void *)buf2, gfx.width() * LV_BUFFER_LINE * 2,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    static lv_indev_t *indev = lv_indev_create();
    LV_ASSERT_MALLOC(indev);
    if (indev == nullptr) {
        LV_LOG_ERROR("lv_indev_create failed");
        return nullptr;
    }
    lv_indev_set_driver_data(indev, &gfx);
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_read_cb);
    lv_indev_set_display(indev, disp);

    static lv_indev_t *indev_encoder = lv_indev_create();
    LV_ASSERT_MALLOC(indev_encoder);
    if (indev_encoder == nullptr) {
        LV_LOG_ERROR("lv_indev_create failed");
        return nullptr;
    }
    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev_encoder, lvgl_read_encoder_cb);
    lv_indev_set_display(indev_encoder, disp);

#if defined(ESP_PLATFORM)
    xGuiSemaphore                                     = xSemaphoreCreateMutex();
    const esp_timer_create_args_t periodic_timer_args = {.callback = &lvgl_tick_timer, .name = "lvgl_tick_timer"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));
    xTaskCreate(lvgl_rtos_task, "lvgl_rtos_task", 8 * 1024, NULL, 1, NULL);
#elif !defined(ESP_PLATFORM) && (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
    xGuiMutex = SDL_CreateMutex();
    SDL_AddTimer(10, lvgl_tick_timer, nullptr);
    SDL_CreateThread(lvgl_sdl_thread, "lvgl_sdl_thread", nullptr);
#endif

    return new lvgl_m5_dial_t{disp, indev, indev_encoder};
}

bool lvgl_port_lock(void) {
#if defined(ESP_PLATFORM)
    return xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdTRUE ? true : false;
#else
    return SDL_LockMutex(xGuiMutex) == 0 ? true : false;
#endif
}

void lvgl_port_unlock(void) {
#if defined(ESP_PLATFORM)
    xSemaphoreGive(xGuiSemaphore);
#elif (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
    SDL_UnlockMutex(xGuiMutex);
#endif
}

#ifdef __cplusplus
}
#endif
