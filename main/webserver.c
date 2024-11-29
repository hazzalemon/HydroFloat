#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/param.h>
// #include "protocol_examples_common.h"
// #include "protocol_examples_utils.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "webpage.h"
#include "memory.h"

extern double float_angle;
extern double temp_read;
extern double density;
extern bool mqtt_mode;
extern double coef1;
extern double coef2;
extern double coef3;
extern double coef4;
extern double coefb;
extern float bat_volt;
extern char ssid[25];
extern char wifi_password[25];

// void string_split(char* input[], char delimiter ,char* output[], int* start){
//     int end = 0;
//     printf("strlength: %i\n", strlen(input));
//     printf("start split\n");

    // for(int i = 0; i <= strlen(input); i++){
//         printf("started loop");
        // printf("input is: %s\n", input[i]);
        // if(*input[i] == delimiter){
        //     end = i - 1;
        //     break;
        // }
    // }
    // printf("starting the copy operation");
    // for(int i = *start; i < end; i++){
    //     output[i - *start] = input[i];
    // }

// }

/* Our URI handler function to be called during GET /uri request */
esp_err_t send_main(httpd_req_t *req)
{
    printf("httpd_resp_send");
    /* Send a simple response */
    httpd_resp_send(req, PAGE_MAIN, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t send_xml(httpd_req_t *req){
    char resp[1024];
    char buf[84];
    // printf("sending XML\n");
    strcpy(resp, "<?xml version = '1.0'?>\n<Data>\n");
    sprintf(buf, "<Float_ang>%.4f</Float_ang>\n", float_angle);
    strcat(resp, buf);
    sprintf(buf, "<tempr>%.4f</tempr>\n", temp_read);
    strcat(resp, buf);
    sprintf(buf, "<density>%.4f</density>\n", density);
    strcat(resp, buf);
    sprintf(buf, "<bat>%.4f</bat>\n", bat_volt);
    strcat(resp, buf);
    sprintf(buf, "<ssid>%s</ssid>\n", ssid);
    strcat(resp, buf);
    sprintf(buf, "<wifi_password>%s</wifi_password>\n", wifi_password);
    strcat(resp, buf);
    sprintf(buf, "<coef1>%.6f</coef1>\n", coef1);
    strcat(resp, buf);
    sprintf(buf, "<coef2>%.6f</coef2>\n", coef2);
    strcat(resp, buf);
    sprintf(buf, "<coef3>%.6f</coef3>\n", coef3);
    strcat(resp, buf);
    sprintf(buf, "<coef4>%.6f</coef4>\n", coef4);
    strcat(resp, buf);
    sprintf(buf, "<coefb>%.6f</coefb>\n", coefb);
    strcat(resp, buf);
    strcat(resp, "</Data>\n");
    printf("finished creating response packet\n");
    // printf(resp);
    // sprintf(res, )
    httpd_resp_set_type(req, "text/xml");
    httpd_resp_send(req,resp, HTTPD_RESP_USE_STRLEN);
    // printf("completed sending response");
    return ESP_OK;
}

esp_err_t enterMQTT(httpd_req_t *req){
    printf("entering MQTT mode\n");
    mqtt_mode = true;
    return ESP_OK;
}

esp_err_t get_wifi(httpd_req_t *req){
    printf("get_wifi called\n");
    strcpy(ssid, "\0");
    strcpy(wifi_password, "\0");

    int i=0;
    int index = 0;
    while(req->uri[i] != '?'){
        printf("next char: %c\n", req->uri[i]);
        i++;
    }
    i++;
    while(req->uri[i] != '?'){
        ssid[index] = req->uri[i];
        i++;
        index++;
    }
    ssid[index] = '\0';
    index = 0;
    i++;
    while(req->uri[i] != '?'){
        wifi_password[index] = req->uri[i];
        i++;
        index++;
    }
    wifi_password[index] = '\0';
    index = 0;
    printf("ssid: %s password: %s\n", ssid, wifi_password);
    save_wifi_creds(ssid, wifi_password);
    return ESP_OK;
}

esp_err_t get_coefs_web(httpd_req_t *req){
    printf("get_coefs_web called\n");
    double* coefs[] = {&coef4,&coef3,&coef2,&coef1,&coefb};
    int cur_coef = 0;
    int cur_index = 0;
    char buffer[50];
    printf("starting for loop");
    for(int i = 7; i < sizeof(req->uri); i++){
        if(req->uri[i] == '?'){
            buffer[cur_index] = '\0';
            *coefs[cur_coef] = strtod(buffer, NULL);
            cur_coef++;
            printf("size of coefs: %i", sizeof(coefs));
            if(cur_coef >= 5){
                printf("breaking loop. too many coefs");
                break;
            }
            cur_index = 0;
        }else if(req->uri[i] == '\0'){
            buffer[cur_index] = '\0';
            *coefs[cur_coef] = strtod(buffer, NULL);
            break;
        }else{
            printf("cur_index: %i cur_coef: %i\n", cur_index, cur_coef);
            buffer[cur_index] = req->uri[i];
            cur_index++;
        }
    }
    save_coefs(&coef1, &coef2, &coef3, &coef4, &coefb);
    return ESP_OK;
}




/* Our URI handler function to be called during POST /uri request */
// esp_err_t post_handler(httpd_req_t *req)
// {
//     /* Destination buffer for content of HTTP POST request.
//      * httpd_req_recv() accepts char* only, but content could
//      * as well be any binary data (needs type casting).
//      * In case of string data, null termination will be absent, and
//      * content length would give length of string */
//     char content[100];

//     /* Truncate if content length larger than the buffer */
//     size_t recv_size = MIN(req->content_len, sizeof(content));

//     int ret = httpd_req_recv(req, content, recv_size);
//     if (ret <= 0) {  /* 0 return value indicates connection closed */
//         /* Check if timeout occurred */
//         if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//             /* In case of timeout one can choose to retry calling
//              * httpd_req_recv(), but to keep it simple, here we
//              * respond with an HTTP 408 (Request Timeout) error */
//             httpd_resp_send_408(req);
//         }
//         /* In case of error, returning ESP_FAIL will
//          * ensure that the underlying socket is closed */
//         return ESP_FAIL;
//     }

//     /* Send a simple response */
//     const char resp[] = "URI POST Response";
//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = send_main,
    .user_ctx = NULL
};
httpd_uri_t send_telem = {
    .uri      = "/xml",
    .method   = HTTP_PUT,
    .handler  = send_xml,
    .user_ctx = NULL
};
httpd_uri_t receive_wifi = {
    .uri      = "/wifi",
    .method   = HTTP_PUT,
    .handler = get_wifi,
    .user_ctx = NULL

};
httpd_uri_t receive_coefs = {
    .uri      = "/coefs",
    .method   = HTTP_PUT,
    .handler = get_coefs_web,
    .user_ctx = NULL

};
httpd_uri_t start_mqtt = {
    .uri     ="/enterMQTT",
    .method   = HTTP_PUT,
    .handler = get_coefs_web,
    .user_ctx = NULL
};

// /* URI handler structure for POST /uri */
// httpd_uri_t uri_post = {
//     .uri      = "/1",
//     .method   = HTTP_POST,
//     .handler  = send_main,
//     .user_ctx = NULL
// };

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &send_telem);
        httpd_register_uri_handler(server, &receive_wifi);
        httpd_register_uri_handler(server, &receive_coefs);
        // httpd_register_uri_handler(server, &uri_post);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}