#ifndef BATT_READ
#define BATT_READ

#include "hal/adc_types.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

float measureBatt(adc_oneshot_unit_handle_t *adc_oneshot_handle, adc_cali_handle_t *adc_cal_handle);
void setup_batt_measure(adc_oneshot_unit_handle_t adc2_handle, adc_cali_handle_t cal_handle);


#endif