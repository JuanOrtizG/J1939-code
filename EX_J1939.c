////////////////////////////////////////////////////////////////////////////////
////                               EX_J1939.c                               ////
////                                                                        ////
//// Example of CCS's J1939 driver.  This example was tested using the CCS  ////
//// CAN Bus 24 Development kit.                                            ////
////                                                                        ////
//// This example will send a message once every second commanding Node B   ////
//// to toggle it's LED once every second.  Requires that EX_J1939B.c be    ////
//// programmed onto Node B of the development kit.  Also pressing the push ////
//// will cause Node A to send out a Global Address Request, causing all    ////
//// device to respond with their claimed address.  Which will be displayed ////
//// over RS232.                                                            ////
////                                                                        ////
//// This example will work with the PCM, PCH and PCD compilers.  The       ////
//// is written to work with the PCD and PCH compiler.  Change the device,  ////
//// clock, RS232 settings, PIN defines and tick timer setting for hardware ////
//// if needed.                                                             ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////

#if defined(__PCD__)
#include <24HJ256GP610.h>
#elif defined(__PCH__)
#include "18F4580.h"
#endif
#fuses NOWDT
#use delay(crystal=16MHz)
#use rs232(UART1,baud=9600)

#define LED_PIN      PIN_B1
#define PUSH_BUTTON  PIN_A4

#include <stdint.h>

void InitJ1939Address(void);
void InitJ1939Name(void);

//////////////////////////////////////////////////////////////////////////////// Tick Timer
#define TICKS_PER_SECOND 1000

typedef uint32_t TICK_TYPE;
static TICK_TYPE TimerTick;

TICK_TYPE GetTick(void)
{
   return(TimerTick);
}

TICK_TYPE GetTickDifference(TICK_TYPE current,TICK_TYPE previous)
{
   return(current - previous);
}

#INT_TIMER2
void tick_time_isr(void)
{
   TimerTick++;
}

//////////////////////////////////////////////////////////////////////////////// J1939 Settings

//Following Macros used to initialize unit's J1939 Address and Name - Required
#define J1939InitAddress()    InitJ1939Address()
#define J1939InitName()       InitJ1939Name()

//Following defines what the CAN's baud rate it, not required defaults to 250Kbit 
//only necessary if using non standard baud rate.  Was changed to 125Kbit to
//work on CCS CAN Bus and CAN Bus 24 development kit.

//#define  Set_1000K_Baud TRUE
//#define  Set_500K_Baud TRUE
#define  Set_250K_Baud TRUE
//#define  Set_200K_Baud True
//#define  Set_125K_Baud True
//#define J1939_BAUD_RATE    125000 // del original para 20Mhz

//Following defines sets up the CAN baud rate, not required if using 250Kbit or 
//500Kbit and clock rates of 8, 16, 20, 32 or 40MHz.
#if defined(__PCD__)
#define CAN_BRG_PRESCALAR           4
#define CAN_BRG_PHASE_SEGMENT_1     2
#define CAN_BRG_PHASE_SEGMENT_2     2
#define CAN_BRG_SYNCH_JUMP_WIDTH    0
#define CAN_BRG_PROPAGATION_TIME    0
#elif defined(__PCH__)
#define CAN_BRG_PRESCALAR           4
#define CAN_BRG_PHASE_SEGMENT_1     6
#define CAN_BRG_PHASE_SEGMENT_2     6
#define CAN_BRG_SYNCH_JUMP_WIDTH    0
#define CAN_BRG_PROPAGATION_TIME    0
#endif

//Following defines/macros used to associate your tick timer to J1939 tick timer
// defines/macro's - Required
#define J1939GetTick()                 GetTick()
#define J1939GetTickDifference(a,b)    GetTickDifference(a,b)
#define J1939_TICKS_PER_SECOND         TICKS_PER_SECOND
#define J1939_TICK_TYPE                TICK_TYPE

//Include the J1939 driver
#include "j1939.c"

//Defines for J1939 Commands used in this example
#define LED_ON       50
#define LED_OFF      51
#define LED_TOGGLE   52

//Define for other Node's J1939 address used in this example
#define OTHER_NODE_ADDRESS    129

//Function used to initialize this unit's J1939 Address
void InitJ1939Address(void)
{
   g_MyJ1939Address = 128;
}

//Function used to initialize this unit's J1939 Name
void InitJ1939Name(void)
{
   g_J1939Name[0] = 0;
   g_J1939Name[1] = 0;
   g_J1939Name[2] = 0;
   g_J1939Name[3] = 0;
   g_J1939Name[4] = 0;
   g_J1939Name[5] = 0;
   g_J1939Name[6] = 0;
   g_J1939Name[7] = 128;
}


//IMPLEMENTACIÓN DE LAS FUNCIONES PARA SPN Y PGN
//PARAMETROS PGN
#define PGN_DASH_DISPLAY                   0xFEFC
#DEFINE PGN_ELECTRONIC_ENGINE_CONTROLLER_1 0XF004
#DEFINE PGN_FUEL_ECONOMY                   0XFEF2
#DEFINE PGN_ENGINE_TEMPERATURE             0XFEEE


//PARAMETROS SPN

#DEFINE SPN_ENGINE_THROTTLE_POSITION      51
#DEFINE SPN_FUEL_LEVEL_1                  96
#DEFINE SPN_ENGINE_COOLANT_TEMPERATURE    110
#DEFINE SPN_ENGINE_FUEL_TEMPERATURE_1     174
#DEFINE SPN_ENGINE_OIL_TEMPERATURE_1      175
#DEFINE SPN_ENGINE_FUEL_RATE              183
#DEFINE SPN_ENGINE_SPEED                  190





void lecturaDelParametro(int16 pgn, int spn, int16* dato)
{  
   
   switch (pgn)
   {
      case PGN_DASH_DISPLAY: 
            switch (spn)
            {
               case SPN_FUEL_LEVEL_1:  *dato = SPN_FUEL_LEVEL_1; break;
            }
            break;
            //FIN DE PGN_DASH_DISPLAY
      case PGN_ELECTRONIC_ENGINE_CONTROLLER_1: 
               
            break;
      case PGN_FUEL_ECONOMY: 
            switch(spn)
            {
               case SPN_ENGINE_FUEL_RATE: *dato = SPN_ENGINE_FUEL_RATE; break; 
            }
           break;
      case PGN_ENGINE_TEMPERATURE: 
           switch (spn)
           {
            case SPN_ENGINE_COOLANT_TEMPERATURE:   *dato = SPN_ENGINE_COOLANT_TEMPERATURE; break;
            case SPN_ENGINE_FUEL_TEMPERATURE_1:    *dato = SPN_ENGINE_FUEL_TEMPERATURE_1;  break;
            case SPN_ENGINE_OIL_TEMPERATURE_1:     *dato = SPN_ENGINE_OIL_TEMPERATURE_1;   break;
           }
            break;
      
      default: *dato = 0; break;
      
   }
}

// FIN DEL BLOQUE DE LAS FUNCIONES SPN Y PGN


//J1939 Task function for this example
void J1939Task(void)
{  
   int16 dato; 
   int16 pgn;
   uint8_t i;
   uint8_t Data[8];
   uint8_t Length;
   J1939_PDU_STRUCT Message;
   
   J1939ReceiveTask();  //J1939ReceiveTask() needs to be called often
   //J1939XmitTask();     //J1939XmitTask() needs to be called often
   
   if(J1939Kbhit())  //Checks for new message in J1939 Receive buffer
   {
      J1939GetMessage(Message,Data,Length);  //Gets J1939 Message from receive buffer
      
      pgn = Message.PDUFormat;
      pgn = pgn <<8 | (int16)Message.DestinationAddress;
      
      printf ("La PGN es: %LX \r",pgn);
      
      printf(" Dirección fuente: %x \r", Message.SourceAddress);
      delay_ms(50); 
      
       lecturaDelParametro(pgn,SPN_ENGINE_FUEL_RATE, &dato );
     printf("El dato recuperado es: %ld \r", dato);
      
      printf("{ ");
      for (i =0; i<Length ; i++){
         printf("%X , ",Data[i]);
      }
      printf("} \r");
      printf("\r");
      
    
     
      
   }      // END KBHIT()
   
  
}





void main()
{
  #if defined(__PCD__)
   setup_timer2(TMR_INTERNAL,10000); //1ms tick
  #else
   setup_timer_2(T2_DIV_BY_4, 250, 5); //1ms tick
  #endif
   enable_interrupts(INT_TIMER2);
  #if defined(__PCD__)
   enable_interrupts(INTR_GLOBAL);
  #else
   enable_interrupts(GLOBAL);
  #endif

   J1939Init();  //Initialize J1939 Driver must be called before any other J1939 function is used
   
      
   while(TRUE)
   {
      J1939Task();
      
   }
}
