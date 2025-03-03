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
 
 
 void motorControl(uint8_t percent){
     static uint8_t lastPercent=0; 
     CAN_TX_MSGOBJ txMsg={0};
     uint8_t txdata[8];
         if(lastPercent!= percent) { 
             lastPercent = percent;
 
             txMsg.bF.id.ID = ID_PWR_MOTOR;
             txMsg.bF.ctrl.DLC = CAN_DLC_1;
             txdata[0] = percent;
         CanSend(&txMsg,txdata); 
     } 
 }
 
 bool motorControl_Process(Event* ev){
     
     typedef enum
     {
         INIT,
         OFF,
         CRUISE,
         BRAKE,
         STANDBY,
         ACCELERATION,
         DECELERATION
     }MotorState;
     static MotorState state = INIT;
     static MotorState oldState = INIT;
     
     
 
     
     
     
     
     // Transition state machine
     switch(state)   
     {
         case INIT:
             if(ev->id == E_INIT){
                 state = OFF;
             }
             break;
         case OFF:
             if(ev->id == E_CONTACT_ON)
             {
                 state = STANDBY;
 
             }
             break;
 
         case STANDBY:
             if(ev->id == E_CONTACT_OFF)
             {
                 state = OFF;
             }else if(ev->id == E_ACCELERATION_ON)
             {
                 state = ACCELERATION;
             }
         case ACCELERATION:
             
             
             break;
         case DECELERATION:
             
             break;
             
         case CRUISE:
             if(ev->id == E_CONTACT_OFF)
             {
                 state = OFF;
             }else if(ev->id == E_ACCELERATION_OFF)
             {
                 state = STANDBY;
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
         case INIT:
         break;
         case OFF:
             motorControl(0);
             break;
         case CRUISE:
 
             break;    
         case STANDBY:
            lightContol_FrontLight(50);
            lightContol_BackLight(50);
            break;
         case BRAKE:
                
             break;
         case ACCELERATION:
             
                 for (uint8_t i = 0; i < 100; i++){
                     __delay_ms(100);
                     motorControl(i);
                 }
 
             break;
         case DECELERATION:
              
                 for (uint8_t i = 0; i < 100; i++){
                     __delay_ms(500);
                     motorControl(i);
                 }
    
             break;
 
         default:
             break;
     }    
     return 0;
 
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
                 if (rxdata[0] > 6) 
                 {
                     XF_post(motorControl_Process, E_ACCELERATION_ON, 0);
                 }else
                 break; 
 
             default:
                 // Aucun cas reconnu
                 break;
         }
     }
 }
 
 