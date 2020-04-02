#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"
#include "fsm_cruce.h"

#define UMBRAL 5

fsm_t* fsm;

/* RAW MODE */
static struct termios orig_termios;
void disable_raw_mode()
{
  if (tcsetattr(0, TCSAFLUSH, &orig_termios) == -1)
    perror ("tcsetattr");
}
void enable_raw_mode()
{
  if (tcgetattr(0, &orig_termios) == -1) perror("tcgetattr");
  atexit(disable_raw_mode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(0, TCSAFLUSH, &raw) == -1) perror("tcsetattr");
}

void printxy (int x, int y, const char* txt);

int key_pressed (void) {
  struct timeval timeout = { 0, 0 };
  fd_set rd_fdset;
  FD_ZERO(&rd_fdset);
  FD_SET(0, &rd_fdset);
  return select(1, &rd_fdset, NULL, NULL, &timeout) > 0;
}
void key_process (int ch) {
  switch (ch) {
  case 's': secundaria_isr(); break;

  case 'q':
    //printf ("\e0\e[?25h");
    printf("\e[%d;0f",3*UMBRAL+2,0);
    exit(0); break;
  }
}

void refresh_screen(void) {

    switch (fsm->current_state) {
      case GR:
          printf("\e[%d;%df%c",UMBRAL+2,UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+4,UMBRAL+1,' ');
          printf("\033[0;32m\e[%d;%df%c",UMBRAL+6,UMBRAL+1,'G');
          printf("\033[0;31m\e[%d;%df%c",UMBRAL+2,3*UMBRAL+1,'R');
          printf("\e[%d;%df%c",UMBRAL+4,3*UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+6,3*UMBRAL+1,' '); break;
      case YR:
          printf("\e[%d;%df%c",UMBRAL+2,UMBRAL+1,' ');
          printf("\033[0;33m\e[%d;%df%c",UMBRAL+4,UMBRAL+1,'Y');
          printf("\e[%d;%df%c",UMBRAL+6,UMBRAL+1,' ');
          printf("\033[0;31m\e[%d;%df%c",UMBRAL+2,3*UMBRAL+1,'R');
          printf("\e[%d;%df%c",UMBRAL+4,3*UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+6,3*UMBRAL+1,' '); break;
      case RG:
          printf("\033[0;31m\e[%d;%df%c",UMBRAL+2,UMBRAL+1,'R');
          printf("\e[%d;%df%c",UMBRAL+4,UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+6,UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+2,3*UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+4,3*UMBRAL+1,' ');
          printf("\033[0;32m\e[%d;%df%c",UMBRAL+6,3*UMBRAL+1,'G'); break;
      case RY:
          printf("\033[0;31m\e[%d;%df%c",UMBRAL+2,UMBRAL+1,'R');
          printf("\e[%d;%df%c",UMBRAL+4,UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+6,UMBRAL+1,' ');
          printf("\e[%d;%df%c",UMBRAL+2,3*UMBRAL+1,' ');
          printf("\033[0;33m\e[%d;%df%c",UMBRAL+4,3*UMBRAL+1,'Y');
          printf("\e[%d;%df%c",UMBRAL+6,3*UMBRAL+1,' '); break;
    }

    if (S==1) {
      printf("\033[0m\e[%d;%df%c",3*UMBRAL,4*UMBRAL,'X');
    } else {
      printf("\033[0m\e[%d;%df%c",3*UMBRAL,4*UMBRAL,'_');
    }
}

static void reloj (void* ignore) {
    portTickType period =  1000 /portTICK_RATE_MS;

    char buf[256];

    portTickType last = xTaskGetTickCount();
    while (1) {
        struct tm* tm_info;
        time_t t = time(NULL);
        tm_info = localtime (&t);
        strftime (buf, 26, "%H:%M:%S ", tm_info);
        printxy (1, 1, buf);
        //printf("%s\n",buf );
        vTaskDelayUntil (&last, period);
    }
}

static void tlc (void* ignore) {

    portTickType period =  1000 /portTICK_RATE_MS;

    portTickType last = xTaskGetTickCount();

    while (1) {

        if (key_pressed())
          key_process(getchar());
        fsm_fire(fsm);
        timer_isr();
        vTaskDelayUntil (&last, period);
    }
}

static void screen (void* ignore) {

    portTickType period =  40 /portTICK_RATE_MS;
    portTickType last = xTaskGetTickCount();

    while (1) {
        refresh_screen();
        vTaskDelayUntil (&last, period);

    }
}

void user_init (void) {
    xTaskHandle task_reloj, task_tlc, task_screen;
    xTaskCreate (reloj, (const signed char*) "reloj", 2048, NULL, 1, &task_reloj);
    xTaskCreate (tlc, (const signed char*) "tlc", 2048, NULL, 1, &task_tlc);
    xTaskCreate (screen, (const signed char*) "screen", 2048, NULL, 1, &task_screen);
}

void setup_screen() {
	printf ("\e7\e[?25l");
	printf("\e[2J\e[%d;0f",UMBRAL-1);

  printf("\e[%d;%df%s",UMBRAL,UMBRAL-3,"PRINCIPAL SECUNDARIA");
  printf("\e[%d;%df%s",3*UMBRAL,UMBRAL-3,"COCHE SECUNDARIA:");

  for (int i=UMBRAL; i<UMBRAL+3; i++) {
    for (int j=UMBRAL+1; j<UMBRAL+8; j++) {
      printf("\e[%d;%df%c",j,i,'X');
      printf("\e[%d;%df%c",j,i+2*UMBRAL,'X');
    }
  }
  /*
	for (int i=0; i<LEN_X;i++) {
    printf("\e[%d;%df%c",UMBRAL+LEN_Y-1,UMBRAL+i,'X');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X/2,UMBRAL+i,'-');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X+1,UMBRAL+i,'X');
	}
  for (int i=LEN_X+ANCHO_Y+1; i<2*LEN_X+ANCHO_Y;i++) {
    printf("\e[%d;%df%c",UMBRAL+LEN_Y-1,UMBRAL+i,'X');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X/2,UMBRAL+i,'-');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X+1,UMBRAL+i,'X');
  }
  for (int i=LEN_X; i<LEN_X+ANCHO_Y+1;i++) {
    printf("\e[%d;%df%c",UMBRAL+LEN_Y-2,UMBRAL+i,'_');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X/2,UMBRAL+i,'-');
    printf("\e[%d;%df%c",UMBRAL+LEN_Y+ANCHO_X+1,UMBRAL+i,'_');
  }
  for (int i=0; i<LEN_Y;i++) {
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X-1,'X');
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X+ANCHO_Y+1,'X');
  }
  for (int i=LEN_Y+ANCHO_X+1; i<2*LEN_Y+ANCHO_X;i++) {
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X-1,'X');
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X+ANCHO_Y+1,'X');
  }
  for (int i=LEN_Y; i<LEN_Y+ANCHO_X+1;i++) {
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X-1,'|');
    printf("\e[%d;%df%c",UMBRAL+i,UMBRAL+LEN_X+ANCHO_Y+1,'|');
  }
  */
  printf("\e[%d;%df%c",1,0,' ');
  printf("\n");
}

void vApplicationIdleHook (void) {}
void vMainQueueSendPassed( void ) {}

int main (void) {

    enable_raw_mode();
    setup_screen();

    fsm = fsm_new_cruce();

    user_init();
    vTaskStartScheduler();
    return 1;
}
