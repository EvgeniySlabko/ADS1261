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

extern bool stopReading; 
DRV_HANDLE Init_ADS1261(DRV_HANDLE spiHandle, volatile uint32_t *cs_lat_potr, volatile uint32_t cs_pin_mask);

ADS_OPERATION_STATUS ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t *data);
ADS_OPERATION_STATUS WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data);
ADS_OPERATION_STATUS ConfigureDevice(DRV_HANDLE adsHandler);
ADS_OPERATION_STATUS Start(DRV_HANDLE spiHandle);
ADS_OPERATION_STATUS Stop(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS OffsetSelfCalibration(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS Reset(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS ReadData(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS SetOffset(DRV_HANDLE adsHandle, uint32_t offset);
ADS_OPERATION_STATUS DRDYHandler(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS GainCalibration(DRV_HANDLE adsHandle);
ADS_OPERATION_STATUS SetFScale(DRV_HANDLE adsHandle, uint32_t fScale);
ADS_OPERATION_STATUS SetInvalidResponseCallback(DRV_HANDLE adsHandle, void (*InvalidResponseCallback)(DRV_HANDLE adsHandle, ADS_OPERATION_STATUS status));


void PauseReading(DRV_HANDLE adsHandle);
void ContinueReading(DRV_HANDLE adsHandle);

bool GetData(DRV_HANDLE adsHandle, uint32_t *data);
void TestForBusy(DRV_HANDLE adsHandle);

//GLx
ADS_OPERATION_STATUS SetGain(DRV_HANDLE adsHandle, unsigned gainValue);

//DFx
ADS_OPERATION_STATUS SetDigitalFilter(DRV_HANDLE adsHandle, unsigned filter);

ADS_OPERATION_STATUS Unlock(DRV_HANDLE adsHandle);
#endif	/* DEFINITIONS_H */
