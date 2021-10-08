#ifndef ADSDRIVER_H
#define	ADSDRIVER_H

#include "ADS1261.h"
#include "ADSDriver.h"
#include <stdlib.h> 
#include <stdint.h>
#include "ADSConfigurations.h"
#include <driver/driver_common.h>
#include <driver/spi/drv_spi_definitions.h>
#include "ADS_Status.h"

//typedef uint32_t ADS_HANDLE;

#define MAX_INSTANCES 5
#define BUFFER_SIZE 20

typedef struct ADSContext
{
    ADSInitData adsInitData;
    DRV_HANDLE spiHandle;
    uint32_t readBuffer[BUFFER_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
    volatile uint32_t *csPort;
    uint32_t csPinMask;
    DRV_SPI_BUFFER_EVENT bufferHandle;     //current buffer state
    uint32_t dataCount;
    uint8_t tmpReadBuffer[5];
    uint8_t tmpWriteBuffer[5];   
    void (*InvalidOperationCallback)(DRV_HANDLE adsHandle, ADS_OPERATION_STATUS status); // CallBack calls on buffer overflow or incorrect response.
    bool suspendReading;
    
} ADSContext;
                           
ADSContext *instances[MAX_INSTANCES];
extern uint8_t instanceCount;
extern DRV_HANDLE currentHandle;

DRV_HANDLE GetInstance();
void SetCS(ADSContext *context);
void UnSetCS(ADSContext *context);
DRV_HANDLE Deinitialize(DRV_HANDLE handle);
bool VerifyHande(DRV_HANDLE adsHandle);

#endif	/* ADSDRIVER_H */

