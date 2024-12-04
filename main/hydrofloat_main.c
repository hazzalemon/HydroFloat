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
#include "hal/adc_types.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"
#include "mqtt.h"
#include "esp_log.h"
#include "wifi.h"


// global variable definitions

char ssid[32];
char wifi_password[64];
int16_t Xaccel = 0;
int16_t Yaccel = 0;
int16_t Zaccel = 0;
double fx = 0;
double fz = 0;
double fy = 0;
int wakeup_time_sec = 5;
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

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

// ADC handlers
adc_oneshot_unit_handle_t adc2_handle;
adc_cali_handle_t cal_handle;

RTC_DATA_ATTR bool mqtt_mode = false;


double calc_gravity(double angle){
    return coef1*(angle*angle*angle*angle) + coef2 * (angle*angle*angle) + coef3 * (angle*angle) + coef1 * angle +  coefb;
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
    setup_batt_measure(&adc2_handle, &cal_handle);
    printf("calling start_littlefs function");
    start_littlefs();
    get_wifi_creds(ssid, wifi_password);
    get_coefs(&coef1, &coef2, &coef3, &coef4, &coefb);
    if (mqtt_mode == false){
        wifi_init_softap();
        start_webserver();
    } else{
        wifi_init_sta();
    }
    
    while(1){
        if(mqtt_mode == false){
            update_accel(accelo, &Xaccel, &Yaccel, &Zaccel, &temp_read);
            bat_volt = measureBatt(adc2_handle, cal_handle);
            fx = Xaccel;
            fz = Zaccel;
            fy = Yaccel;
            roll_accel = sqrt(fx*fx + fz*fz);
            float_angle = asin(roll_accel/(sqrt(fy*fy + fx*fx + fz*fz))) * (180/M_PI);
            // printf("X val:  %d Y val: %d Z val: %d temp %f angle %f  bat_voltage: %f \n", Xaccel, Yaccel, Zaccel, temp_read, float_angle, bat_volt);
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        else{
            update_accel(accelo, &Xaccel, &Yaccel, &Zaccel, &temp_read);
            fx = Xaccel;
            fz = Zaccel;
            fy = Yaccel;
            roll_accel = sqrt(fx*fx + fz*fz);
            float_angle = asin(roll_accel/(sqrt(fy*fy + fx*fx + fz*fz))) * (180/M_PI);
            bat_volt = measureBatt(adc2_handle, cal_handle);
            float_angle = asin(roll_accel/(sqrt(Yaccel*Yaccel + Xaccel*Xaccel + Zaccel*Zaccel))) * (180/M_PI);
            send_mqtt_packet(temp_read, float_angle, float_angle, bat_volt);
            ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));            
            esp_deep_sleep_start();

        }
    }
    
}


