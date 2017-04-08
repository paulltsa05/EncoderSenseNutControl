/* 
 * File:   ModbusMaster.h
 * Author: kpit
 *
 * Created on February 18, 2017, 2:34 AM
 */

#ifndef MODBUSMASTER_H
#define	MODBUSMASTER_H

#ifdef	__cplusplus
extern "C" {
#endif
/* _____PROJECT INCLUDES_____________________________________________________ */
// functions to calculate Modbus Application Data Unit CRC
#include "crc16.h"

// functions to manipulate words
#include "word.h"


/* 
Arduino class library for communicating with Modbus slaves over 
RS232/485 (via RTU protocol).
*/
    // Modbus function codes for bit access
#define   ku8MBReadCoils                                              0x01  //< Modbus function 0x01 Read Coils
#define   ku8MBReadDiscreteInputs                                     0x02  //< Modbus function 0x02 Read Discrete Inputs
#define   ku8MBWriteSingleCoil                                        0x05     //< Modbus function 0x05 Write Single Coil
#define   ku8MBWriteMultipleCoils                                     0x0F     //< Modbus function 0x0F Write Multiple Coils

    // Modbus function codes for 16 bit access
#define   ku8MBReadHoldingRegisters                                   0x03     //< Modbus function 0x03 Read Holding Registers
#define   ku8MBReadInputRegisters                                     0x04     //< Modbus function 0x04 Read Input Registers
#define   ku8MBWriteSingleRegister                                    0x06     //< Modbus function 0x06 Write Single Register
#define   ku8MBWriteMultipleRegisters                                 0x10     //< Modbus function 0x10 Write Multiple Registers
#define   ku8MBMaskWriteRegister                                      0x16     //< Modbus function 0x16 Mask Write Register
#define   ku8MBReadWriteMultipleRegisters                             0x17     //< Modbus function 0x17 Read Write Multiple Registers
    
    // Modbus timeout [milliseconds]
#define   ku16MBResponseTimeout                                      2000     //< Modbus timeout [milliseconds]
    
   


    // Modbus exception codes
    /**
    Modbus protocol illegal function exception.
    
    The function code received in the query is not an allowable action for
    the server (or slave). This may be because the function code is only
    applicable to newer devices, and was not implemented in the unit
    selected. It could also indicate that the server (or slave) is in the
    wrong state to process a request of this type, for example because it is
    unconfigured and is being asked to return register values.
    
    @ingroup constant
    */
#define   ku8MBIllegalFunction                                        0x01;

    /**
    Modbus protocol illegal data address exception.
    
    The data address received in the query is not an allowable address for 
    the server (or slave). More specifically, the combination of reference 
    number and transfer length is invalid. For a controller with 100 
    registers, the ADU addresses the first register as 0, and the last one 
    as 99. If a request is submitted with a starting register address of 96 
    and a quantity of registers of 4, then this request will successfully 
    operate (address-wise at least) on registers 96, 97, 98, 99. If a 
    request is submitted with a starting register address of 96 and a 
    quantity of registers of 5, then this request will fail with Exception 
    Code 0x02 "Illegal Data Address" since it attempts to operate on 
    registers 96, 97, 98, 99 and 100, and there is no register with address 
    100. 
    
    @ingroup constant
    */
#define   ku8MBIllegalDataAddress                                     0x02;
    
    /**
    Modbus protocol illegal data value exception.
    
    A value contained in the query data field is not an allowable value for 
    server (or slave). This indicates a fault in the structure of the 
    remainder of a complex request, such as that the implied length is 
    incorrect. It specifically does NOT mean that a data item submitted for 
    storage in a register has a value outside the expectation of the 
    application program, since the MODBUS protocol is unaware of the 
    significance of any particular value of any particular register.
    
    @ingroup constant
    */
#define   ku8MBIllegalDataValue                                       0x03;
    
    /**
    Modbus protocol slave device failure exception.
    
    An unrecoverable error occurred while the server (or slave) was
    attempting to perform the requested action.
    
    @ingroup constant
    */
#define   ku8MBSlaveDeviceFailure                                     0x04;

    // Class-defined success/exception codes
    /**
    ModbusMaster success.
    
    Modbus transaction was successful   the following checks were valid:
      - slave ID
      - function code
      - response code
      - data
      - CRC
      
    @ingroup constant
    */
#define   ku8MBSuccess                                                0x00  
    
    /**
    ModbusMaster invalid response slave ID exception.
    
    The slave ID in the response does not match that of the request.
    
    @ingroup constant
    */
#define   ku8MBInvalidSlaveID                                         0xE0  
    
    /**
    ModbusMaster invalid response function exception.
    
    The function code in the response does not match that of the request.
    
    @ingroup constant
    */
#define   ku8MBInvalidFunction                                        0xE1  
    
    /**
    ModbusMaster response timed out exception.
    
    The entire response was not received within the timeout period, 
    ModbusMaster::ku8MBResponseTimeout. 
    
    @ingroup constant
    */
#define   ku8MBResponseTimedOut                                       0xE2  
    
    /**
    ModbusMaster invalid response CRC exception.
    
    The CRC in the response does not match the one calculated.
    
    @ingroup constant
    */
#define   ku8MBInvalidCRC                                             0xE3  
  
#define   ku8MaxBufferSize                                            64     ///< size of response/transmit buffers    

    

    unsigned char  _u8MBSlave;                                         ///< Modbus slave (1..255) initialized in begin()
    unsigned int _u16ReadAddress;                                    ///< slave register from which to read
    unsigned int _u16ReadQty;                                        ///< quantity of words to read
    unsigned int _u16ResponseBuffer[ku8MaxBufferSize];               ///< buffer to store Modbus slave response; read via GetResponseBuffer()
    unsigned int _u16WriteAddress;                                   ///< slave register to which to write
    unsigned int _u16WriteQty;                                       ///< quantity of words to write
    unsigned int _u16TransmitBuffer[ku8MaxBufferSize];               ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
    unsigned int* txBuffer; // from Wire.h -- need to clean this up Rx
    unsigned char _u8TransmitBufferIndex;
    unsigned int u16TransmitBufferLength;
    unsigned int* rxBuffer; // from Wire.h -- need to clean this up Rx
    unsigned char _u8ResponseBufferIndex;
    unsigned char _u8ResponseBufferLength;
    

    // master function that conducts Modbus transactions
    unsigned char ModbusMasterTransaction(unsigned char u8MBFunction);

  
    // preTransmission callback function; gets called before writing a Modbus message
    void (*_preTransmission)();
    // postTransmission callback function; gets called after a Modbus message has been sent
    void (*_postTransmission)();

 //    void begin(unsigned char, Stream &serial);
    void ModbusMasterInit(unsigned char slave);

    void preTransmission(void (*)());
    void postTransmission(void (*)());   
    
    unsigned int getResponseBuffer(unsigned char);
    void     clearResponseBuffer();
    unsigned char  setTransmitBuffer(unsigned char, unsigned int);
    void     clearTransmitBuffer();
    
    void beginTransmission(unsigned int);
    unsigned char requestFrom(unsigned int, unsigned int);
 //   void sendbit(bool);
    void send8(unsigned char);
    void send16(unsigned int);
    void send32(unsigned long);
    unsigned char available(void);
    unsigned int receive(void);
    
    
    unsigned char  readCoils(unsigned int, unsigned int);
    unsigned char  readDiscreteInputs(unsigned int, unsigned int);
    unsigned char  readHoldingRegisters(unsigned int, unsigned int);
    unsigned char  readInputRegisters(unsigned int, unsigned char);
    unsigned char  writeSingleCoil(unsigned int, unsigned char);
    unsigned char  writeSingleRegister(unsigned int, unsigned int);
    unsigned char  writeMultipleCoils(unsigned int, unsigned int);
 //   unsigned char  writeMultipleCoils();
    unsigned char  writeMultipleRegisters(unsigned int, unsigned int);
 //   unsigned char  writeMultipleRegisters();
    unsigned char  maskWriteRegister(unsigned int, unsigned int, unsigned int);
    unsigned char  readWriteMultipleRegisters(unsigned int, unsigned int, unsigned int, unsigned int);
//    unsigned char  readWriteMultipleRegisters(unsigned int, unsigned int);    


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSMASTER_H */

