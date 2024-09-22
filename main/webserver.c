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

extern double float_angle;
extern double temp_read;
extern double density;
extern double bat_volt;


/* Our URI handler function to be called during GET /uri request */
esp_err_t send_main(httpd_req_t *req)
{
    /* Send a simple response */
    httpd_resp_send(req, PAGE_MAIN, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t send_xml(httpd_req_t *req){
    char resp[2048];
    char buf[40];
    printf("sending XML\n");
    strcpy(resp, "<?xml version = '1.0'?>\n<Data>\n");
    sprintf(buf, "<Float_ang>%f</Float_ang>\n", float_angle);
    strcat(resp, buf);
    sprintf(buf, "<tempr>%f</tempr>\n", temp_read);
    strcat(resp, buf);
    sprintf(buf, "<density>%f</density>\n", density);
    strcat(resp, buf);
    sprintf(buf, "<bat>%f</bat>\n", bat_volt);
    strcat(resp, buf);

    strcat(resp, "</Data>\n");
    // printf(resp);
    // sprintf(res, )
    httpd_resp_set_type(req, "text/xml");
    httpd_resp_send(req,resp, HTTPD_RESP_USE_STRLEN);
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