#ifndef FSM_CRUCE_H
#define FSM_CRUCE_H
#endif


#include "fsm.h"
//#include <wiringPi.h>

enum tt_estado {GR, YR, RG, RY};

fsm_t* fsm_new_cruce (void);

//Funciones de comprobacion
static int comprobarTS(fsm_t* this);
static int comprobarT(fsm_t* this);

//Funciones de transicion
static void setYR(fsm_t* this);
static void setRG(fsm_t* this);
static void setRY(fsm_t* this);
static void setGR(fsm_t* this);

//ISR function
void secundaria_isr (void);
void timer_isr (void);
int timer_finished(void);
void timer_start(int t);

//Definicion de los tiempos en segundos
#define Long	10
#define Short	2

portTickType* T_next;
int T;
int S;
