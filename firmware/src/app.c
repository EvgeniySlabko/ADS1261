/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "ADS1261.h"
#include "Delay.h"
#include "ADSDriver.h"

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

uint32_t statusArray[256];
uint32_t dataArr[300];
uint8_t i = 0;
uint8_t j = 0;
uint8_t status;
uint32_t calVal = 0;
bool flag = false;
uint32_t data;
DRV_HANDLE ads_handle;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    appData.handleSPI0 = DRV_HANDLE_INVALID;
    
    //DRV_HANDLE ADSHandle = GetInstance();
    //Deinitialize(ADSHandle);
            
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
void Button_Task()
{
    if (!BUTTON)
    {
        if (true)
        {
            //SetOffset(ads_handle, calVal++);
            OffsetSelfCalibration(ads_handle);
            flag =  true;
        }
        
        //DelayInMillisecond(1000);
    }
    
}

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            
            if (DRV_HANDLE_INVALID == appData.handleSPI0)
            {
                appData.handleSPI0 = DRV_SPI_Open(0, DRV_IO_INTENT_READWRITE);
                appInitialized &= (DRV_HANDLE_INVALID != appData.handleSPI0);
            }
        
            if (appInitialized)
            {
                /* initialize the SPI state machine */
                appData.spiStateMachine = APP_SPI_STATE_START;
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            
            DelayInMillisecond(100);
            ads_handle = Init_ADS1261(appData.handleSPI0, &LATD, (uint32_t)0b1000000000000);
            //SetOffset(ads_handle, 0x555555);
            //Unlock(ads_handle);
            //DelayInMillisecond(100);
            
            //Stop(appData.handleSPI0);
            //Reset(appData.handleSPI0);
            //DelayInMillisecond(1000);
            
            //Init_ADS1261(appData.handleSPI0);

            //DelayInMillisecond(1000);
            
            //Start(appData.handleSPI0);  
            //Reset(ads_handle);
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            
            DelayInMillisecond(30);
            //Button_Task();
            //ReadData(ads_handle);
            //uint8_t reg = ReadRegisterByte(ads_handle, OFCAL0_ADDR);
            //OffsetSelfCalibration(ads_handle);
            
            if (ReadDataIfExists(ads_handle, &data))
            {
                RED = ~RED;
            }
            
            
        }
        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
/*******************************************************************************
 End of File
 */
