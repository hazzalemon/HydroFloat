#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_system.h"
#include "nvs_flash.h"
// #include "esp_event.h"
#include "esp_netif.h"
// #include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"




void send_mqtt_packet(double temp, double gravity, double angle, double batt){
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.0.2:1883",
    };
    char buffer[100];
    sprintf(buffer, "temp:%f,gravity:%f,angle:%f,batt:%f",temp, gravity, angle, batt);
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
    esp_mqtt_client_publish(client, "/topic/hydrofloat", buffer, 0, 1, 0);
    // esp_mqtt_client_disconnect(client);

}
