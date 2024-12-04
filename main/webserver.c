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
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_sleep.h"



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
extern char ssid[32];
extern char wifi_password[64];
extern int wakeup_time_sec; 

static const char TAG[] = "http_server";


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
    char buf[100];
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
    // printf("finished creating response packet\n");
    // printf(resp);
    // sprintf(res, )
    httpd_resp_set_type(req, "text/xml");
    httpd_resp_send(req,resp, HTTPD_RESP_USE_STRLEN);
    // printf("completed sending response");
    return ESP_OK;
}

esp_err_t enter_mqtt(httpd_req_t *req){
    printf("entering MQTT mode\n");
    mqtt_mode = true;
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));
    esp_deep_sleep_start();
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

static esp_err_t http_server_ota_update_handler(httpd_req_t *req)
{
  esp_err_t error;
  esp_ota_handle_t ota_handle;
  char ota_buffer[1024];
  int content_len = req->content_len;   // total content length
  int content_received = 0;
  int recv_len = 0;
  bool is_req_body_started = false;
  bool flash_successful = false;
  printf("entered ota update handler\n");
  // get the next OTA app partition which should be written with a new firmware
  const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

  // our ota_buffer is not sufficient to receive all data in a one go
  // hence we will read the data in chunks and write in chunks, read the below
  // mentioned comments for more information
  do
  {
    // The following is the API to read content of data from the HTTP request
    /* This API will read HTTP content data from the HTTP request into the
     * provided buffer. Use content_len provided in the httpd_req_t structure to
     *  know the length of the data to be fetched.
     *  If the content_len is to large for the buffer then the user may have to
     *  make multiple calls to this functions (as done here), each time fetching
     *  buf_len num of bytes (which is ota_buffer length here), while the pointer
     *  to content data is incremented internally by the same number
     *  This function returns
     *  Bytes: Number of bytes read into the buffer successfully
     *  0: Buffer length parameter is zero/connection closed by peer.
     *  HTTPD_SOCK_ERR_INVALID: Invalid Arguments
     *  HTTPD_SOCK_ERR_TIMEOUT: Timeout/Interrupted while calling socket recv()
     *  HTTPD_SOCK_ERR_FAIL: Unrecoverable error while calling socket recv()
     *  Parameters to this function are:
     *  req: The request being responded to
     *  ota_buffer: Pointer to a buffer that the data will be read into
     *  length: length of the buffer which ever is minimum (as we don't want to
     *          read more data which buffer can't handle)
     */
    recv_len = httpd_req_recv(req, ota_buffer, MIN(content_len, sizeof(ota_buffer)));
    // if recv_len is less than zero, it means some problem (but if timeout error, then try again)
    if( recv_len < 0 )
    {
      // Check if timeout occur, then we will retry again
      if( recv_len == HTTPD_SOCK_ERR_TIMEOUT )
      {
        ESP_LOGI(TAG, "http_server_ota_update_handler: Socket Timeout");
        continue;     // Retry Receiving if Timeout Occurred
      }
      // If there is some other error apart from Timeout, then exit with fail
      ESP_LOGI(TAG, "http_server_ota_update_handler: OTA Other Error, %d", recv_len);
      return ESP_FAIL;
    }
    ESP_LOGI(TAG, "http_server_ota_update_handler: OTA RX: %d of %d", content_received, content_len);

    // We are here which means that "recv_len" is positive, now we have to check
    // if this is the first data we are receiving or not, If so, it will have
    // the information in the header that we need
    if( !is_req_body_started )
    {
      is_req_body_started = true;
      // Now we have to identify from where the binary file content is starting
      // this can be done by actually checking the escape characters i.e. \r\n\r\n
      // Get the location of the *.bin file content (remove the web form data)
      // the strstr will return the pointer to the \r\n\r\n in the ota_buffer
      // and then by adding 4 we reach to the start of the binary content/start
      char *body_start_p = strstr(ota_buffer, "\r\n\r\n") + 4u;
      int body_part_len = recv_len - (body_start_p - ota_buffer);
    //   ESP_LOGI(TAG, "http_server_ota_update_handler: OTA File Size: %d", content_len);
      /*
       * esp_ota_begin function commence an OTA update writing to the specified
       * partition. The specified partition is erased to the specified image
       * size. If the image size is not yet known, OTA_SIZE_UNKNOWN is passed
       * which will cause the entire partition to be erased.
       * On Success this function allocates memory that remains in use until
       * esp_ota_end is called with the return handle.
       */
      error = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
      if( error != ESP_OK )
      {
        ESP_LOGI(TAG, "http_server_ota_update_handler: Error with OTA Begin, Canceling OTA");
        return ESP_FAIL;
      }
      else
      {
        ESP_LOGI(TAG, "http_server_ota_update_handler: Writing to partition subtype %d at offset 0x%lx", update_partition->subtype, update_partition->address);
      }
      /*
       * esp_ota_write function, writes the OTA update to the partition.
       * This function can be called multiple times as data is received during
       * the OTA operation. Data is written sequentially to the partition.
       * Here we are writing the body start for the first time.
       */
      esp_ota_write(ota_handle, body_start_p, body_part_len);
      content_received += body_part_len;
    }
    else
    {
      /* Continue to receive data above using httpd_req_recv function, and write
       * using esp_ota_write (below), until all the content is received. */
      esp_ota_write(ota_handle, ota_buffer, recv_len);
      content_received += recv_len;
    }

  } while ( (recv_len>0) && (content_received < content_len) );
  // till complete data is received and written or some error is there we will
  // remain in the above mentioned do-while loop

  /* Finish the OTA update and validate newly written app image.
   * After calling esp_ota_end, the handle is no longer valid and memory associated
   * with it is freed (regardless of the results).
   */
  if( esp_ota_end(ota_handle) == ESP_OK )
  {
    // let's update the partition i.e. configure OTA data for new boot partition
    if( esp_ota_set_boot_partition(update_partition) == ESP_OK )
    {
      const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
      ESP_LOGI(TAG, "http_server_ota_update_handler: Next boot partition subtype %d at offset 0x%lx", boot_partition->subtype, boot_partition->address);
      flash_successful = true;
    }
    else
    {
      ESP_LOGI(TAG, "http_server_ota_update_handler: Flash Error");
    }
  }
  else
  {
    ESP_LOGI(TAG, "http_server_ota_update_handler: esp_ota_end Error");
  }

  // We won't update the global variables throughout the file, so send the message about the status
  if( flash_successful )
  {
    ;
    // http_server_monitor_send_msg(HTTP_MSG_WIFI_OTA_UPDATE_SUCCESSFUL);
  }
  else
  {
    ;
    // http_server_monitor_send_msg(HTTP_MSG_WIFI_OTA_UPDATE_FAILED);
  }
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
    .handler = enter_mqtt,
    .user_ctx = NULL
};
httpd_uri_t ota_update =
{
  .uri       = "/OTAupdate",
  .method    = HTTP_POST,
  .handler   = http_server_ota_update_handler,
  .user_ctx  = NULL
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
        httpd_register_uri_handler(server, &start_mqtt);
        httpd_register_uri_handler(server, &ota_update);

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