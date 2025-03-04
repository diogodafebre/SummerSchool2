/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F97J60
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "can/can.h"
#include "car_msg_id.h"
#include "lcd/lcd.h"
#include "carFunctions.h"
#include "xf/xf.h"
volatile uint32_t dummy;

CarState carState;
/*
                         Main application
 */
void main(void)
{
    CAN_FILTEROBJ_ID filterObj;
    CAN_MASKOBJ_ID maskObj;

    // Initialisation des parame?tres
    filterObj.ID = 1;     // ID que nous souhaitons filtrer 
    filterObj.SID11 = 0;  // Nous utilisons que des ID standards 
    filterObj.EXIDE = 0;  // 0 ? Identifiant standard (11 bits)

    maskObj.MID = 0x00F;  // Masque des 4 derniers bit (le CTL -> 0x1)
    maskObj.MIDE = 1;     // Filtrage sur ID standard
    maskObj.MSID11 = 0;   // Pas utilise?
    SYSTEM_Initialize();

    //Appel de la fonction CanSetFilter dans le main
    CanInit(CAN_250K_500K, false);
    CanSetFilter(CAN_FILTER0, &filterObj, &maskObj);
    // Initialize the device
    XF_init();

    TMR0_SetInterruptHandler(XF_decrementAndQueueTimers);
    // TODO complete code

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    XF_post(lightControl_Process,E_INIT,0);
    while (1)
    {
        updateCarState();   // high priority CAN read function
        XF_executeOnce();        
    }
}
/**
 End of File
*/