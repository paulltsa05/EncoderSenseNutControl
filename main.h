/* 
 * File:   main.h
 * Author: kpit
 *
 * Created on February 18, 2017, 4:46 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define NUTRUN_IDLE                 0
#define NUTRUN_SPEED_BELOWTHRES     1
#define NUTRUN_CHECK_MAXSPEED       2
#define NUTRUN_DETECTSHUTDOWN       3


//Interrupt Function callback

void EncINX_PulseRisingEvent(void);

void Timer0_tick10msecFunc(void);    

void Timer3_Overflow(void);
void Timer5_Overflow(void);

void EncA_PulseEdgeEvent(unsigned int capturedValue);
void EncB_PulseEdgeEvent(unsigned int capturedValue);

unsigned long millis(void); //arduino Like implementation for miilis())

void millisReset(void);//arduino Like implementation for miilis())



#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

