/*
 * Daniel Villalva
 * Lab 7 - WIFI and IOT
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"

const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_html_hdr1[] = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\n";
const static char http_index_hml[] = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset=UTF-8 name="viewport" content="width = device-width initial-scale = 1.0">
    <title>Home Page</title>
</head>
<body>
    <div class="header">
        <h1>Lab 7: WiFi and ADC</h1>
    </div> <input class="btn" id="btn0" type="button" value="BTN0" onclick="sendRequestLed()"> <input class="btn"
        id="btn1" type="button" value="Get Data" onclick="sendRequestData()">
    <div class="sensorVal">
        <p>Sensor Value: </p>
        <p id="sen"></p>
    </div>
    <style>
        * {
            margin: 0;
            padding: 0;
        }
        body {
            background-color: #dfe0ff; 
        }
        .header {
            width: 100%;
            height: 50px;
            color: white;
            background-color: #5b59b5;
            padding: 0;
            text-align: center;
        }
        .header h1 {
            color: white;
            vertical-align: center;
            font-size: 42px;
        }
        .btn {
            margin: 0;
            margin-top: .5%;
            background-color: #5b59b5;
            width: 100%;
            border: none;
            color: white;
            padding: 25px 38px;
            text-align: center;
            text-decoration: none;
            font-size: 20px;
        }
        .sensorVal {
            margin: 0;
            margin-top: .5%;
            width: 100%;
            height: 100px;
            color: white;
            background-color: #5b59b5;
            padding: 0;
            text-align: center;
        }
        .sensorVal p {
            color: white;
            vertical-align: center;
            font-size: 38px;
        }
    </style>
    <script> 
        function changeButton(value) { 
            var btn = document.getElementById("btn0"); 
                if (value === "0") { 
                    btn.value = "LED is ON"; 
                } 
                else { 
                    btn.value = "LED is OFF"; 
                } 
            } 
            function sendRequestLed() { 
                var http = new XMLHttpRequest(); 
                http.onreadystatechange = (() => { 
                    if (http.readyState === 4) { 
                        if (http.status === 200) { 
                            changeButton(http.responseText); 
                        } 
                    } 
                }); 
                http.open("GET", "0", true); 
                http.send(); 
            } 
            function sendRequestData() { 
                var http = new XMLHttpRequest(); 
                http.onreadystatechange = (() => { 
                    if (http.readyState === 4) { 
                        if (http.status === 200) { 
                            document.getElementById("sen").innerHTML = http.responseText; 
                        } 
                    } 
                }); 
                http.open("GET", "1", true); 
                http.send(); 
                } 
                setInterval(sendRequest1, 1000); 
        </script>
</body>
</html>)=====";

#define EXAMPLE_ESP_WIFI_SSID "VILLALVA"
#define EXAMPLE_ESP_WIFI_PASS "80567676"
#define EXAMPLE_MAX_STA_CONN 2
#define BLINK_GPIO 2
int count = 0;

static EventGroupHandle_t s_wifi_event_group;

void wifi_init_softap() {
    s_wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();   //Initialize TCP connection
    ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
     },
    };

    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void onURL(struct netconn *conn, char command) {
    /*If button is pressed it will toggle LED and the HTML webpage*/
    if(command == '0'){
        if(count == 0){
            netconn_write(conn, http_html_hdr1, sizeof(http_html_hdr1)-1, NETCONN_NOCOPY);
            netconn_write(conn, "0", 1, NETCONN_NOCOPY);
            gpio_set_level(BLINK_GPIO, 1);
            count++;
        }
        else {
            netconn_write(conn, http_html_hdr1, sizeof(http_html_hdr1)-1, NETCONN_NOCOPY);
            netconn_write(conn, "1", 1, NETCONN_NOCOPY);
            gpio_set_level(BLINK_GPIO, 0);
            count = 0;
        }
    }
    /*If the other button is pressed it will read the ADC pin and print it to the HTML webpage*/
    else if(command == '1'){
        int raw = adc1_get_raw(ADC1_CHANNEL_0);     //store the value of the ADC into a variable
        float voltage = (raw * 3.3)/4096;  //Convert the ADC data into voltage
        char buffer[6];
        sprintf(buffer, "%.1f V", voltage);  //Convert float to string
        netconn_write(conn, http_html_hdr1, sizeof(http_html_hdr1)-1, NETCONN_NOCOPY);
        netconn_write(conn, buffer, 6, NETCONN_NOCOPY);
    }
    else{
        netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);
        netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
    }
}

static void http_server_netconn_serve(struct netconn *conn) {
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;
    /* Read the data from the port, blocking if nothing yet there.
    We assume the request (the part we care about) is in one netbuf */
    err = netconn_recv(conn, &inbuf);
    gpio_pad_select_gpio(BLINK_GPIO);   //Clear pin for LED
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);   //Set pin as output
    if (err == ERR_OK) {
        netbuf_data(inbuf, (void**)&buf, &buflen);
        /* Is this an HTTP GET command? (only check the first 5 chars, since there are other formats for GET, and we're keeping it very simple )*/
        if (buflen>=5 && buf[0]=='G' && buf[1]=='E' && buf[2]=='T' && buf[3]==' ' && buf[4]=='/' ) {
            /* Send the HTML header
            * subtract 1 from the size, since we dont send the \0 in the string
            * NETCONN_NOCOPY: our data is const static, so no need to copy it */
            onURL(conn, buf[5]);
        }
    }
    netconn_close(conn);
    netbuf_delete(inbuf);
}

static void http_server(void *pvParameters) {
    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    do {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
            }
    } while(err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
}

void setADC() {
    adc1_config_width(ADC_WIDTH_BIT_12);    //12-Bit conversion
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);     //ADC1 channel 1 is GPIO 36
}
void app_main() {
    nvs_flash_init();
    setADC();   //Initalize ADC channel
    wifi_init_softap(); //Initialize softAP
    xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
}
