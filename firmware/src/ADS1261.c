#include "ADS1261.h"
#include "Delay.h"
#include "app.h"

ADSInitData ads_init_data;
DRV_HANDLE spiHandle;

bool calibration = false;

DRV_HANDLE Init_ADS1261(DRV_HANDLE spiHandle, volatile uint32_t *cs_lat_potr, volatile uint32_t cs_pin_mask)
{
    currentHandle = GetInstance();
    if (currentHandle == DRV_HANDLE_INVALID)
    {
        return DRV_HANDLE_INVALID;
    }
    
    memset (&(currentContext->adsInitData), 0, sizeof (ADSInitData));
    currentContext->spiHandle = spiHandle;
    currentContext->csPinMask = cs_pin_mask;
    currentContext->csPort = cs_lat_potr;
    currentContext->bufferHandle = 0;

    // Set default configuration

    //    Select the ADC data rate.
    //    00000: 2.5 SPS
    //    00001: 5 SPS
    //    00010: 10 SPS
    //    00011: 16.6 SPS
    //    00100: 20 SPS (default)
    //    00101: 50 SPS
    //    00110: 60 SPS
    //    00111: 100 SPS
    //    01000: 400 SPS
    //    01001: 1200 SPS
    //    01010: 2400 SPS
    //    01011: 4800 SPS
    //    01100: 7200 SPS
    //    01101: 14400 SPS
    //    01110: 19200 SPS
    //    01111: 25600 SPS
    //    10000 - 11111: 40000 SPS (fCLK = 10.24 MHz)
    currentContext->adsInitData.mode0.dataRate = DR5;
    
    //    Select the digital filter mode.
    //    000: sinc1
    //    001: sinc2
    //    010: sinc3
    //    011: sinc4
    //    100: FIR (default)
    //    101: Reserved
    //    110: Reserved
    //    111: Reserved
    currentContext->adsInitData.mode0.filter = 0b100;

    //    Conversion Start Delay
    //    Program the time delay at conversion start. Delay values are
    //    with fCLK = 7.3728 MHz.
    //    0000: 0 탎 (not for 25600 SPS or 40000 SPS operation)
    //    0001: 50 탎 (default)
    //    0010: 59 탎
    //    0011: 67 탎
    //    0100: 85 탎
    //    0101: 119 탎
    //    0110: 189 탎
    //    0111: 328 탎
    //    1000: 605 탎
    //    1001: 1.16 ms
    //    1010: 2.27 ms
    //    1011: 4.49 ms
    //    1100: 8.93 ms
    //    1101: 17.8 ms
    //    1110: Reserved
    //    1111: Reserved
    currentContext->adsInitData.mode1.delay = 0b0001;
    //Chop and AC-Excitation Modes
    //Select the Chop and AC-excitation modes.
    //00: Normal mode (default)
    //01: Chop mode
    //10: 2-wire AC-excitation mode ( ADS1261 only)
    //11: 4-wire AC-excitation mode ( ADS1261 only)

    currentContext->adsInitData.mode1.chop = 0b11;
    // ADC Conversion Mode
    currentContext->adsInitData.mode1.convrt = 0b0; // 0: Continuous conversions (default)
    // GPIO control. Set AN1 and AN2 as analog inputs
    currentContext->adsInitData.mode2.gpio_con2 = 1;
    currentContext->adsInitData.mode2.gpio_con3 = 1;
    //ads_init_data.mode2.gpio_con0 = 1;
    //ads_init_data.mode2.gpio_con1 = 1;
    
    currentContext->adsInitData.ref.rmuxn = 0b10;
    currentContext->adsInitData.ref.rmuxp = 0b10;
    //Reference Negative Input
    //Select the negative reference input.
    //00: Internal reference negative
    //01: AVSS internal (default)
    //10: AIN1 external
    //11: AIN3 external ( ADS1261 only)

    //ads_init_data.ref.rmuxn = 0b01; 

    //Reference Positive Input
    //Select the positive reference input.
    //00: Internal reference positive
    //01: AVDD internal (default)
    //10: AIN0 external
    //11: AIN2 external ( ADS1261 only)
    //ads_init_data.ref.rmuxp = 0b01; 


    //Gain
    //Select the gain.
    //000: 1 (default)
    //001: 2
    //010: 4
    //011: 8
    //100: 16
    //101: 32
    //110: 64
    //111: 128
    currentContext->adsInitData.pga.gain = 0b111;
 

    currentContext->adsInitData.inpmux.muxp = 0b0011;
    currentContext->adsInitData.inpmux.muxn = 0b0100;


    ConfigureDevice(currentHandle);
return currentHandle;
}

void TransmitError(DRV_HANDLE adsHandle)
{
    RED = 1;
}

void TransmitComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
    SetCS();
}

void WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[3];
    uint8_t query[3];
    query[0] = 0x40 + address;
    query[1] = data;
    query[2] = 0x00;

    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 3, response, 3, TransmitComplete, 0, &(currentContext->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
    
    //DelayInMillisecond(10);
}

uint8_t ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[3];
    uint8_t query[3];
    query[0] = 0x20 + address;
    query[1] = 0x00;
    query[2] = 0x00;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 3, response, 3, TransmitComplete, 0, &(currentContext->bufferHandle));     
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
    return response[2];
}

void Start(DRV_HANDLE adsHandle)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x08;
    query[1] = 0x00;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 2, response, 2, TransmitComplete, 0, &(currentContext->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

void Stop(DRV_HANDLE adsHandle)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x0A;
    query[1] = 0x00;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 2, response, 2, TransmitComplete, 0, &(currentContext->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

void Reset(DRV_HANDLE adsHandle)
{ 
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x06;
    query[1] = 0x00;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 2, response, 2, TransmitComplete, 0, &(currentContext->bufferHandle));  
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

uint32_t ReadData(DRV_HANDLE adsHandle)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint32_t result;
    uint8_t dataBuf[5];
    uint8_t resultBuf[5];   
    dataBuf[0] = 0x12; // operation
    dataBuf[1] = 0x00; // Arbitary
    dataBuf[2] = 0x00; // nop
    dataBuf[3] = 0x00; // nop
    dataBuf[4] = 0x00; // nop
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, dataBuf, 5, resultBuf, 5, TransmitComplete, 0, &(currentContext->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
    
    result += resultBuf[2];
    result = result << 8;
    result += resultBuf[3];
    result = result << 8;
    result += resultBuf[4];
    return result;
}

void ConfigureDevice(DRV_HANDLE adsHandle)
{   
    WriteRegisterByte(currentHandle, MODE0_ADDR, *(uint8_t*) &(currentContext->adsInitData.mode0));
    WriteRegisterByte(currentHandle, MODE2_ADDR, *(uint8_t*) &(currentContext->adsInitData.mode2));
    WriteRegisterByte(currentHandle, REF_ADDR, *(uint8_t*) &(currentContext->adsInitData.ref));
    WriteRegisterByte(currentHandle, PGA_ADDR, *(uint8_t*) &(currentContext->adsInitData.pga));
    WriteRegisterByte(currentHandle, INPMUX_ADDR, *(uint8_t*) &(currentContext->adsInitData.inpmux));
    WriteRegisterByte(currentHandle, MODE1_ADDR, *(uint8_t*) &(currentContext->adsInitData.mode1));
}

void OffsetSelfCalibration(DRV_HANDLE adsHandle)
{
    currentHandle = adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS();
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x0A;
    query[1] = 0x00;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(currentContext->spiHandle, query, 2, response, 2, TransmitComplete, 0, &(currentContext->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

void DRDYHandler()
{
    if (!calibration)
    {
        // ?????? ????????
    }  
}

void TestForBusy(DRV_HANDLE adsHandle)
{
    currentHandle = adsHandle;
    if (currentContext->bufferHandle != 0)
    {
        while((DRV_SPI_BufferStatus(currentContext->bufferHandle)) != DRV_SPI_BUFFER_EVENT_COMPLETE){}
    }
}
void SetOffset(DRV_HANDLE adsHandle, uint32_t subtractedValue)
{
    subtractedValue &= 0x00FFFFFF;
    uint8_t *offsetPtr = (uint8_t *)&subtractedValue;
    WriteRegisterByte(adsHandle, OFCAL2_ADDR, offsetPtr[2]);
    WriteRegisterByte(adsHandle, OFCAL1_ADDR, offsetPtr[1]);
    WriteRegisterByte(adsHandle, OFCAL0_ADDR, offsetPtr[0]);
}