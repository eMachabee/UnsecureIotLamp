#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#include "storage.h"
#include "lwip/dns.h"

#include "mqtt.h"

#define LIGHT_MQTT_PORT (1885);
const char DNS_IP_S[] = "8.8.8.8";
bool dns_done = false;
ip_addr_t mqtt_ip;

// Wifi credentials
#warning Set wifi credentials
const char WIFI_SSID[] = "myssid";
const char WIFI_PASSWORD[] = "mypassword";

#warning Set MQTT Server
const char DEFAULT_LIGHT_NAME[] = "Unsecure Iot Lamp";
const char DEFAULT_MQTT_SERVER[] = "mqtt.example.com";

static void DnsDoneCallback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

int main() {
    ip_addr_t dns_ip;
    char mqttServ[256] = {0x0};
    uint16_t mqtt_port = LIGHT_MQTT_PORT;
    err_t err = ERR_OK;
    stdio_init_all();

    cyw43_arch_init();

    cyw43_arch_enable_sta_mode();

    // Connect to the WiFI network - loop until connected
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected!\n");
    
    // Initialize web server
    httpd_init();
    printf("Http server initialized\n");

    // Configure SSI and CGI handler
    ssi_init(); 
    printf("SSI Handler initialized\n");
    cgi_init();
    printf("CGI Handler initialized\n");

    // Initialize storage
    if (STORAGE_Init(DEFAULT_LIGHT_NAME, DEFAULT_MQTT_SERVER) == true) {
        // Write the default server address and name if none is set
        printf("Storage initialized\n");
        char devName[256] = {0x0};
        STORAGE_GetDeviceName(devName);
        STORAGE_GetMqttServer(mqttServ);
        printf("Device name: %s\n", devName);
        printf("MQTT Server: %s\n", mqttServ);
    }

    // Setup DNS and obtain IP address for MQTT
    dns_init();
    (void)ipaddr_aton(DNS_IP_S, &dns_ip);
    (void)dns_setserver(0, &dns_ip);
    err = dns_gethostbyname(mqttServ, &mqtt_ip, &DnsDoneCallback, NULL);

    if (err == ERR_ARG) {
        printf("[DNS] dns client not initialized or invalid hostname\n");
    } else if (err == ERR_INPROGRESS) {
        printf("[DNS] enqueue a request to be sent to the DNS server for resolution if no errors are present\n");
    } else if (err == ERR_OK) {
        printf("[DNS] hostname already in local table\n");
    }

    while (dns_done != true) {
        ;
    }

    printf("[DNS] Done operations done, IP found\n");

    if (MQTT_Init(&mqtt_ip, mqtt_port) != true) {
        printf("[ERROR] Error starting MQTT");
    }

    // Infinite loop
    while(1);
}

static void DnsDoneCallback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    printf("[DNS] DNS query done for %s\n", name);
    memcpy(&mqtt_ip, ipaddr, sizeof(ip_addr_t));
    dns_done = true;
}
