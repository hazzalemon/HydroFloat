#ifndef WIFI_H
#define WIFI_H

extern char ssid[32];
extern char wifi_password[64];

void wifi_init_softap();
void wifi_init_sta(void);

#endif