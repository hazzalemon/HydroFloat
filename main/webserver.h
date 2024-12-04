#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <esp_http_server.h>

extern double float_angle;
extern double temp_read;
extern bool mqtt_mode;
extern char ssid[32];
extern char password[64];
extern double coef1;
extern double coef2;
extern double coef3;
extern double coef4;
extern double coefb;
extern int wakeup_time_sec;

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t);


#endif