#include <M5GFX.h>
#if defined(SDL_h_)

void setup();
void loop();

__attribute__((weak)) int user_func(bool *running) {
    setup();
    do {
        loop();
    } while (*running);
    return 0;
}

int main(int, char **) {
    // Simulate M5Dial encoder
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_LEFT, 39);
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_DOWN, 38);
    lgfx::Panel_sdl::addKeyCodeMapping(SDLK_SPACE, 32);

    // You can specify the time in milliseconds to perform slow execution that ensures screen updates.
    // The second argument is effective for step execution with breakpoints.
    return lgfx::Panel_sdl::main(user_func, 128);
}

#endif
