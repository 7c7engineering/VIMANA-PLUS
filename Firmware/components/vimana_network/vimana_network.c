#include "vimana_network.h"

#define PORT 2390

static const char *TAG = "NETWORK";

const char * APssid = "VIMANA2_VT_101";  // Name for the AP based on country call sign
const char * APpassword = "VIMANA101";   // Password for the AP
#define CONFIG_EXAMPLE_IPV4 1

static QueueHandle_t vimana_rx_queue;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    esp_netif_t* wifiAP = esp_netif_create_default_wifi_ap();
    esp_netif_ip_info_t ip_info;
    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 192,168,43,42);
	IP4_ADDR(&ipInfo.gw, 192,168,43,42);
	IP4_ADDR(&ipInfo.netmask, 255,255,255,0);
	esp_netif_dhcps_stop(wifiAP);
	esp_netif_set_ip_info(wifiAP, &ipInfo);
	esp_netif_dhcps_start(wifiAP);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .channel = 1,
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    strcpy((char *)wifi_config.ap.ssid, APssid);
    strcpy((char *)wifi_config.ap.password, APpassword);
    wifi_config.ap.ssid_len = strlen(APssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             APssid, APssid, 1);
}

void handle_vimana_app_data(char * data){
    vimana_rx_packet_t rx_packet;
    rx_packet.aux_ch = data[0];                               // Using 8 bit wastes 24 bits in the structs because it is aligned to 32 bits in ESP
    rx_packet.rch = ((int16_t)(data[1]<<8) + data[2]) - 296;  // Why this specific offset??
    rx_packet.pch = ((int16_t)(data[3]<<8) + data[4]) - 296;  // Why this specific offset??
    rx_packet.tch = ((int16_t)(data[5]<<8) + data[6]);        
    rx_packet.ych = ((int16_t)(data[7]<<8) + data[8]) - 296;

    /*
    Originally the throttle channel was offset by positive 60 LSB.
    I guess this was done to have the motors turning slightly at zero throttle in app.
    This is not the place to add these offsets, so lets keep this control inputs true to what the app is sending.
    If such behavior is desired, it should be done in the airplane statemachine down the line.
    */

   if(vimana_rx_queue != NULL){ // Always check!
       xQueueSend(vimana_rx_queue, &rx_packet, 0); // Airplane statemachine will handle the rest
   }
}

static void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {
        // Setup socket for UDP listening
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10; // Could be larger for VIMANA, but 10 seconds is fine for now
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        // bind socket to port
        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1) {
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                if(len == 12){ // This is the length of a transmission from the app
                    handle_vimana_app_data(rx_buffer);
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

esp_err_t vimana_network_init(QueueHandle_t rx_queue){
    vimana_rx_queue = rx_queue;

    ESP_LOGI(TAG, "Vimana Network Init");
    ESP_LOGI(TAG, "Starting Wifi AP");
    wifi_init_softap();
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
    return ESP_OK;
}


