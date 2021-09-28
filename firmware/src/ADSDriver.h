#ifndef ADSDRIVER_H
#define	ADSDRIVER_H

#include "ADS1261.h"
#include "ADSDriver.h"
#include <stdlib.h> 
#include <stdint.h>
#include "ADSConfigurations.h"
#include <driver/driver_common.h>
#include <driver/spi/drv_spi_definitions.h>

//typedef uint32_t ADS_HANDLE;

#define MAX_INSTANCES 5
#define currentContext ((ADSContext *)currentHandle)

typedef struct ADSContext
{
    ADSInitData adsInitData;
    DRV_HANDLE spiHandle;
    uint32_t readBuffer[5];
    uint8_t readCount;
    uint8_t writeCount;
    volatile uint32_t *csPort;
    uint32_t csPinMask;
    DRV_SPI_BUFFER_EVENT bufferHandle;     //current buffer state
    
} ADSContext;

ADSContext *instances[MAX_INSTANCES];
extern uint8_t instanceCount;
extern DRV_HANDLE currentHandle;

DRV_HANDLE GetInstance();
void SetCS();
void UnSetCS();
DRV_HANDLE Deinitialize(DRV_HANDLE handle);
#endif	/* ADSDRIVER_H */

