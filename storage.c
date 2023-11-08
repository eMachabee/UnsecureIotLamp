#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <hardware/sync.h>
#include <hardware/flash.h>

#include "storage.h"

// Based on https://www.makermatrix.com/blog/read-and-write-data-with-the-pi-pico-onboard-flash/
// TODO Implement wear protection
// TODO If needed, add length protection

// FLASH_SECTOR_SIZE = 4096
// FLASH_PAGE_SIZE = 256

/* DEFINES */
#define FIELD_SIZE (FLASH_PAGE_SIZE/sizeof(char))
#define BASE_ADDRESS (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define DEVICE_NAME_ADDRESS (BASE_ADDRESS)
#define MQTT_ADDRESS_ADDRESS (BASE_ADDRESS + FIELD_SIZE)

/* GLOBAL VARIABLES */
char deviceName[512] = {0x0};  // 512 char
char mqttAddress[512] = {0x0};  // 512 char

/* PRIVATE DEFINES */
static bool ReadStringFromFlash(uint32_t address, char* string, size_t len);
static bool WriteDataToFlash();
static char ReadCharFromFlash(uint32_t address);

/* PUBLIC FUNCTIONS */
bool STORAGE_Init(const char* name, const char* address) {
    bool ret = true;

    if ((ReadCharFromFlash(DEVICE_NAME_ADDRESS) == 0xff) || (ReadCharFromFlash(MQTT_ADDRESS_ADDRESS) == 0xff)) {
        printf("**NOT SET, write values %s, %s", name, address);
        strcpy(deviceName, name);
        strcpy(mqttAddress, address);
        ret &= STORAGE_Save(true);
    } else {
        ret &= ReadStringFromFlash(DEVICE_NAME_ADDRESS, deviceName, 256);
        ret &= ReadStringFromFlash(MQTT_ADDRESS_ADDRESS, mqttAddress, 256);
    }
    return ret;
}

bool STORAGE_Save(bool mqttenable) {
    return WriteDataToFlash();
}

bool STORAGE_SetDeviceName(const char* name) {
    strcpy(deviceName, name);
    return true;
}

bool STORAGE_GetDeviceName(char* name) {
    strcpy(name, deviceName);
    return true;
}

bool STORAGE_SetMqttServer(const char* server) {
    strcpy(mqttAddress, server);
    return true;
}

bool STORAGE_GetMqttServer(char* server) {
    strcpy(server, mqttAddress);
    return true;
}

/* PRIVATE FUNCTIONS */
static bool ReadStringFromFlash(uint32_t address, char* str, size_t len) {
    bool ret = true;
    
    char readChar = 0xff;
    char* pChar;
    uint32_t counter = 0;
    uint32_t addr = 0;

    while (readChar != 0x00 && counter < len) {
        addr = XIP_BASE +  address + counter;  // XIP_BASE is RAM address, we need to skip it
        pChar = (char *)addr;
        readChar = *pChar;
        str[counter] = readChar;
        counter++;
    }

    return ret;
}

static char ReadCharFromFlash(uint32_t addr) {
    char readChar = 0xff;
    char* pChar;

    pChar = (char*)(XIP_BASE + addr);
    readChar = *pChar;
    return readChar;
}

static bool WriteDataToFlash() {
    bool ret = true;
    uint32_t ints = save_and_disable_interrupts();

    flash_range_erase((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);
    flash_range_program(DEVICE_NAME_ADDRESS, deviceName, strlen(deviceName));
    if (strlen(deviceName) < 256) {
        flash_range_program(MQTT_ADDRESS_ADDRESS, mqttAddress, strlen(mqttAddress));
    }

    restore_interrupts(ints);
    return ret;
}
