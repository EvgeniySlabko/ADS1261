#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <driver/driver.h>
#include "ADSConfigurations.h"
#include "ADSDriver.h"
#include <driver/driver_common.h>

//#define DRDY_PIN                        PORTDbits.RD13 
//#define CS_PIN                          PORTDbits.RD12 

extern bool calibration;
extern bool wasData;

uint8_t ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address);
DRV_HANDLE Init_ADS1261(DRV_HANDLE spiHandle, volatile uint32_t *cs_lat_potr, volatile uint32_t cs_pin_mask);
void WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data);
void ConfigureDevice(DRV_HANDLE adsHandler);
void Start(DRV_HANDLE spiHandle);
void Stop(DRV_HANDLE adsHandle);
void OffsetSelfCalibration(DRV_HANDLE adsHandle);
void Reset(DRV_HANDLE adsHandle);
uint32_t ReadData(DRV_HANDLE adsHandle);
bool ReadDataIfExists(DRV_HANDLE adsHandle, uint32_t *data);
void SetOffset(DRV_HANDLE adsHandle, uint32_t offset);
void TransmitError(DRV_HANDLE adsHandle);
void DRDYHandler();
void TestForBusy(DRV_HANDLE adsHandle);

//GLx
void SetGain(DRV_HANDLE adsHandle, unsigned gainValue);

//DFx
void SetDigitalFilter(DRV_HANDLE adsHandle, unsigned filter);

void Unlock(DRV_HANDLE adsHandle);
#endif	/* DEFINITIONS_H */
