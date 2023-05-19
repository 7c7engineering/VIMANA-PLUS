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