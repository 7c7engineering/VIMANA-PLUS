#pragma once
#include <stdint.h>
#include "driver/ledc.h"
#include "esp_err.h"

typedef struct led {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_t;

// list some colors
#define LED_RED     (led_t){.red = 255, .green = 0, .blue = 0}
#define LED_GREEN   (led_t){.red = 0, .green = 255, .blue = 0}
#define LED_BLUE    (led_t){.red = 0, .green = 0, .blue = 255}
#define LED_YELLOW  (led_t){.red = 255, .green = 255, .blue = 0}
#define LED_PURPLE  (led_t){.red = 255, .green = 0, .blue = 255}
#define LED_CYAN    (led_t){.red = 0, .green = 255, .blue = 255}
#define LED_WHITE   (led_t){.red = 255, .green = 255, .blue = 255}
#define LED_BLACK   (led_t){.red = 0, .green = 0, .blue = 0}



esp_err_t init_leds(void);
esp_err_t set_leds(led_t led1, led_t led2);