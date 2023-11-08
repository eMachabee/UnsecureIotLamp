#include "mqtt.h"
#include <string.h>

#include "email.h"

#warning Set MQTT credentials
struct mqtt_connect_client_info_t mqtt_client_info =
{
 MQTT_CLIENT_ID_NAME,
 "mqttuser", /* user */
 "mqttpw", /* pass */
 60, /* keep alive */
 NULL, /* will_topic */
 NULL, /* will_msg */
 0, /* will_qos */
 0 /* will_retain */
#if LWIP_ALTCP && LWIP_ALTCP_TLS
 , NULL
#endif
};

typedef struct MQTT_CLIENT_DATA_T {
    mqtt_client_t *mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    uint8_t data[MQTT_OUTPUT_RINGBUF_SIZE];
    uint8_t topic[100];
    uint32_t len;
} MQTT_CLIENT_DATA_T;

ip_addr_t addr;
MQTT_CLIENT_DATA_T* mqtt;


static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
    strcpy((char*)mqtt_client->topic, (char*)topic);
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    // printf("mqtt_incoming_data_cb\n");
    MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
    LWIP_UNUSED_ARG(data);
    strncpy((char*)mqtt_client->data, (char*)data, len);
    printf("[MQTT] Received topic: %s\n", (char*)mqtt_client->topic);
    printf("[MQTT] Data received over MQTT: %s\n", (char*)mqtt_client->data);

    char dest[100] = {0};
    char* slash = strstr((char*)mqtt_client->topic, "/");
    if (slash == NULL) {
        return;
    }
    strncpy(dest, slash + 1, 100);

    EMAIL_SendEmail(dest, (char*)mqtt_client->data);

    memset(mqtt_client->data, 0x00, strlen((char*)mqtt_client->data));
    memset(mqtt_client->topic, 0x00, strlen((char*)mqtt_client->topic));
}

static void mqtt_request_cb(void *arg, err_t err) {
  MQTT_CLIENT_DATA_T* mqtt_client = ( MQTT_CLIENT_DATA_T*)arg;

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", mqtt_client->mqtt_client_info.client_id, (int)err));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
  MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
  LWIP_UNUSED_ARG(client);

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", mqtt_client->mqtt_client_info.client_id, (int)status));

  if (status == MQTT_CONNECT_ACCEPTED) {
    printf("MQTT_CONNECT_ACCEPTED\n");

    //example_publish(client, arg);
    //mqtt_disconnect(client);

    mqtt_sub_unsub(client,
            "#", 0,
            mqtt_request_cb, arg,
            1);        
  }
}

bool MQTT_Init(ip_addr_t* addr, uint16_t port) {
    mqtt = (MQTT_CLIENT_DATA_T*)calloc(1, sizeof(MQTT_CLIENT_DATA_T));

    if (!mqtt) {
        printf("No RAM for MQTT Client\n");
        return 0;
    }

    mqtt->mqtt_client_info = mqtt_client_info;
    mqtt->mqtt_client_inst = mqtt_client_new();

    mqtt_set_inpub_callback(mqtt->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, mqtt);

    err_t err = mqtt_client_connect(mqtt->mqtt_client_inst, addr, port, &mqtt_connection_cb, mqtt, &mqtt->mqtt_client_info);
    if (err != ERR_OK) {
        printf("connect error\n");
        return false;
    }

    return true;
}

