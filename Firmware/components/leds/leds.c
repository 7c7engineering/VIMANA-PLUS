#include <stdio.h>
#include "leds.h"

const int GPIO_LED1_RED = 17;
const int GPIO_LED1_GREEN = 16;
const int GPIO_LED1_BLUE = 15;
const int GPIO_LED2_RED = 14;
const int GPIO_LED2_GREEN = 8;
const int GPIO_LED2_BLUE = 9;


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 10 bits

const ledc_channel_t LEDC_CHANNEL_RED1 = LEDC_CHANNEL_0;
const ledc_channel_t LEDC_CHANNEL_GREEN1 = LEDC_CHANNEL_1;
const ledc_channel_t LEDC_CHANNEL_BLUE1 = LEDC_CHANNEL_2;
const ledc_channel_t LEDC_CHANNEL_RED2 = LEDC_CHANNEL_3;
const ledc_channel_t LEDC_CHANNEL_GREEN2 = LEDC_CHANNEL_4;
const ledc_channel_t LEDC_CHANNEL_BLUE2 = LEDC_CHANNEL_5;

esp_err_t led_hsl_to_rgb(led_hsl_t led, led_t * led_out){
    // Convert HSL to RGB
    // https://www.rapidtables.com/convert/color/hsl-to-rgb.html
    float c = (1 - fabs(2 * led.lum - 1)) * led.sat;
    float x = c * (1 - fabs(fmod(led.hue / 60, 2) - 1));
    float m = led.lum - c / 2;
    float r = 0;
    float g = 0;
    float b = 0;
    if (led.hue >= 0 && led.hue < 60){
        r = c;
        g = x;
        b = 0;
    } else if (led.hue >= 60 && led.hue < 120){
        r = x;
        g = c;
        b = 0;
    } else if (led.hue >= 120 && led.hue < 180){
        r = 0;
        g = c;
        b = x;
    } else if (led.hue >= 180 && led.hue < 240){
        r = 0;
        g = x;
        b = c;
    } else if (led.hue >= 240 && led.hue < 300){
        r = x;
        g = 0;
        b = c;
    } else if (led.hue >= 300 && led.hue < 360){
        r = c;
        g = 0;
        b = x;
    }
    led_out->red = (uint8_t)((r + m) * 255);
    led_out->green = (uint8_t)((g + m) * 255);
    led_out->blue = (uint8_t)((b + m) * 255);
    return ESP_OK;
}

esp_err_t led_rgb_to_hsl(led_t led, led_hsl_t * led_out){
    // Convert RGB to HSL
    // https://www.rapidtables.com/convert/color/rgb-to-hsl.html
    float r = (float)led.red / 255.0;
    float g = (float)led.green / 255.0;
    float b = (float)led.blue / 255.0;
    float cmax = r;
    if (g > cmax) cmax = g;
    if (b > cmax) cmax = b;
    float cmin = r;
    if (g < cmin) cmin = g;
    if (b < cmin) cmin = b;
    float delta = cmax - cmin;
    float h = 0;
    if (delta == 0){
        h = 0;
    } else if (cmax == r){
        h = 60 * (((g - b) / delta) + 0);
    } else if (cmax == g){
        h = 60 * (((b - r) / delta) + 2);
    } else if (cmax == b){
        h = 60 * (((r - g) / delta) + 4);
    }
    if (h < 0) h += 360;
    float l = (cmax + cmin) / 2;
    float s = 0;
    if (delta == 0){
        s = 0;
    } else {
        s = delta / (1 - fabs(2*l - 1));
    }
    led_out->hue = h;
    led_out->sat = s;
    led_out->lum = l;
    return ESP_OK;
}

esp_err_t init_leds(void){
    // Prepare and then apply the LEDC PWM timer configuration
        ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configurations
    // The LEDS are active low, so the led channel inverted bit needs to be set
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_RED1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_LED1_RED,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.gpio_num = GPIO_LED1_GREEN;
    ledc_channel.channel = LEDC_CHANNEL_GREEN1;
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.gpio_num = GPIO_LED1_BLUE;
    ledc_channel.channel = LEDC_CHANNEL_BLUE1;
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.gpio_num = GPIO_LED2_RED;
    ledc_channel.channel = LEDC_CHANNEL_RED2;
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.gpio_num = GPIO_LED2_GREEN;
    ledc_channel.channel = LEDC_CHANNEL_GREEN2;
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.gpio_num = GPIO_LED2_BLUE;
    ledc_channel.channel = LEDC_CHANNEL_BLUE2;
    ledc_channel.flags.output_invert = 1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    return ESP_OK;
}

esp_err_t set_leds(led_t led1, led_t led2){
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED1, led1.red));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN1, led1.green));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE1, led1.blue));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED2, led2.red));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN2, led2.green));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE2, led2.blue));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED1));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN1));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE1));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED2));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN2));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE2));
    return ESP_OK;
}

esp_err_t set_leds_hsl(led_hsl_t led1, led_hsl_t led2){
    led_t led1_rgb;
    led_t led2_rgb;
    ESP_ERROR_CHECK(led_hsl_to_rgb(led1, &led1_rgb));
    ESP_ERROR_CHECK(led_hsl_to_rgb(led2, &led2_rgb));
    ESP_ERROR_CHECK(set_leds(led1_rgb, led2_rgb));
    return ESP_OK;
}

esp_err_t do_rainbow(void){
    for(int i = 0; i < 360; i++){
        led_hsl_t led1 = {i, 1, 0.5};
        led_hsl_t led2 = {i + 45, 1, 0.5};
        ESP_ERROR_CHECK(set_leds_hsl(led1, led2));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    return ESP_OK;
}