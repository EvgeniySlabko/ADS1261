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

#define IS_PULSE_CONVERSION_MODE(adsHnadle) (((ADSContext *)adsHandle)->adsInitData.mode1.convrt == 0b1)

typedef struct ADSContext
{
    ADSInitData adsInitData;                    //divice configuration
    DRV_HANDLE spiHandle;                       //
    uint32_t readBuffer[BUFFER_SIZE];           // data buffer. 
    uint8_t readIndex;                          // 
    uint8_t writeIndex;                         //
    volatile uint32_t *csPort;                  // LAT register for CS pin for spi 
    uint32_t csPinMask;                         // mask for CS pin
    DRV_SPI_BUFFER_EVENT bufferHandle;          //current spi buffer state. spi driver set DRV_SPI_BUFFER_EVENT_COMPLETE after finish of operation.
    uint32_t dataCount;                         // unread data in the buffer
    uint8_t tmpReadBuffer[5];                   // IO buffers
    uint8_t tmpWriteBuffer[5];                  //
    uint8_t missedPackets;                      //number of missed packets
    void (*InvalidOperationCallback)(DRV_HANDLE adsHandle, ADS_CALLBACK_MESSAGE status); // CallBack called in the following cases:
                                                                                         // -owerflof read buffer
                                                                                         // -get invalid response
    
    void (*DataHandler) (DRV_HANDLE adsHandle);                                          // Data received. You can read it from buffer.
    bool suspendReading;                                                                 // suspend reading flag. nessesary for sunchronize read interrupt data and user messages such as set new configuration, calibration or read/write configuration registers
    
} ADSContext;
                           
ADSContext *instances[MAX_INSTANCES];       
extern uint8_t instanceCount;

DRV_HANDLE GetInstance();
void SetCS(ADSContext *context);
void UnSetCS(ADSContext *context);
DRV_HANDLE Deinitialize(DRV_HANDLE handle);
bool VerifyHande(DRV_HANDLE adsHandle);

#endif	/* ADSDRIVER_H */

