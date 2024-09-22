#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <esp_http_server.h>

extern double float_angle;
extern double temp_read;

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t);


#endif