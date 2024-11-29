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
#include "esp_sleep.h"
#include "accel.h"
#include "webserver.h"
#include "batt_read.h"
#include "memory.h"


// global variable definitions
#define LOCAL_SSID "hydrofloat"
#define LOCAL_PASS ""
char ssid[50];
char wifi_password[50];
int16_t Xaccel = 0;
int16_t Yaccel = 0;
int16_t Zaccel = 0;
double fx = 0;
double fz = 0;
double fy = 0;
int wakeup_time_sec = 30;
double float_angle = 0;
double roll_accel = 0;
double temp_read = 0;
double density = 1.2;

double coef1 = 0;
double coef2 = 0;
double coef3 = 0;
double coef4 = 0;
double coefb = 0;

float bat_volt = 0;

RTC_DATA_ATTR bool mqtt_mode = false;

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

void send_mqtt(double bat_volt, double angle, double temp, double density){

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
    printf("calling start_littlefs function");
    start_littlefs();
    get_wifi_creds(ssid, wifi_password);
    get_coefs(&coef1, &coef2, &coef3, &coef4, &coefb);
    if (mqtt_mode ==false){
        wifi_init_softap();
        start_webserver();
    } else{
        ;
    }
    




    while(1){
        if(mqtt_mode == false){
            update_accel(accelo, &Xaccel, &Yaccel, &Zaccel, &temp_read);
            bat_volt = measureBatt();
            fx = Xaccel;
            fz = Zaccel;
            fy = Yaccel;
            roll_accel = sqrt(fx*fx + fz*fz);
            float_angle = asin(roll_accel/(sqrt(fy*fy + fx*fx + fz*fz))) * (180/M_PI);
            printf("X val:  %d Y val: %d Z val: %d temp %f angle %f  bat_voltage: %f \n", Xaccel, Yaccel, Zaccel, temp_read, float_angle, bat_volt);
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        else{
            vTaskDelay(100/portTICK_PERIOD_MS);
            printf("coming out of sleep.....");
            ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));            
            update_accel(accelo, &Xaccel, &Yaccel, &Zaccel, &temp_read);
            bat_volt = measureBatt();
            float_angle = asin(roll_accel/(sqrt(Yaccel*Yaccel + Xaccel*Xaccel + Zaccel*Zaccel))) * (180/M_PI);
            // send_MQTT();
            esp_deep_sleep_start();

        }
    }
    
}


