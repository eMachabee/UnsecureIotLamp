#include "email.h"

#include "lwip/apps/smtp.h"

#warning Set constants for email functionnality
#define SENDER_ADRESS "noreply@example.com"
#define SMTP_SERVER_ADDR "smtp.example.com"

static void my_smtp_result_fn(void *arg, u8_t smtp_result, u16_t srv_err, err_t err)
{
  printf("mail (%p) sent with results: 0x%02x, 0x%04x, 0x%08x\n", arg,
         smtp_result, srv_err, err);
}

void EMAIL_SendEmail(char* dest, char* body)
{
    if (dest == NULL || body == NULL) {
        return;
    }

    printf("[EMAIL] Destination: %s\n", dest);
    printf("[EMAIL] Body:\n%s\n", body);

    if (smtp_set_server_addr(SMTP_SERVER_ADDR) != ERR_OK) {
        printf("[EMAIL] Could not set server address\n");
    }
    if (smtp_set_auth(NULL, NULL) != ERR_OK) {
        printf("[EMAIL] Error setting auth\n");
    }
    smtp_set_server_port(25);
    if (smtp_send_mail(SENDER_ADRESS, dest, "Lamp notification", body, my_smtp_result_fn, NULL) != ERR_OK) {
        printf("[EMAIL] Error sending email with send_mail\n");
    }
}
