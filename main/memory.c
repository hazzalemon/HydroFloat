#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_littlefs.h"

void start_littlefs(){
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // Use settings defined above to initialize and mount LittleFS filesystem.
    // Note: esp_vfs_littlefs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
}

void get_coefs(float* coef1, float* coef2, float* coef3, float* coef4){
    FILE *f = fopen("/littlefs/coefs.txt", "r");
    fclose(f);
}

void save_coefs(float* coef1, float* coef2, float* coef3, float* coef4){
    remove("/littlefs/coefs.txt");
    FILE *f = fopen("/littlefs/coefs.txt", "w");
    fprintf(f, "%f,%f,%f,%f\n", coef1, coef2, coef3, coef4);
    fclose(f);
}
