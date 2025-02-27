/*
 * File:   carFunctions.c
 * Author: pascal.sartoret
 *
 * Created on 1. d�cembre 2023, 15:10
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
        BRAKE                
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
    // TODO complete code
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
    // TODO complete code
        default:
            break;
    }    
    return 0;
}

void updateCarState(void) 
{
    // Lecture en cas de message
    CAN_RX_MSGOBJ rxObj ;
    uint8_t rxdata[8];
 
    if(CanReceive(&rxObj,&rxdata) == 0) {
        if((rxObj.bF.id.ID & 0x3F0) == ID_CONTACT_KEY){
            carState.contactKey = rxdata[0];
            if(rxdata[0]==0){
                XF_post(lightControl_Process,E_CONTACT_OFF,0);
            }else{
                XF_post(lightControl_Process,E_CONTACT_ON,0);
            }
        }
    }
}

