/* 
 * File:   ModbusManager.h
 * Author: kpit
 *
 * Created on February 18, 2017, 4:02 PM
 */

#ifndef MODBUSMANAGER_H
#define	MODBUSMANAGER_H

#ifdef	__cplusplus
extern "C" {
    
#endif

//******* State of EncPulseOpState
#define IDLE            0
    
#define EnALoEnBLo      1
#define EnAHiEnBLo      2
#define EnAHiEnBHi      3
#define EnALoEnBHi      4
    
#define EncError         16
    

    

//data send to Modbus Slave    
unsigned char EncPulseCntResult;
unsigned int EncPulseOpState;

//data read from Modbus Slave
unsigned int MBreadNoOfPulseCnt;
unsigned int MBreadPulseTimeoutSec;
unsigned int MBread_EPC_Ctrl;


void ModbusMasterSetup(void);
void MB_UpdateEPCParameter(void);//update MBreadNoOfPulseCnt and MBreadPulseTimeoutSec
void MB_getEPC_PLC_Control(void);//update MBread_EPC_Ctrl
void MB_SendEncPulseOpState(void);//SEND OR WRITE EncPulseOpState
void MB_SendEncPulseCntResult(void);//SEND OR WRITE Coil EncPulseCntResult


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSMANAGER_H */

