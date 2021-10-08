#include "ADS1261.h"
#include "Delay.h"
#include "app.h"
#include "ADSDriver.h"

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
    context->InvalidOperationCallback = 0;
    context->readIndex = 0;
    context->writeIndex = 0;
              
            
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


    if (ConfigureDevice(currentHandle) == ADS_CONFIGURATION_ERRROR)
    {
        Deinitialize(currentHandle);
        return DRV_HANDLE_INVALID;
    }
    return currentHandle;
}

void PauseReading(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    SYS_INT_Disable();
    context->suspendReading = true;
    SYS_INT_Enable();
    TestForBusy(adsHandle); // wait until the read operation completes. 
}

void ContinueReading(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);    
    SYS_INT_Disable();
    context->suspendReading = false;
    SYS_INT_Enable();
}

void TransmitComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
    ADSContext *adsContext = (ADSContext *) context;
    if ((adsContext->tmpReadBuffer[0] != 0xFF || adsContext->tmpReadBuffer[1] != adsContext->tmpWriteBuffer[0]))
    {
        if (adsContext->InvalidOperationCallback != 0) 
            adsContext->InvalidOperationCallback((ADS_OPERATION_STATUS)adsContext, ADS_INVALID_RESPONSE);
    }
    
    SetCS(adsContext);
}

ADS_OPERATION_STATUS WriteRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t data)
{
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    UnSetCS(context);

    context->tmpWriteBuffer[0] = ADS_WREG + address;
    context->tmpWriteBuffer[1] = data;
    context->tmpWriteBuffer[2] = ADS_NOP;  
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 3, context->tmpReadBuffer, 3, TransmitComplete, (void *) context, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS ReadRegisterByte(DRV_HANDLE adsHandle, uint8_t address, uint8_t *data)
{
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
    
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    UnSetCS(context);
    
    context->tmpWriteBuffer[0] = ADS_RREG + address;
    context->tmpWriteBuffer[1] = ADS_NOP;
    context->tmpWriteBuffer[2] = ADS_NOP;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 3, context->tmpReadBuffer, 3, TransmitComplete, context, &(context->bufferHandle));     
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    *data = context->tmpReadBuffer[2];
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS Start(DRV_HANDLE adsHandle)
{
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
    ADSContext *context = (ADSContext *)adsHandle;
  
    PauseReading(adsHandle);

    UnSetCS(context);
    context->tmpWriteBuffer[0] = ADS_START;
    context->tmpWriteBuffer[1] = ADS_NOP;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->tmpReadBuffer, 2, TransmitComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS Stop(DRV_HANDLE adsHandle)
{
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    UnSetCS(context);
    context->tmpWriteBuffer[0] = ADS_STOP;
    context->tmpWriteBuffer[1] = ADS_NOP;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->tmpReadBuffer, 2, TransmitComplete, context, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS Reset(DRV_HANDLE adsHandle)
{ 
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    UnSetCS(context);
    context->tmpWriteBuffer[0] = ADS_RESET;
    context->tmpWriteBuffer[1] = ADS_NOP;
      
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->readBuffer, 2, TransmitComplete, context, &(context->bufferHandle));  
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

void ReadingComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void *context)
{   
    uint32_t result = 0;
    ADSContext *adsContext = (ADSContext *)context;
    result += adsContext->tmpReadBuffer[2];
    result = result << 8;
    result += adsContext->tmpReadBuffer[3];
    result = result << 8;
    result += adsContext->tmpReadBuffer[4];
    
    //adsContext->readBuffer[adsContext->writeIndex] = dbgIter++;
    adsContext->readBuffer[adsContext->writeIndex] = result;
    adsContext->writeIndex = (adsContext->writeIndex == BUFFER_SIZE - 1) ? 0 : adsContext->writeIndex + 1;    
    if (adsContext->dataCount == BUFFER_SIZE)
    {
        adsContext->dataCount = 1;
        if (adsContext->InvalidOperationCallback != 0) adsContext->InvalidOperationCallback((DRV_HANDLE)context, ADS_BUFFER_OVERFLOW); // Owerflow callback
    }
    else
    {
        adsContext->dataCount++;
    }
    
    TransmitComplete(event, bufferHandle, context);
}

ADS_OPERATION_STATUS ReadData(DRV_HANDLE adsHandle)
{
    if (!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE; 
        
    ADSContext *context = (ADSContext *)adsHandle;
    TestForBusy(adsHandle);
    
    UnSetCS(context);
    context->tmpWriteBuffer[0] = ADS_RDATA; // operation
    context->tmpWriteBuffer[1] = ADS_NOP; // Arbitary
    context->tmpWriteBuffer[2] = ADS_NOP; // nop
    context->tmpWriteBuffer[3] = ADS_NOP; // nop
    context->tmpWriteBuffer[4] = ADS_NOP; // nop
    
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 5, context->tmpReadBuffer, 5, ReadingComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }
    
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS ConfigureDevice(DRV_HANDLE adsHandle)
{   
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    ADS_OPERATION_STATUS status = ADS_COMPLETE;
    status &= WriteRegisterByte(adsHandle, ADS_MODE0_ADDR, *(uint8_t*) &(context->adsInitData.mode0));
    status &= WriteRegisterByte(adsHandle, ADS_MODE2_ADDR, *(uint8_t*) &(context->adsInitData.mode2));
    status &= WriteRegisterByte(adsHandle, ADS_REF_ADDR, *(uint8_t*) &(context->adsInitData.ref));
    status &= WriteRegisterByte(adsHandle, ADS_PGA_ADDR, *(uint8_t*) &(context->adsInitData.pga));
    status &= WriteRegisterByte(adsHandle, ADS_INPMUX_ADDR, *(uint8_t*) &(context->adsInitData.inpmux));
    status &= WriteRegisterByte(adsHandle, ADS_MODE1_ADDR, *(uint8_t*) &(context->adsInitData.mode1));
    
    status &= WriteRegisterByte(adsHandle, ADS_OFCAL0_ADDR, context->adsInitData.ofScaleL);
    status &= WriteRegisterByte(adsHandle, ADS_OFCAL1_ADDR, context->adsInitData.ofScaleM);
    status &= WriteRegisterByte(adsHandle, ADS_OFCAL2_ADDR, context->adsInitData.ofScaleH);
    
    status &= WriteRegisterByte(adsHandle, ADS_FSCAL0_ADDR, context->adsInitData.fScaleL);
    status &= WriteRegisterByte(adsHandle, ADS_FSCAL1_ADDR, context->adsInitData.fScaleM);
    status &= WriteRegisterByte(adsHandle, ADS_OFCAL2_ADDR, context->adsInitData.fScaleH);
    
    if (status != ADS_COMPLETE) return ADS_CONFIGURATION_ERRROR;
    
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS Unlock(DRV_HANDLE adsHandle)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    
    PauseReading(adsHandle);
                
    UnSetCS(context);
    context->tmpWriteBuffer[0] = ADS_UNLOCK;
    context->tmpWriteBuffer[1] = ADS_NOP;
    
    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->tmpReadBuffer, 2, TransmitComplete, context, &(context->bufferHandle)); 
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {         
        return ADS_SPI_ERROR;
    }
    
    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS OffsetSelfCalibration(DRV_HANDLE adsHandle)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    Unlock(adsHandle);
    UnSetCS(context);
    
    context->tmpWriteBuffer[0] = ADS_SFOCAL;
    context->tmpWriteBuffer[1] = ADS_NOP;

    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->tmpReadBuffer, 2, 0, 0, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }

    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

ADS_OPERATION_STATUS GainCalibration(DRV_HANDLE adsHandle)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    PauseReading(adsHandle);
    
    Unlock(adsHandle);
    UnSetCS(context);
    
    context->tmpWriteBuffer[0] = ADS_GANCAL;
    context->tmpWriteBuffer[1] = ADS_NOP;

    DRV_SPI_BUFFER_HANDLE bufferHandle = DRV_SPI_BufferAddWriteRead2(context->spiHandle, context->tmpWriteBuffer, 2, context->tmpReadBuffer, 2, 0, 0, &(context->bufferHandle));
    if (bufferHandle == DRV_SPI_BUFFER_HANDLE_INVALID)
    {
        return ADS_SPI_ERROR;
    }

    ContinueReading(adsHandle);
    return ADS_COMPLETE;
}

void TestForBusy(DRV_HANDLE adsHandle)
{
    ADSContext *context = (ADSContext *)adsHandle;
    if (context->bufferHandle != 0)
    {   
        while((DRV_SPI_BufferStatus(context->bufferHandle)) != DRV_SPI_BUFFER_EVENT_COMPLETE){}
    }
}

ADS_OPERATION_STATUS SetOffset(DRV_HANDLE adsHandle, uint32_t subtractedValue)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    
    context->adsInitData.ofScaleL = subtractedValue & 0xFF;
    subtractedValue >> 8;
    context->adsInitData.ofScaleM = subtractedValue & 0xFF;
    subtractedValue >> 8;
    context->adsInitData.ofScaleH = subtractedValue & 0xFF;
        
    ConfigureDevice(adsHandle);  
}

ADS_OPERATION_STATUS SetFScale(DRV_HANDLE adsHandle, uint32_t fScale)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    context->adsInitData.fScaleL = fScale & 0xFF;
    fScale >> 8;
    context->adsInitData.fScaleM = fScale & 0xFF;
    fScale >> 8;
    context->adsInitData.fScaleH = fScale & 0xFF;
    ConfigureDevice(adsHandle);
}

ADS_OPERATION_STATUS SetGain(DRV_HANDLE adsHandle, unsigned gainValue)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    context->adsInitData.pga.gain = gainValue;
    ADS_OPERATION_STATUS result = ConfigureDevice(adsHandle);
    return result;
}

ADS_OPERATION_STATUS SetDigitalFilter(DRV_HANDLE adsHandle, unsigned filter)
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    context->adsInitData.mode0.filter = filter;
    ADS_OPERATION_STATUS result = ConfigureDevice(adsHandle);
    return result;
}

ADS_OPERATION_STATUS DRDYHandler(DRV_HANDLE adsHandle)
{
    
    ADSContext *context = (ADSContext *)adsHandle;
    
    if (context->suspendReading) return;
    
    return ReadData(adsHandle);
}

bool GetData(DRV_HANDLE adsHandle, uint32_t *data)
{
    if(!VerifyHande(adsHandle)) return false;  
    ADSContext *context = (ADSContext *)adsHandle;
    uint32_t currentCount;
    SYS_INT_Disable();
    if (context->dataCount == 0)
    {
      SYS_INT_Enable();
      return false;
    }
    else
    {
        context->dataCount--;
        SYS_INT_Enable();
    }
    
    *data = context->readBuffer[context->readIndex];
    context->readIndex = (context->readIndex == (BUFFER_SIZE - 1)) ? 0 : context->readIndex + 1;
    
    return true;
}

ADS_OPERATION_STATUS SetInvalidResponseCallback(DRV_HANDLE adsHandle, void (*InvalidResponseCallback)(DRV_HANDLE adsHandle, ADS_OPERATION_STATUS status))
{
    if(!VerifyHande(adsHandle)) return ADS_INVALID_HANDLE;
    ADSContext *context = (ADSContext *)adsHandle;
    context->InvalidOperationCallback = InvalidResponseCallback;
    return ADS_COMPLETE;
}