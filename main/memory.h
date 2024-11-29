#ifndef MEMORY_H
#define MEMORY_H




void start_littlefs();
void get_coefs(double* coef1, double* coef2, double* coef3, double* coef4, double* coefb);
void save_coefs(double* coef1, double* coef2, double* coef3, double* coef4, double* coefb);
void get_wifi_creds(char ssid[], char password[]);
void save_wifi_creds(char ssid[], char password[]);
void start_littlefs();




#endif