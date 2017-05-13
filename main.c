/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15
        Device            :  PIC16F18344
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include "ModbusManager.h"
#include "LED_Ctrl.h"
#include "main.h"


/*
                         Main application
 */
unsigned int OldEncPulseOpState=0,EncPulseOpState=0;

//unsigned int Speed_rpm_pulseTime=0,Speed_rpm_pulsecnt=0,Speed_rpm=0;
//unsigned int PulseTime_10uSec=0,PulseCount=0;
//unsigned int TrackForward=0,TrackReverse=0;




unsigned long EncEdgeTimeIntervalOld=0, EncEdgeTimeInterval=0x00000000, EncEdgeMinTimeInterval=0xEFFFFFFF;
unsigned long EncEdgeMinTimeIntSumAvg=0;
unsigned long EncSpdredFact= 30;  //encoder speed reduction Factor, x/100=0.25 % reduce in speed to detect Nut run stop 

unsigned int NutRunLength = 0;//interms of number of encoder pulse.

unsigned char multipleTimecheck0=0;
unsigned char multipleTimecheck1=0;

signed int EncoderPosition=0, OldEncPos=0, EncoderPositionAtIndex=0;
signed char EncoderPositionOneCycle=0; //it count the position from start to one revolution only, for every rev it reset
unsigned char EncPulseError=0;
bool flagForFirstRevolution=0;

bool EncEdgeGotFlag=0;
bool ShutdownDetected=0;


unsigned char NutRunstate=NUTRUN_IDLE;
unsigned long IdleTimeOnShutOff=0;

unsigned long TimertickMsec=0;

unsigned int delayCount=0;
bool delayFlag=0;



void NutRunCycleReset(void);

void OnEncPulseEdgeEvent(uint16_t capturevalue);

void getEncoderState(void);

void EPC_StateMachineControlLoop();

void NutRunCycleStop(void);
void delayMsec(unsigned long);
void NutRunCycleStart(void);

unsigned char valuetocharHighNibble(unsigned char hexvalue);
unsigned char valuetocharLowNibble(unsigned char hexvalue);
unsigned char valtoasciichar(unsigned char hexvalue1);

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    TMR3_WriteTimer(0);//reset TMR3
    TMR3_StopTimer();//Stop TMR3
    TMR5_WriteTimer(0);//reset TMR5
    TMR5_StopTimer();//Stop TMR5
    
    ModbusMasterSetup();
    EncPulseOpState=IDLE;
    
    INLVLC = 0xFF;
    INLVLA = 0xFF;
  
    getEncoderState();
    OldEncPulseOpState=EncPulseOpState;
    NutRunCycleStart();
    
//  LED_RED_RC0_SetLow();
//  LED_GREEN_RC1_SetLow();
//  LED_BLUE_RC2_SetLow();

        // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    LedONStartConfig(LED_RED, 1, 1);
    while(LedONStatusBusy());//wait for LED blink     
//    LedONStartConfig(LED_GREEN, 1, 1);
//    while(LedONStatusBusy());//wait for LED blink 
//    LedONStartConfig(LED_BLUE, 1, 1);
//    while(LedONStatusBusy());//wait for LED blink    
    
    RS485_TXEN_RB6_SetHigh();
    
    //Test RS485
 //   EUSART_Write('>');
 //   EUSART_Write('>');
    
    MB_UpdateEPCParameter();

    while (1)
    {
//        if(!ENC_B_RC4_GetValue())
//            PulseCount++;
//            
//            
//        if(ENC_A_RC5_GetValue() == 1)
//        {    
//            PulseCount++;
//        }
//        else
//            PulseCount++;
        // Add your application code
        MB_UpdateEPCParameter();
        delayMsec(2000);
 //       EPC_StateMachineControlLoop();
    }
}

void EPC_StateMachineControlLoop() //call the statemachine in main loop
{
       
    if((NutRunstate == NUTRUN_SPEED_BELOWTHRES) || (NutRunstate == NUTRUN_CHECK_MAXSPEED))
    {   //Track continuos pulse edge if no pulse for  1000msec, then exit state to NUTRUN_IDLE
        //Edge time interval ticks at 0.5 usec
        if((EncEdgeGotFlag == 0) && (EncEdgeTimeInterval > 2000000))
        {
            NutRunstate= NUTRUN_IDLE;
            NutRunCycleReset();
            EUSART_Write('1');
            LED_GREEN_RC1_SetLow();
            
        }    
    }    
    
    if(ShutdownDetected == 1)
    {

        ShutdownDetected=0;
        LED_GREEN_RC1_SetLow();
        LedONStartConfig(LED_RED, 2, 15);
//        EUSART_Write('\r');
//        EUSART_Write('\n');
//        EUSART_Write('N');
//        EUSART_Write('u');
//        EUSART_Write('t');
//        EUSART_Write(' ');
//        EUSART_Write('R');
//        EUSART_Write('u');
//        EUSART_Write('N');
//        EUSART_Write('=');
//        EUSART_Write('0');
//        EUSART_Write('x');
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(NutRunLength>>8) & 0x00FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(NutRunLength>>8) & 0x00FF)));
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(NutRunLength) & 0x00FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(NutRunLength) & 0x00FF)));  
//        EUSART_Write(',');
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned long)(EncEdgeMinTimeInterval>>24) & 0x000000FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned long)(EncEdgeMinTimeInterval>>24) & 0x000000FF)));
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned long)(EncEdgeMinTimeInterval>>16) & 0x000000FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned long)(EncEdgeMinTimeInterval>>16) & 0x000000FF)));
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(EncEdgeMinTimeInterval>>8) & 0x00FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(EncEdgeMinTimeInterval>>8) & 0x00FF)));
//        EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(EncEdgeMinTimeInterval) & 0x00FF)));
//        EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(EncEdgeMinTimeInterval) & 0x00FF)));  
//
//        EUSART_Write('\r');
//        EUSART_Write('\n');

 
    }
    
    if(EncEdgeGotFlag==1)//trigger this on background only when rise and fall edge of pulse of enc A and B arrive.
    {
        EncEdgeGotFlag=0;
        
//            EUSART_Write('[');
//            EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned long)(EncEdgeTimeIntervalOld>>16) & 0x000000FF)));
//            EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned long)(EncEdgeTimeIntervalOld>>16) & 0x000000FF)));
//            EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(EncEdgeTimeIntervalOld>>8) & 0x000000FF)));
//            EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(EncEdgeTimeIntervalOld>>8) & 0x000000FF)));
//            EUSART_Write(valuetocharHighNibble((unsigned char)((unsigned int)(EncEdgeTimeIntervalOld) & 0x000000FF)));
//            EUSART_Write(valuetocharLowNibble((unsigned char)((unsigned int)(EncEdgeTimeIntervalOld) & 0x000000FF)));        
//            EUSART_Write(']');

  

//        switch (EncPulseOpState)//will executed only on transition of Pulse Edge
//        {
//            case EnALoEnBLo :       EUSART_Write('0');
//                                    break;
//            case EnAHiEnBLo :       EUSART_Write('1');
//                                    break;
//            case EnAHiEnBHi :       EUSART_Write('2');
//                                    break;
//            case EnALoEnBHi :       EUSART_Write('3');    
//                                    break;
//            case EncError   :       EUSART_Write('F');
//                                    break;
//            default         :       break;    
//        }
  
    }   
    
}

//Interrupt trigger function call
void EncA_PulseEdgeEvent(uint16_t capturedValue)// ISR callback function
{
    BUZZER_RC3_SetHigh();//for debug
    // Disable the CCP1 interrupt
    PIE4bits.CCP1IE = 0;
    // Disable the CCP2 interrupt
    PIE4bits.CCP2IE = 0;    
//    OnEncPulseEdgeEvent(capturedValue);
//    // Disable the CCP1 interrupt
    PIE4bits.CCP1IE = 1;
//    // Enable the CCP2 interrupt
//    PIE4bits.CCP2IE = 1;
    BUZZER_RC3_SetLow();//for debug
    
}
void EncB_PulseEdgeEvent(uint16_t capturedValue)// ISR callback function
{
    BUZZER_RC3_SetHigh();//for debug
    // Disable the CCP1 interrupt
 //   PIE4bits.CCP1IE = 0;
    // Disable the CCP2 interrupt
    PIE4bits.CCP2IE = 0;    
    
 //   OnEncPulseEdgeEvent(capturedValue);
    // Enable the CCP1 interrupt
//    PIE4bits.CCP1IE = 1;
    // Disable the CCP2 interrupt
 //   PIE4bits.CCP2IE = 0;    
    BUZZER_RC3_SetLow();//for debug
}

void OnEncPulseEdgeEvent(uint16_t capturevalue)//executed on ISR on each edges of encoder A and B
{
    
    TMR3_WriteTimer(0);
    TMR3_StartTimer();//Start if not started TMR3    
//    TMR5_WriteTimer(0);
//    TMR5_StartTimer();//Start if not started TMR5 

    EncEdgeTimeInterval= EncEdgeTimeInterval + capturevalue;
    
    switch(NutRunstate)
    {
        case NUTRUN_IDLE:
            if((EncoderPosition>32)||(EncoderPosition < -32))//check one revolution
            //            if((EncoderPosition>64)||(EncoderPosition < -64))//check one revolution
            {
                NutRunstate= NUTRUN_SPEED_BELOWTHRES;
                EUSART_Write('B');
            }
            break;
            
        case NUTRUN_SPEED_BELOWTHRES:
            if(EncEdgeTimeInterval < 6000) // edge to edge time is 6000 *0.5usec= 3.0 msec i.e spped > 600rpm = 10 rps = 32p pulse per rev
            {
                NutRunstate= NUTRUN_CHECK_MAXSPEED;
                multipleTimecheck0=0;
                EncEdgeMinTimeIntSumAvg=0;
                multipleTimecheck1=0;
                EncEdgeMinTimeInterval=0xEFFFFFFF;
                EUSART_Write('C');
                LED_GREEN_RC1_SetHigh();//
            }
            break;
            
        case NUTRUN_CHECK_MAXSPEED:
                
            if(EncEdgeTimeInterval < EncEdgeMinTimeInterval)// Note Max speed point
            {    
                multipleTimecheck0++;//check consecutive occurence
                switch(multipleTimecheck0)
                {
                    case 1: OldEncPos=EncoderPosition;
                            EncEdgeMinTimeIntSumAvg+=EncEdgeTimeInterval;
                        break;
                    case 2: EncEdgeMinTimeIntSumAvg+=EncEdgeTimeInterval;
                        break;
                    case 3: EncEdgeMinTimeIntSumAvg+=EncEdgeTimeInterval;
                        break;    
                    case 4: EncEdgeMinTimeIntSumAvg+=EncEdgeTimeInterval;
                        break;    
                    case 5: EncEdgeMinTimeIntSumAvg+=EncEdgeTimeInterval;
                            if((EncoderPosition-OldEncPos) == (multipleTimecheck0-1))
                            {
                                EncEdgeMinTimeInterval= EncEdgeMinTimeIntSumAvg/multipleTimecheck0; //record smallest
                            }
                            multipleTimecheck0=0;
                            EncEdgeMinTimeIntSumAvg=0;
                            break;
                    default:break;
                }        
                
                //EUSART_Write('K');
            }
            else if(EncEdgeTimeInterval >= (EncEdgeMinTimeInterval + ((EncEdgeMinTimeInterval*EncSpdredFact)/100)))
            {   //Look for a reduction in speed from maximum speed by some factor define by EncSpdredFact
                //look for multiple consecutive event only
                
                multipleTimecheck1++;
                switch(multipleTimecheck1)
                {
                    case 1: OldEncPos=EncoderPosition;
                        break;
                    case 2: 
                        break;
                    case 3: 
                        break;
                    case 4: 
                        break;    
                    case 5: if((EncoderPosition-OldEncPos) == (multipleTimecheck1-1))
                            {
                                ShutdownDetected = 1;
                                NutRunCycleStop();

                                if(EncoderPosition < 0)
                                    NutRunLength = -EncoderPosition;//interms of number of encoder pulse.
                                else
                                    NutRunLength = EncoderPosition;

                                IdleTimeOnShutOff = millis() + 2100;//wait in detect shutdown state for 2000 sec
                                EUSART_Write('D');
                                NutRunstate = NUTRUN_DETECTSHUTDOWN;
                            }
                            multipleTimecheck1=0;
                            break;
                    default:break;
                }
                

            }
            break; 
            
        case NUTRUN_DETECTSHUTDOWN:
            if(millis() > IdleTimeOnShutOff )//stay in this mode for sometime
            {
                NutRunstate = NUTRUN_IDLE;
                EncoderPosition = 0;
                EncEdgeMinTimeInterval= 0xEFFFFFFF;
                EUSART_Write('A');
                NutRunCycleStart();
            }    
            break;
            
        default : break;
    }
    
    EncEdgeTimeIntervalOld= EncEdgeTimeInterval;
    EncEdgeTimeInterval=0;
    
    EncEdgeGotFlag=1;
    
    OldEncPulseOpState=EncPulseOpState;
    getEncoderState();// update current value of EncPulseOpState
    
    if((OldEncPulseOpState == EnALoEnBxx) && (EncPulseOpState == EnAHiEnBxx))//rising edge
    {
        if(ENC_B_RC4_GetValue()) //REVERSE DIRECTION DETECTED
        {
            EncoderPosition--;
            EncoderPositionOneCycle--;                        
        }
        else//FORWARD DIRECTION DETECTED
        {
            EncoderPosition++;
            EncoderPositionOneCycle++;            
        }    
    }
    else if((OldEncPulseOpState == EnAHiEnBxx) && (EncPulseOpState == EnALoEnBxx))//falling edge
    {
        if(ENC_B_RC4_GetValue())//FORWARD DIRECTION DETECTED
        {
            EncoderPosition++;
            EncoderPositionOneCycle++;                        
        }
        else //REVERSE DIRECTION DETECTED
        {
            EncoderPosition--;
            EncoderPositionOneCycle--;                        
        }
    }    

              

//    switch (EncPulseOpState)//will executed only on transition of Pulse Edge
//        {
//            case EnALoEnBLo :       
//                                    if(OldEncPulseOpState == EnALoEnBHi )
//                                    {  
//                                        EncoderPosition++;
//                                        EncoderPositionOneCycle++;
//                                    }
//                                    else if(OldEncPulseOpState == EnAHiEnBLo )
//                                    {    
//                                        EncoderPosition--;
//                                        EncoderPositionOneCycle--;
//                                    }
//                                    
//                                
//                            break;
//            case EnAHiEnBLo :       if(OldEncPulseOpState == EnALoEnBLo )
//                                    {    
//                                        EncoderPosition++;
//                                        EncoderPositionOneCycle++;
//                                    }
//                                    else if(OldEncPulseOpState == EnAHiEnBHi )
//                                    {   
//                                        EncoderPosition--;
//                                        EncoderPositionOneCycle--;
//                                    }
//                                    
//                            break;
//            case EnAHiEnBHi :       if(OldEncPulseOpState == EnAHiEnBLo )
//                                    {    
//                                        EncoderPosition++;
//                                        EncoderPositionOneCycle++;
//                                    }
//                                    else if(OldEncPulseOpState == EnALoEnBHi )
//                                    {    
//                                        EncoderPosition--;
//                                        EncoderPositionOneCycle--;
//                                    }
//                                               
//
//                                
//                            break;
//            case EnALoEnBHi :       if(OldEncPulseOpState == EnAHiEnBHi )
//                                    {    
//                                        EncoderPosition++;
//                                        EncoderPositionOneCycle++;
//                                    }
//                                    else if(OldEncPulseOpState == EnALoEnBLo )
//                                    {    
//                                        EncoderPosition--;
//                                        EncoderPositionOneCycle--;
//                                    }
//                                    
//                            break;
//            default:        break;    
//        }    
    
}



void NutRunCycleReset(void)
{
    EncEdgeMinTimeInterval= 0xEFFFFFFF; // For max speed detection
    EncEdgeTimeInterval= 0x00000000;
    
    EncoderPosition= 0; // 
    //EncoderPositionOneCycle=0;

    NutRunLength = 0; // interms of number of encoder pulse.
 
}

void NutRunCycleStart(void)
{
    NutRunCycleReset();
   
    TMR3_WriteTimer(0);
    TMR3_StartTimer();//Start if not started TMR3    
    PIE3bits.TMR3IE = 1;
  //  TMR5_WriteTimer(0);
  //  TMR5_StartTimer();//Start if not started TMR5 
  //  PIE4bits.TMR5IE = 1;

}

void NutRunCycleStop(void)
{
    TMR3_WriteTimer(0);
    TMR3_StopTimer();//Start if not started TMR3    
    PIE3bits.TMR3IE = 0;
    TMR5_WriteTimer(0);
    TMR5_StopTimer();//Start if not started TMR5 
    PIE4bits.TMR5IE = 0;
}

void getEncoderState(void)
{
    if(ENC_A_RC5_GetValue())// rising uses timer 3 & CCP1 Enc A
    {
            //EncPulseOpState= EnAHiEnBLo;
            EncPulseOpState= EnAHiEnBxx;
            //EUSART_Write('2');
    
    }
    else
    {
            //EncPulseOpState= EnALoEnBLo;
            EncPulseOpState= EnALoEnBxx;
            //EUSART_Write('0');
    }
    
//    if(ENC_A_RC5_GetValue())// rising uses timer 3 & CCP1 Enc A
//    {
//        if(ENC_B_RC4_GetValue())// rising uses timer 5 & CCP2 Enc B
//        {
//            
//            EncPulseOpState= EnAHiEnBHi;
//            //EUSART_Write('3');
//        } 
//        else
//        {
//            EncPulseOpState= EnAHiEnBLo;
//            //EUSART_Write('2');
//        }    
//    }
//    else
//    {
//        if(ENC_B_RC4_GetValue())// rising uses timer 5 & CCP2
//        {
//            EncPulseOpState= EnALoEnBHi;
//            //EUSART_Write('1');
//        } 
//        else
//        {
//            EncPulseOpState= EnALoEnBLo;
//            //EUSART_Write('0');
//        }       
//    }
}


void EncINX_PulseRisingEvent(void)
{   //on rising only 
    EncoderPositionAtIndex = EncoderPosition;
    
    if(EncoderPositionOneCycle < 0)
        EncoderPositionOneCycle=-EncoderPositionOneCycle;//make all neg to positive
            
    if(flagForFirstRevolution == 1 && EncoderPositionOneCycle > 5) 
        //check 5 so that pending ISR issue due to INX ISR is highest priority 
    {    
        if(EncoderPositionOneCycle < 32)
        {       
            EncPulseError=1; //Encoder Fault Code 1: Missing pulse detected in a revolution
//            EUSART_Write('X');
//            EUSART_Write(valuetocharHighNibble((unsigned char)((EncoderPositionOneCycle) & 0x00FF)));
//            EUSART_Write(valuetocharLowNibble((unsigned char)((EncoderPositionOneCycle) & 0x00FF)));
        }
        else if(EncoderPositionOneCycle > 32)
        {
            EncPulseError=2; //Encoder Fault Code 2: More pulse detected in a revolution
//            EUSART_Write('Y');
//            EUSART_Write(valuetocharHighNibble((unsigned char)((EncoderPositionOneCycle) & 0x00FF)));
//            EUSART_Write(valuetocharLowNibble((unsigned char)((EncoderPositionOneCycle) & 0x00FF)));    
        }    
//        if((EncoderPositionOneCycle < 64) && (EncoderPositionOneCycle>0))
//        {       
//            EncPulseError=1; 
//            EUSART_Write('X');
//        }
//        else if((EncoderPositionOneCycle > -64) && (EncoderPositionOneCycle<0))
//        {
//            EncPulseError=1;
//            EUSART_Write('X');
//        }
//        else if((EncoderPositionOneCycle > 64) || (EncoderPositionOneCycle < -64))
//        {
//            EUSART_Write('Y');
//        }    
    } 
    
    flagForFirstRevolution=1; // to skip First revolution on device start or MCU On
        
    //EUSART_Write('*');
    EncoderPositionOneCycle=0;
}


void Timer3_Overflow(void)
{
    EncEdgeTimeInterval= EncEdgeTimeInterval + 0x0000FFFF;
    if(EncEdgeTimeInterval >0xEFFFFFFF)
        EncEdgeTimeInterval=0xEFFFFFFF;
    
}
void Timer5_Overflow(void)
{

}

void Timer0_tick10msecFunc(void)//call every 10msec but due to MCC bug it works at 5 msec
{
    LedON_Control_10msec();
    //LED_RED_RC0_Toggle();
    TimertickMsec= TimertickMsec+5;
    //EUSART_Write('U');
    if(delayFlag==1)
        delayCount+=5;
}

unsigned long millis(void) //arduino Like implementation for miilis())
{
    return TimertickMsec;
}

void millisReset(void)//arduino Like implementation for miilis())
{
    TimertickMsec=0;
}

void delayMsec(unsigned long timems)
{
    delayCount=0;
    delayFlag=1;
    while(delayCount < timems);
    delayFlag=0;
}

unsigned char valuetocharHighNibble(unsigned char hexvalue)
{
    unsigned char hexval;
    hexval=(unsigned char)((unsigned char)(hexvalue>>4) & 0x0F);
    hexval= valtoasciichar(hexval);
    return hexval;
    
}

unsigned char valuetocharLowNibble(unsigned char hexvalue)
{
    unsigned char hexval;
    hexval=(unsigned char)((unsigned char)(hexvalue) & 0x0F);
    hexval= valtoasciichar(hexval); 
    return hexval;
}


unsigned char valtoasciichar(unsigned char hexvalue1)
{   
    unsigned char hexval='0';
    switch((unsigned char)hexvalue1)
    {
        case 0x00: hexval= '0'; break;
        case 0x01: hexval= '1'; break;
        case 0x02: hexval= '2'; break;
        case 0x03: hexval= '3'; break;
        case 0x04: hexval= '4'; break;
        case 0x05: hexval= '5'; break;
        case 0x06: hexval= '6'; break;
        case 0x07: hexval= '7'; break;
        case 0x08: hexval= '8'; break;
        case 0x09: hexval= '9'; break;
        case 0x0A: hexval= 'A'; break;
        case 0x0B: hexval= 'B'; break;
        case 0x0C: hexval= 'C'; break;
        case 0x0D: hexval= 'D'; break;
        case 0x0E: hexval= 'E'; break;
        case 0x0F: hexval= 'F'; break;
        default : break;             
    }
    return hexval;
}


/**
 End of File
*/