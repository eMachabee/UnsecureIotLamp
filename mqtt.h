#include <stdbool.h>

#include "lwip/apps/mqtt_priv.h"

#warning Set unique value for MQTT broker
#define MQTT_CLIENT_ID_NAME "unsecureiotlamp"

bool MQTT_Init(ip_addr_t* addr, uint16_t port);
