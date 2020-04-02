#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"

#define GPIO_BUTTON 14
#define GPIO_PIR 12
#define GPIO_ALARM 2

fsm_t* fsm_new_alarm (int* validp, int pir, int alarm);
fsm_t* fsm_new_code (int* validp, int button);

static int valid_code = 0;

void printxy (int x, int y, const char* txt);

static void
reloj (void* ignore)
{
    portTickType period =  250 /portTICK_RATE_MS;
    portTickType last = xTaskGetTickCount();
    while (1) {
        printxy(1,1,"hola");
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
