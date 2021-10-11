#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <driver/driver.h>
#include "ADSConfigurations.h"
#include "ADSDriver.h"
#include "ADS_Status.h"
#include <driver/driver_common.h>

// initialise new instance of ADS driver. example: DRV_HANDLE ads_handle = Init_ADS1261(appData.handleSPI0, &LATD, (uint32_t)0b1000000000000);
DRV_HANDLE Init_ADS1261(DRV_HANDLE spiHandle, volatile uint32_t *cs_lat_potr, volatile uint32_t cs_pin_mask);

// configere the device. 
ADS_OPERATION_STATUS ConfigureDevice(DRV_HANDLE adsHandler);

ADS_OPERATION_STATUS ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t *data);
ADS_OPERATION_STATUS WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data);
ADS_OPERATION_STATUS Start(DRV_HANDLE spiHandle);
ADS_OPERATION_STATUS Stop(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS OffsetSelfCalibration(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS Reset(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS SetOffset(DRV_HANDLE adsHandle, uint32_t offset);
ADS_OPERATION_STATUS GainCalibration(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS SetFScale(DRV_HANDLE adsHandle, uint32_t fScale);
ADS_OPERATION_STATUS Unlock(DRV_HANDLE adsHandle);

ADS_OPERATION_STATUS SetInvalidResponseCallback(DRV_HANDLE adsHandle, void (*InvalidResponseCallback)(DRV_HANDLE adsHandle, ADS_CALLBACK_MESSAGE status));
ADS_OPERATION_STATUS SetDataHandler(DRV_HANDLE adsHandle, void (*DataHandler)(DRV_HANDLE adsHandle));
// this should only be called upon of the DRDY interrupt. (fallen edge).
ADS_OPERATION_STATUS DRDYHandler(DRV_HANDLE adsHandle);

// It calls only from DRDY handler.
ADS_OPERATION_STATUS ReadData(DRV_HANDLE adsHandle);

//sincronize.
void PauseReading(DRV_HANDLE adsHandle);
void ContinueReading(DRV_HANDLE adsHandle);


//Get data from buffer.
bool GetData(DRV_HANDLE adsHandle, uint32_t *data);

void TestForBusy(DRV_HANDLE adsHandle);

//Set gain(PGA). GLx 
ADS_OPERATION_STATUS SetGain(DRV_HANDLE adsHandle, unsigned gainValue);

//Set dig. filter. DFx
ADS_OPERATION_STATUS SetDigitalFilter(DRV_HANDLE adsHandle, unsigned filter);

//Set Conversion mode
ADS_OPERATION_STATUS SetConversionMode(DRV_HANDLE adsHandle, unsigned mode);
#endif	/* DEFINITIONS_H */
