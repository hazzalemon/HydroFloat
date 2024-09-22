#include <stdio.h>
#include <stdlib.h>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "hal/adc_types.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

static esp_adc_cal_characteristics_t adc2_chars;

// double measureBatt(){
//     esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_DEFAULT, 0,&adc2_chars);
//     adc2_config_channel_atten( ADC_CHANNEL_7, ADC_ATTEN_DB_12 );
//     uint32_t reading;
//     esp_err_t ret = ESP_FAIL;
//     double voltage; 
//     while(ret != ESP_OK){
//         ret = adc2_get_raw(ADC_CHANNEL_7, ADC_WIDTH_BIT_DEFAULT, &reading);
//     }
//     printf("adc raw out: %lu\n", reading);
//     voltage = reading * 2450.0/4095.0;
//     printf("adc calibrated out: %f\n", voltage);
//     return (voltage/1000.0);
//     }

float measureBatt(){
    int reading;
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc2_handle));
    adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ADC_CHANNEL_7, &config));
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc2_handle));
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_2,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_cali_handle_t cal_handle;
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cal_handle));

    esp_err_t ret = ESP_FAIL;
    while(ret != ESP_OK){
        ret = adc_oneshot_get_calibrated_result(adc2_handle, cal_handle, ADC_CHANNEL_7, &reading);
    }
    // printf("raw_reading: %i", reading);
    // printf("cal reading: %f", reading*2.0/1000.0);

    return (reading*2.0/1000.0);
}
    