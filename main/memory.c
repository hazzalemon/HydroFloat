#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_littlefs.h"



void start_littlefs(){
    printf("trying to start littlefs");
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        printf("uhoh, error occured mounting littlefs :(");
    }else{
        printf("great success! littlefs mounted!");
    }
}

void get_coefs(double* coef1, double* coef2, double* coef3, double* coef4, double* coefb){
    char line[128];
    char buffer[50];
    int cur_coef = 0;
    int cur_index = 0;
    FILE *f = fopen("/littlefs/coefs.txt", "r");
    if (f != NULL){
        fgets(line, sizeof(line), f);
        printf("line from coefs.txt: %s\n", line);
        double* coefs[] = {coef1, coef2, coef3, coef4, coefb};
        for(int i = 0; i < sizeof(line); i++){
            if(line[i] == ','){
                buffer[cur_index] = '\0';
                *coefs[cur_coef] = strtod(buffer, NULL);
                cur_coef++;
            cur_index = 0;
            }else if(line[i] == '\0'){
                buffer[cur_index] = '\0';
                *coefs[cur_coef] = strtod(buffer, NULL);
                break;
        }else{
            buffer[cur_index] = line[i];
            cur_index++;
        }
        }
        fclose(f);
    }
    // char line[2];
    // FILE *f = fopen("/littlefs/coefs.txt", "r");
    // printf("reading coeficient file");
    // if (f != NULL){
    //     fgets(line, sizeof(line), f);
    //     double* coefs[] = {coef1, coef2, coef3, coef4, coefb};
    //     int cur_coef = 0;
    //     int cur_index = 0;
    //     char buffer[50];
    //     printf("starting for loop");
    //     for(int i = 0; i < sizeof(line); i++){
    //         if(line[i] == ','){
    //             buffer[cur_index] = '\0';
    //             *coefs[cur_coef] = strtod(buffer, NULL);
    //             cur_coef++;
    //             cur_index = 0;
    //         }else{
    //             buffer[cur_index] = line[i];
    //             cur_index++;
    //         }
    //     }

    //     fprintf(f, "read from file: %s", line);

    //     fclose(f);
    // }
}

void save_coefs(double* coef1, double* coef2, double* coef3, double* coef4, double* coefb){
    remove("/littlefs/coefs.txt");
    FILE *f = fopen("/littlefs/coefs.txt", "w");
    printf( "%0.6f,%0.6f,%0.6f,%0.6f,%0.6f\n", *coef1, *coef2, *coef3, *coef4, *coefb);
    fprintf(f, "%0.6f,%0.6f,%0.6f,%0.6f,%0.6f", *coef1, *coef2, *coef3, *coef4, *coefb);
    fclose(f);
}

void save_wifi_creds(char ssid[], char password[]){
    remove("/littlefs/wifi.txt");
    FILE *f = fopen("/littlefs/wifi.txt", "w");
    fprintf(f, "%s,%s,", ssid, password);
    fclose(f);
}

void get_wifi_creds(char ssid[], char password[]){

    char line[128];
    FILE *f = fopen("/littlefs/wifi.txt", "r");
    if (f != NULL){
        fgets(line, sizeof(line), f);
        strcpy(ssid, "\0");
        strcpy(password, "\0");

        int i=0;
        int index = 0;
        while(line[i] != ',' && line[i] != '\0'){
            ssid[index] = line[i];
            i++;
            index++;
        }
        i++;
        while(index < sizeof(ssid)){
            ssid[index] = '\0';
            index++;
        }
        index = 0;
        while(line[i] != ',' && line[i] != '\0'){
            password[index] = line[i];
            i++;
            index++;
        }
        while(index < sizeof(password)){
            password[index] = '\0';
            index++;
        }
        fclose(f);
    }else{
        printf("no file found");
    }
}

// void get_coefs_creds(char ssid[], char password[]){
//     char line[128];
//     FILE *f = fopen("/littlefs/coefs.txt", "r");
//     if (f != NULL){
//         fgets(line, sizeof(line), f);
//         printf("read from file: %s", line);
//         fclose(f);
//     }else{
//         printf("no file found");
//     }
// }
