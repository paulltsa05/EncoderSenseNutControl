
#include "ModbusMaster.h"
#include "ModbusManager.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/eusart.h"

//*****************  Modbus DATA ******************************** 
#define MODBUSSLAVE_ID 5
//EPC-MCU : (Encoder Pulse Counter MCU is the Master) Data for Modbus Communication

//##########  Slave to Master  --- Register data array of 4 bytes at start address 0xE100
    
    //Holding register
    #define ADDR_MBreadParam        14 // offset 40001 + 14 = 40015 //Start address of 4 byte Pulse count Parameter data
    unsigned int MBreadSetNoOfPulse=0;
    
//##########   Master to slave  --- Binary data (represent as coil in MOdbus)
    //Holding register
    #define ADDR_MBwriteParam       20016 //Mater should write periodically
    unsigned int MBwriteCurrentNoOfPulse=0;
    
    //Single Coil
    #define ADDR_CoilToolOperating     00004
    unsigned char MBwriteToolOperating=0;

 
//***************************************************************************************

////*****************  Modbus DATA ******************************** 
//#define MODBUSSLAVE_ID 5
////EPC-MCU : (Encoder Pulse Counter MCU) Data for MOdbus Communication
////##########   Master to slave  --- Binary data (represent as coil in MOdbus)
//
//    #define ADDR_EncPulseCntResult 0xE002 //Coil at address 0xE002 (Manual Load Control)
//    bool EncPulseCntResult= 0; //writeSingleCoil(0xE002, EncPulseCntResult);
//    //EncPulseCntResult will trigger from Master On event of OK/NOK/TIMEOUT of EPC-MCU
//    
//    #define ADDR_EncPulseOpState     0xE004  //writeSingleRegister at address 0xE004
//    unsigned int EncPulseOpState =0; //writeSingleRegister(0xE004, EncPulseState);
//    
//
////##########  Slave to Master  --- Register data array of 4 bytes at start address 0xE100
//    
//    #define ADDR_MBreadParam          0xE100  //Start address of 4 byte Pulse count Parameter data
//    unsigned int MBreadNoOfPulseCnt=48; //readInputRegisters(0xE100, 2);
//    unsigned int MBreadPulseTimeoutSec=4; 
//    
//    #define ADDR_MBread_EPC_Ctrl     0xE200  //Start address of 2 byte EPC Sensing ON/OFF signal from PLC
//    unsigned int MBread_EPC_Ctrl;  //readInputRegisters(0xE200, 1);
//    
//    
////***************************************************************************************

void preTransmission_App()
{
    RS485_TXEN_RB6_SetHigh();// transmit enable RS485 driver chip
}

void postTransmission_App()
{
    RS485_TXEN_RB6_SetLow(); // transmit enable RS485 driver chip
    PIR1bits.RCIF=0;//flush receive buffer
}

void ModbusMasterSetup(void)
{
    RS485_TXEN_RB6_SetLow();//disable Tx RS485 at start-up
    EUSART_Initialize(); 
    
    ModbusMasterInit(MODBUSSLAVE_ID);// Initialise Modbus master with Slave ID
    
    preTransmission(preTransmission_App);//attach preTx callback on RS485 line
    postTransmission(postTransmission_App);//attach postTx callback on RS485 line
}

void MB_UpdateEPCParameter(void)//update MBreadSetNoOfPulse variable
{
    unsigned char result;
    result = readHoldingRegisters(ADDR_MBreadParam, 1);//blocking function with timeout 2sec
    if (result == ku8MBSuccess)
    {

        MBreadSetNoOfPulse=(unsigned int)getResponseBuffer(0x00);
        
    }
    MBreadSetNoOfPulse=0;
}


//void MB_SendMBwriteToolOperating(void)//SEND OR WRITE MBwriteToolOperating
//{
//    unsigned char result;
//    result=writeSingleCoil(ADDR_CoilToolOperating, MBwriteToolOperating);
//}
//
//void MB_SendResultNoOfPulse(void)//SEND OR WRITE Coil EncPulseCntResult
//{
//    unsigned char result;
//    result=writeSingleCoil(ADDR_MBwriteParam, MBwriteCurrentNoOfPulse);
//}
