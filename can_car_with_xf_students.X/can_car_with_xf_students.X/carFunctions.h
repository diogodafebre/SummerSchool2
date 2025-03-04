/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_CARFUNCTIONS_H
#define	XC_CARFUNCTIONS_H

typedef enum 
{
    E_NULL=0,
    E_INIT,
    E_CONTACT_OFF,
    E_CONTACT_ON,
    E_HIGH_BRAKE,
    E_START_STOP,
    E_TEMPOMAT_ON,
    E_TEMPOMAT_OFF,
    E_ACCELERATION_ON,
    E_ACCELERATION_OFF,
    E_HIGH_BRAKE_RELASED,
    E_GEAR_PARK,
    E_GEAR_DRIVE,
    E_GEAR_REVERSE,
    E_GEAR_NEUTRAL
            
}Event_id;


#include <xc.h> // include processor files - each processor file is guarded.  
#include "xf/xf.h"


bool gearSelect_Proceess(Event* ev);
bool lightControl_Process(struct Event_* ev);
bool motorControl_Process(Event* ev);

void updateCarState(void);
void gearSelect(uint8_t gearEngaged);
void lightContol_FrontLight(uint8_t light);
void lightContol_BackLight(uint8_t light);

void motorControl(uint8_t percent,uint8_t starter);
void handleAcceleration(void);

void audioGest(uint8_t wheelsDrive,uint8_t motorAudio);

#endif	/* XC_CARFUNCTIONS_H */

