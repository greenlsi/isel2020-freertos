#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"

void printxy (int x, int y, const char* txt);

static void
reloj (void* ignore)
{
    portTickType period =  1000 /portTICK_RATE_MS;

    char buf[256];

    portTickType last = xTaskGetTickCount();
    while (1) {
        struct tm* tm_info;
        time_t t = time(NULL);
        tm_info = localtime (&t);
        strftime (buf, 26, "%H:%M:%S ", tm_info);
        printxy (1, 1, buf);
        vTaskDelayUntil (&last, period);
    }
}

void
user_init (void)
{
    xTaskHandle task_reloj;
    xTaskCreate (reloj, (const signed char*) "reloj", 2048, NULL, 1, &task_reloj);
}

void vApplicationIdleHook (void) {}
void vMainQueueSendPassed( void ) {}

int
main (void)
{
    user_init();
    vTaskStartScheduler();
    return 1;
}
