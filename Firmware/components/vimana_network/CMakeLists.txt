set(priv_requires "esp_event" "driver" "esp_wifi")
idf_component_register(SRCS "vimana_network.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES ${priv_requires})
