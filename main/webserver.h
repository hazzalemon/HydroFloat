#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <esp_http_server.h>

extern double float_angle;
extern double temp_read;
extern bool mqtt_mode;
extern char ssid[50];
extern char password[50];
extern double coef1;
extern double coef2;
extern double coef3;
extern double coef4;
extern double coefb;

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t);


#endif