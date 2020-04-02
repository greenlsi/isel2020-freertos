#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm_cruce.h"

int ticks = 0;
int max_ticks = Short;

fsm_t* fsm_new_cruce (void)
{
	static fsm_trans_t tt[] = {
		{GR,	comprobarTS,	YR,	setYR},
		{YR,	comprobarT,		RG,	setRG},
		{RG,	comprobarT,		RY,	setRY},
		{RY,	comprobarT,		GR,	setGR},
		{-1,	NULL,			-1,	NULL},
	};
	return fsm_new(tt);
}

//Funciones de comprobacion
static int comprobarTS(fsm_t* this) {	return (T && S);}

static int comprobarT(fsm_t* this)	{	return T; 		}

//Funciones de transicion
static void setYR(fsm_t* this)
{
	T = 0;
	S = 0;
	timer_start(Short);
}

static void setRG(fsm_t* this)
{
	T = 0;
	timer_start(Long);
}

static void setRY(fsm_t* this)
{
	T = 0;
	timer_start(Short);
}

static void setGR(fsm_t* this)
{
	T = 0;
	timer_start(Long);
}

//ISR function
void secundaria_isr (void) { S = 1; return;}
void timer_isr (void) {
	if (T==0)
		ticks = ticks + 1;
	if (ticks>max_ticks)
		T = 1;
	return;
}

void timer_start(int t) {
	max_ticks =  t;
	ticks = 0;
}
