
#include "ModbusMaster.h"
#include "ModbusManager.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/eusart.h"


//*****************  Modbus DATA ******************************** 
#define MODBUSSLAVE_ID 5
//EPC-MCU : (Encoder Pulse Counter MCU) Data for MOdbus Communication
//##########   Master to slave  --- Binary data (represent as coil in MOdbus)

    #define ADDR_EncPulseCntResult 0xE002 //Coil at address 0xE002 (Manual Load Control)
    bool EncPulseCntResult= 0; //writeSingleCoil(0xE002, EncPulseCntResult);
    //EncPulseCntResult will trigger from Master On event of OK/NOK/TIMEOUT of EPC-MCU
    
    #define ADDR_EncPulseOpState     0xE004  //writeSingleRegister at address 0xE004
    unsigned int EncPulseOpState =0; //writeSingleRegister(0xE004, EncPulseState);
    

//##########  Slave to Master  --- Register data array of 4 bytes at start address 0xE100
    
    #define ADDR_MBreadParam          0xE100  //Start address of 4 byte Pulse count Parameter data
    unsigned int MBreadNoOfPulseCnt=48; //readInputRegisters(0xE100, 2);
    unsigned int MBreadPulseTimeoutSec=4; 
    
    #define ADDR_MBread_EPC_Ctrl     0xE200  //Start address of 2 byte EPC Sensing ON/OFF signal from PLC
    unsigned int MBread_EPC_Ctrl;  //readInputRegisters(0xE200, 1);
    
    
//***************************************************************************************

void preTransmission_App()
{
    RS485_TXEN_RB6_SetHigh();
}

void postTransmission_App()
{
    RS485_TXEN_RB6_SetLow();
}

void ModbusMasterSetup(void)
{
    RS485_TXEN_RB6_SetLow();//disable Tx RS485 at start-up
    EUSART_Initialize(); 
    
    ModbusMasterInit(MODBUSSLAVE_ID);// Initialise Modbus master with Slave ID
    
    preTransmission(preTransmission_App);//attach preTx callback on RS485 line
    postTransmission(postTransmission_App);//attach postTx callback on RS485 line
}

void MB_UpdateEPCParameter(void)//update MBreadNoOfPulseCnt and MBreadPulseTimeoutSec
{
    unsigned char result;
    result = readInputRegisters(ADDR_MBreadParam, 2);//blocking function with timeout 2sec
    if (result == ku8MBSuccess)
    {

        MBreadNoOfPulseCnt=(unsigned int)getResponseBuffer(0x00);
        MBreadPulseTimeoutSec=(unsigned int)getResponseBuffer(0x01);

    }
}
void MB_getEPC_PLC_Control(void)//update MBread_EPC_Ctrl
{
    unsigned char result;
    result = readInputRegisters(ADDR_MBread_EPC_Ctrl, 1);//blocking function with timeout 2sec
    if (result == ku8MBSuccess)
    {

        MBread_EPC_Ctrl=(unsigned int)getResponseBuffer(0x00);
    }
}

void MB_SendEncPulseOpState(void)//SEND OR WRITE EncPulseOpState
{
    unsigned char result;
    result=writeSingleRegister(ADDR_EncPulseOpState, EncPulseOpState);
}

void MB_SendEncPulseCntResult(void)//SEND OR WRITE Coil EncPulseCntResult
{
    unsigned char result;
    result=writeSingleCoil(ADDR_EncPulseCntResult, EncPulseCntResult);
}
