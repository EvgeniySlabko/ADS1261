#include "ADS1261.h"
#include "Delay.h"
#include "app.h"

DRV_HANDLE spiHandle;
bool wasData = false;
bool calibration = false;

DRV_HANDLE Init_ADS1261(DRV_HANDLE spiHandle, volatile uint32_t *cs_lat_potr, volatile uint32_t cs_pin_mask)
{
    DRV_HANDLE currentHandle = GetInstance();
    ADSContext *context = (ADSContext *)currentHandle;
    if (currentHandle == DRV_HANDLE_INVALID)
    {
        return DRV_HANDLE_INVALID;
    }
    
    memset (&(context->adsInitData), 0, sizeof (ADSInitData));
    
    context->spiHandle = spiHandle;
    context->csPinMask = cs_pin_mask;
    context->csPort = cs_lat_potr;
    context->bufferHandle = 0;

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
    context->adsInitData.mode0.dataRate = DR5;
    
    //    Select the digital filter mode.
    //    000: sinc1
    //    001: sinc2
    //    010: sinc3
    //    011: sinc4
    //    100: FIR (default)
    //    101: Reserved
    //    110: Reserved
    //    111: Reserved
    context->adsInitData.mode0.filter = DF5;

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
    context->adsInitData.mode1.delay = 0b0001;
    //Chop and AC-Excitation Modes
    //Select the Chop and AC-excitation modes.
    //00: Normal mode (default)
    //01: Chop mode
    //10: 2-wire AC-excitation mode ( ADS1261 only)
    //11: 4-wire AC-excitation mode ( ADS1261 only)

    context->adsInitData.mode1.chop = 0b11;
    // ADC Conversion Mode
    context->adsInitData.mode1.convrt = 0b0; // 0: Continuous conversions (default)
    // GPIO control. Set AN1 and AN2 as analog inputs
    context->adsInitData.mode2.gpio_con2 = 1;
    context->adsInitData.mode2.gpio_con3 = 1;
    //ads_init_data.mode2.gpio_con0 = 1;
    //ads_init_data.mode2.gpio_con1 = 1;
    
    context->adsInitData.ref.rmuxn = 0b10;
    context->adsInitData.ref.rmuxp = 0b10;
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
    context->adsInitData.pga.gain = GL8;
 

    context->adsInitData.inpmux.muxp = 0b0011;
    context->adsInitData.inpmux.muxn = 0b0100;


    ConfigureDevice(currentHandle);
return currentHandle;
}

void TransmitError(DRV_HANDLE adsHandle)
{
    //GREEN = 1;
}

void TransmitComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
    ADSContext *adsContext = (ADSContext *) context;
    SetCS(adsContext);
}

void WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data)
{
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    uint8_t response[3];
    uint8_t query[3];
    query[0] = 0x40 + address;
    query[1] = data;
    query[2] = 0x00;

    
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 3, response, 3, TransmitComplete, (void *) context, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

uint8_t ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address)
{
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    uint8_t response[3];
    uint8_t query[3];
    query[0] = 0x20 + address;
    query[1] = 0x00;
    query[2] = 0x00;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 3, response, 3, TransmitComplete, context, &(context->bufferHandle));     
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
    return response[2];
}

void Start(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    
    TestForBusy(adsHandle);

    
    UnSetCS(context);
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x08;
    query[1] = 0x00;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 2, response, 2, TransmitComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

void Stop(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x0A;
    query[1] = 0x00;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 2, response, 2, TransmitComplete, context, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

void Reset(DRV_HANDLE adsHandle)
{ 
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x06;
    query[1] = 0x00;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 2, response, 2, TransmitComplete, context, &(context->bufferHandle));  
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
}

uint32_t ReadData(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    uint32_t result;
    uint8_t dataBuf[5];
    uint8_t resultBuf[5];   
    dataBuf[0] = 0x12; // operation
    dataBuf[1] = 0x00; // Arbitary
    dataBuf[2] = 0x00; // nop
    dataBuf[3] = 0x00; // nop
    dataBuf[4] = 0x00; // nop
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, dataBuf, 5, resultBuf, 5, TransmitComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }
    
    
    DelayInMillisecond(1);
    result += resultBuf[2];
    result = result << 8;
    result += resultBuf[3];
    result = result << 8;
    result += resultBuf[4];
    return result;
}

void ConfigureDevice(DRV_HANDLE adsHandle)
{   
    ADSContext *context = (ADSContext *)adsHandle;
    WriteRegisterByte(adsHandle, MODE0_ADDR, *(uint8_t*) &(context->adsInitData.mode0));
    WriteRegisterByte(adsHandle, MODE2_ADDR, *(uint8_t*) &(context->adsInitData.mode2));
    WriteRegisterByte(adsHandle, REF_ADDR, *(uint8_t*) &(context->adsInitData.ref));
    WriteRegisterByte(adsHandle, PGA_ADDR, *(uint8_t*) &(context->adsInitData.pga));
    WriteRegisterByte(adsHandle, INPMUX_ADDR, *(uint8_t*) &(context->adsInitData.inpmux));
    WriteRegisterByte(adsHandle, MODE1_ADDR, *(uint8_t*) &(context->adsInitData.mode1));
    
    WriteRegisterByte(adsHandle, OFCAL0_ADDR, context->adsInitData.ofScaleL);
    WriteRegisterByte(adsHandle, OFCAL1_ADDR, context->adsInitData.ofScaleM);
    WriteRegisterByte(adsHandle, OFCAL2_ADDR, context->adsInitData.ofScaleH);
}

void CalibrationFinish(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context)
{
    //calibration = false;
    //wasData = false;
}

void Unlock(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    
    TestForBusy(adsHandle);
    
            
    UnSetCS(context);
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0xF5;
    query[1] = 0x00;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 2, response, 2, TransmitComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {         
        TransmitError(adsHandle);
    }
}
void OffsetSelfCalibration(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    
    Unlock(adsHandle);
    //calibration = true; 
    Start(adsHandle);
    UnSetCS(context);
    TestForBusy(adsHandle);
    uint8_t response[2];
    uint8_t query[2];
    query[0] = 0x19;
    query[1] = 0x00;

    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, query, 2, response, 2, TransmitComplete, context, &(context->bufferHandle));
    calibration = false;
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        TransmitError(adsHandle);
    }

    TestForBusy(adsHandle);
}

void TestForBusy(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    if (context->bufferHandle != 0)
    {   
        while((DRV_SPI_BufferStatus(context->bufferHandle)) != DRV_SPI_BUFFER_EVENT_COMPLETE){}
    }
}

void SetOffset(DRV_HANDLE adsHandle, uint32_t subtractedValue)
{
    ADSContext *context = (ADSContext *)adsHandle;
    context->adsInitData.ofScaleL = subtractedValue & 0xFF;
    subtractedValue >> 8;
    context->adsInitData.ofScaleM = subtractedValue & 0xFF;
    subtractedValue >> 8;
    context->adsInitData.ofScaleH = subtractedValue & 0xFF;
    ConfigureDevice(adsHandle);
}

bool ReadDataIfExists(DRV_HANDLE adsHandle, uint32_t *data)
{
    if (!wasData)
    {
        data = NULL;
        return false;
    }
    
    wasData = false;
    *data = ReadData(adsHandle);
    return true;
}

void SetGain(DRV_HANDLE adsHandle, unsigned gainValue)
{
  ADSContext *context = (ADSContext *)adsHandle;
  context->adsInitData.pga.gain = gainValue;
  ConfigureDevice(adsHandle);
}

void SetDigitalFilter(DRV_HANDLE adsHandle, unsigned filter)
{
  ADSContext *context = (ADSContext *)adsHandle;
  context->adsInitData.mode0.filter = filter;
  ConfigureDevice(adsHandle);
}

void DRDYHandler()
{
    //RED = ~RED;
    //if (!calibration)
    //{
        wasData = true;
    //}
    
}