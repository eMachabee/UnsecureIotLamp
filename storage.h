#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdbool.h>

/**
 * @brief   Initialize the storage module
 * @param   name    Pointer to the default name, null terminated
 * @param   address    Pointer to the default address name, null terminated
 * 
 * @return  Success
*/
bool STORAGE_Init(const char* name, const char* address);

/**
 * @brief   Save the changes to flash
 * @param   mqttenable  Enable saving MQTT in addition to name
 * 
 * @return  Success
*/
bool STORAGE_Save(bool mqttenable);

/**
 * @brief   Set the device name to flash
 * @param   name    Pointer to the name to write, null terminated
 * 
 * @return  Success
*/
bool STORAGE_SetDeviceName(const char* name);

/**
 * @brief   Get the device name from flash
 * @param   name    Pointer to write the name, null terminated
 * 
 * @return  Success
*/
bool STORAGE_GetDeviceName(char* name);

/**
 * @brief   Set the device name to flash
 * @param   name    Pointer to the name to write, null terminated
 * 
 * @return  Success
*/
bool STORAGE_SetMqttServer(const char* server);

/**
 * @brief   Get the MQTT server from flash
 * @param   name    Pointer to write the MQTT server, null terminated
 * 
 * @return  Success
*/
bool STORAGE_GetMqttServer(char* server);

#endif // __STORAGE_H__
