#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "leds.h"
#include "motor.h"
#include "vimana_network.h"


const char * tag = "MAIN";


QueueHandle_t vimana_rx_queue;          // Queue for incoming packets from the network (now UDP, could be replaced with BLE,LORA,etc.)
EventGroupHandle_t  vimana_event_group; // Here we store status-bits of the aircraft

const EventBits_t APP_PWR_CH;
const EventBits_t APP_AUX_CH;

static vimana_rx_packet_t prev_packet = {0}; // Previous packet, used to detect changes

float mapf(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void turn_off_motors(){
    motor_set_speed(1, 0);
    motor_set_speed(2, 0);
}


/*
This function will control the plane with just two motors, no servos.
It will use the throttle channel to control the speed of the motors, 
and the roll channel to control the differential speed between the motors.

TODO: add parameters to controll the mixing.

*/
void update_motors_no_servo(int16_t throttle, int16_t roll){
    float motor1 = 0;
    float motor2 = 0;
    // tch is throttle
    // rch is roll
    motor1 = (float)throttle + (float)roll;
    motor2 = (float)throttle - (float)roll;
    motor1 = mapf(motor1, 0, 512, 0, 100);
    motor2 = mapf(motor2, 0, 512, 0, 100);
    motor_set_speed(1, motor1);
    motor_set_speed(2, motor2);

}



void vtask_plane_state_machine(void *pvParameters)
{
    vimana_rx_packet_t packet;
    while(1) {
        uint8_t aux_ch = 0;
        // Wait for a packet to arrive within 3 seconds
        if(xQueueReceive(vimana_rx_queue, &packet, 3000 / portTICK_PERIOD_MS)) {
            // New control input received!
            if(memcmp(&prev_packet, &packet, sizeof(vimana_rx_packet_t)) == 0) { // Data is the same, ignore
                // Only update PWM, or servo PPMS if the input has changed so we don't mess to much with the timers
                continue; // Go back to waiting for a packet
            }
            prev_packet = packet;
            ESP_LOGI(tag, "New control input received: aux_ch=%d, rch=%d, pch=%d, tch=%d, ych=%d", packet.aux_ch, packet.rch, packet.pch, packet.tch, packet.ych);
            if(packet.aux_ch & 0x80){
                set_leds((led_t){255,0,160}, (led_t){255,0,160});  // These should move in LED statemachine in future!
                xEventGroupSetBits(vimana_event_group, APP_PWR_CH);
                update_motors_no_servo(packet.tch, packet.rch);

            }
            else{
                xEventGroupClearBits(vimana_event_group, APP_PWR_CH);
                set_leds((led_t){0,50,0}, (led_t){0,50,0});
                turn_off_motors();
            }
            if(packet.aux_ch & 0x40) xEventGroupSetBits(vimana_event_group, APP_AUX_CH);
            else xEventGroupClearBits(vimana_event_group, APP_AUX_CH);
        }
        else {
            ESP_LOGW(tag, "No packet received in 3 seconds!");
            xEventGroupClearBits(vimana_event_group, APP_PWR_CH);
            set_leds((led_t){140,50,0}, (led_t){140,50,0});
            turn_off_motors();
        }
    }
}



void app_main(void)
{
    // set pin 47 high to enable the ADC resistor divider (HW bug)
    gpio_set_direction(47, GPIO_MODE_OUTPUT);
    gpio_set_level(47, 1);
    ESP_ERROR_CHECK(nvs_flash_init());                  // Initialize NVS (used for Wifi driver)
    ESP_ERROR_CHECK(esp_netif_init());                  // Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_event_loop_create_default());   // Create FreeRTOS event loop

    vimana_rx_queue = xQueueCreate(10, sizeof(vimana_rx_packet_t));
    vimana_event_group = xEventGroupCreate();

    init_leds();
    motor_init();
    do_rainbow();
    set_leds((led_t){10,10,10}, (led_t){10,10,10});
    
    servo_init();

    while(1){
        // sweep servo 1 from 0 to 180 degrees and back
        for(int i=0; i<180; i++){
            servo_set_angle(1, i);
            led_hsl_t led = (led_hsl_t){i, 1, 0.50};
            set_leds_hsl(led, led);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        for(int i=180; i>0; i--){
            servo_set_angle(1, i);
            led_hsl_t led = (led_hsl_t){i, 1, 0.50};
            set_leds_hsl(led, led);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }


    vimana_network_init(vimana_rx_queue);
    set_leds((led_t){0,50,0}, (led_t){0,50,0});
    xTaskCreate(vtask_plane_state_machine, "rx_packet", 4096, NULL, 5, NULL);
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

