#pragma once

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "esp_err.h"


esp_err_t motor_init(void);
esp_err_t servo_init(void);
esp_err_t motor_set_speed(int motor, float speed);
esp_err_t servo_set_angle(int servo, float angle);