#include <stdio.h>
#include "motor.h"


const int GPIO_MOTOR1_PWM = 4;
const int GPIO_MOTOR2_PWM = 5;



esp_err_t motor_init(void){
    // setup mcpwm timers
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 8000;    //frequency = 1000Hz
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config));   //Configure PWM0A & PWM0B with above settings
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_MOTOR1_PWM);                 
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_MOTOR2_PWM);

    return ESP_OK;

}

esp_err_t motor_set_speed(int motor, int speed){
    if (motor == 1){
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speed);
    }
    else if (motor == 2){
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, speed);
    }
    else{
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}