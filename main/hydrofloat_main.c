/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/i2c_master.h"
#include "esp_wifi.h"
#include "string.h"
#include "nvs_flash.h"
#include "math.h"

#include "accel.h"
#include "webserver.h"
#include "batt_read.h"


// global variable definitions
#define LOCAL_SSID "hydrofloat"
#define LOCAL_PASS ""
int Xaccel = 0;
int Yaccel = 0;
int Zaccel = 0;
double float_angle = 0;
double roll_accel = 0;
double temp_read = 0;
double density = 1.2;
float bat_volt = 0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;

    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;

    }
}


void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = LOCAL_SSID,
            .ssid_len = strlen(LOCAL_SSID),
            .channel = 0,
            .password = LOCAL_PASS,
            .max_connection = 4,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(LOCAL_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

}


void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    i2c_master_dev_handle_t accelo;
    init_accel_i2c(&accelo);
    setup_accel(accelo);
    wifi_init_softap();
    start_webserver();



    while(1){
        update_accel(accelo, &Xaccel, &Yaccel, &Zaccel, &temp_read);
        bat_volt = measureBatt();
        roll_accel = sqrt(Xaccel*Xaccel + Zaccel*Zaccel);
        float_angle = asin(roll_accel/(sqrt(Yaccel*Yaccel + Xaccel*Xaccel + Zaccel*Zaccel))) * (180/M_PI);
        printf("X val:  %d Y val: %d Z val: %d temp %f angle %f  bat_voltage: %f \n", Xaccel, Yaccel, Zaccel, temp_read, float_angle, bat_volt);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
}


