#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

#include "storage.h"

// CGI handler which is run when a request for /led.cgi is detected
const char *cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0], "led") == 0)
    {
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if (strcmp(pcValue[0], "0") == 0)
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            return "/commande.shtml";
        }
        else if (strcmp(pcValue[0], "1") == 0)
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            return "/commande.shtml";
        }
    }

    if (strcmp(pcParam[0], "nom") == 0)
    {
        if (strcmp(pcValue[0], "username") == 0)
        {
            if (strcmp(pcParam[1], "password") == 0)
            {
                if (strcmp(pcValue[1], "password") == 0)
                {
                    return "/commande.shtml";
                }
            }
        }
    }

    if (strcmp(pcParam[0], "name") == 0) {
        printf("New name: %s", pcValue[0]);
        STORAGE_SetDeviceName(pcValue[0]);
        STORAGE_Save(false);
        return "/commande.shtml";
    }

    // Send the index page back to the user
    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {// Html request for "/led.cgi" triggers cgi_handler
     "/led.cgi", cgi_led_handler,},
     {
        "/save.cgi", cgi_led_handler,
     }
};

void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 2);
}
