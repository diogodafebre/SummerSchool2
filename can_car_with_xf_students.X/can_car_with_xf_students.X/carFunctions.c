/*
 * File:   carFunctions.c
 * Author: Diogo Febre
 *
 * Created on 3. mars 2025, 15:10
 */


 #include <xc.h>
 #include "mcc_generated_files/mcc.h"
 
 #include <string.h>
 #include "can/can.h"
 #include "car_msg_id.h"
 #include "carFunctions.h"
 #include "xf/event.h"
 
 extern CarState carState;
 
 #define MIN(a,b) (((a)<(b))?(a):(b))
 #define MAX(a,b) (((a)>(b))?(a):(b))

 void motorControl(uint8_t percent,uint8_t starter){
     static uint8_t lastPercent = 0; 
     static uint8_t lastStarter = 0; 
     CAN_TX_MSGOBJ txMsg={0};
     uint8_t txdata[8];
         if((lastPercent != percent)||(lastStarter != starter)) { 
             lastPercent = percent;
             lastStarter = starter;
 
             txMsg.bF.id.ID = ID_PWR_MOTOR;
             txMsg.bF.ctrl.DLC = CAN_DLC_2;
             txdata[0] = percent;
             txdata[1] = starter;
         CanSend(&txMsg,txdata); 
     } 
 }
 void lightContol_FrontLight(uint8_t light)
 {
 
    static uint8_t lastLight=0; 
    CAN_TX_MSGOBJ txMsg={0};
    uint8_t txdata[8];
    if(lastLight!= light) { 
        lastLight = light;
    
        txMsg.bF.id.ID = ID_LIGHT_FRONT;
        txMsg.bF.ctrl.DLC = CAN_DLC_1;
        txdata[0] = light;
        CanSend(&txMsg,txdata); 
    } 
 }
 void lightContol_BackLight(uint8_t light)
 {
     static uint8_t lastLight=0; 
     CAN_TX_MSGOBJ txMsg={0};
     uint8_t txdata[8];
     if(lastLight!= light) { 
         lastLight = light;
 
         txMsg.bF.id.ID = ID_LIGHT_BACK;
         txMsg.bF.ctrl.DLC = CAN_DLC_1;
         txdata[0] = light;
         CanSend(&txMsg,txdata); 
     } 
 }
 bool motorControl_Process(Event* ev){
     
     typedef enum
     {
         OFF,
         CRUISE
     }MotorState;
     static MotorState state = OFF;
     static MotorState oldState = OFF;

     
     // Transition state machine
     switch(state)   
     {
        
         case OFF:
             if(ev->id == E_CONTACT_ON)
             {
                 state = CRUISE;
 
             }
             break;
             
 
         case CRUISE:
             if(ev->id == E_CONTACT_OFF)
             {
                 state = OFF;
             }
         
         default :
             break;
     }
     
     // Check state change
     if(oldState == state)    
    {
        return 0;            // no change, no entry action
    }
   
     
    oldState = state; 
    
    // State machine actions
    switch(state)            
     {
         
         case OFF:
             motorControl(0,0);
             break;
         case CRUISE:
             motorControl(10,1);
             break;    
            
         default:
             break;
     }    
     return 0;
 
 }
 bool lightControl_Process(Event* ev)
 {
     typedef enum
     {
         INIT,
         OFF,
         CRUISE,
         BRAKE,     
                   
     }LightState;
     static LightState state = INIT;
     static LightState oldState = INIT;
     switch(state)   // transition state machine
     {
         case INIT:
             if(ev->id == E_INIT){
                 state = OFF;
             }
             break;
         case OFF:
             if(ev->id == E_CONTACT_ON)
             {
                 state = CRUISE;
 
             }
             break;
         case CRUISE:
             if(ev->id == E_CONTACT_OFF)
             {
                 state = OFF;
 
             }
             else if (ev->id == E_HIGH_BRAKE)
             {
                 state = BRAKE;
             }
             break; 
         case BRAKE:
             if(ev->id == E_CONTACT_OFF)
             {
                 state = OFF;
 
             }else if (ev->id == E_HIGH_BRAKE_RELASED){
                 state = CRUISE;
             }
             
 
         default:
             break;
     }
    if(oldState == state)    // check state change
    {
        return 0;            // no change, no entry action
    }
    oldState = state; 
    switch(state)            // on entry execution state machine
     {
         case INIT:
         break;
         case OFF:
             lightContol_FrontLight(0);
             lightContol_BackLight(0);
             break;
         case CRUISE:
             lightContol_FrontLight(50);
             lightContol_BackLight(50);
             break;    
         case BRAKE:
             lightContol_BackLight(100);
             break;
 
         default:
             break;
     }    
     return 0;
 }
 void updateCarState(void) 
 {
     // Lecture en cas de message
     CAN_RX_MSGOBJ rxObj;
     uint8_t rxdata[8];
 
     if (CanReceive(&rxObj, rxdata) == 0) 
     {
         switch (rxObj.bF.id.ID & 0x7F0)
         {
             // Contact
             case ID_CONTACT_KEY:
                 carState.contactKey = rxdata[0];
 
                 if (rxdata[0] == 0) 
                 {
                     XF_post(lightControl_Process, E_CONTACT_OFF, 0);
                     XF_post(motorControl_Process, E_CONTACT_OFF, 0);
                 } 
                 else 
                 {
                     XF_post(lightControl_Process, E_CONTACT_ON, 0);
                     XF_post(motorControl_Process, E_CONTACT_ON, 0);
                     
                 }
                 break; 
 
             // Freinage
             case ID_BRAKE_PEDAL:
                 carState.brakePedal = rxdata[0];
 
                 if (rxdata[0] > 6) 
                 {
                     XF_post(lightControl_Process, E_HIGH_BRAKE, 0);
                 }else if (rxdata[0] < 6) 
                 {
                     XF_post(lightControl_Process, E_HIGH_BRAKE_RELASED, 0);
                 }
                 break; 
 
                
             // Accélération
             case ID_ACCEL_PEDAL:
                 carState.accelPedal = rxdata[0];
                 if (rxdata[0] > 10) 
                 {
                     XF_post(motorControl_Process, E_ACCELERATION_ON, 0);
                 }else if (rxdata[0] < 10) 
                 {
                     XF_post(motorControl_Process, E_ACCELERATION_OFF, 0);
                 }
                 break; 
 
             default:
                 // Aucun cas reconnu
                 break;
         }
     }
 }
 
 