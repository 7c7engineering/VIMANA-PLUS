#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_mac.h"
#include "freertos/queue.h"


typedef struct vimana_rx_packet {
    uint8_t aux_ch; // Aux channel
    int16_t rch;    // Roll channel
    int16_t pch;    // Pitch channel
    int16_t tch;    // Throttle channel
    int16_t ych;    // Yaw channel

} vimana_rx_packet_t;

esp_err_t vimana_network_init(QueueHandle_t rx_queue);
