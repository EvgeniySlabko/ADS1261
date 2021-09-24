#include "ADS1261.h"
#include "Delay.h"
#include "app.h"

ADSInitData ads_init_data;
DRV_HANDLE spiHandle;

void Init_ADS1261(DRV_HANDLE spiHandle)
{
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
    ads_init_data.mode0.dataRate = DR5;
    
//    Select the digital filter mode.
//    000: sinc1
//    001: sinc2
//    010: sinc3
//    011: sinc4
//    100: FIR (default)
//    101: Reserved
//    110: Reserved
//    111: Reserved
    ads_init_data.mode0.filter = 0b100;

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
    ads_init_data.mode1.delay = 0b0001;
//Chop and AC-Excitation Modes
//Select the Chop and AC-excitation modes.
//00: Normal mode (default)
//01: Chop mode
//10: 2-wire AC-excitation mode ( ADS1261 only)
//11: 4-wire AC-excitation mode ( ADS1261 only)

    ads_init_data.mode1.chop = 0b11;
// ADC Conversion Mode
    ads_init_data.mode1.convrt = 0b0; // 0: Continuous conversions (default)
// GPIO control. Set AN1 and AN2 as analog inputs
    ads_init_data.mode2.gpio_con2 = 1;
    ads_init_data.mode2.gpio_con3 = 1;
    //ads_init_data.mode2.gpio_con0 = 1;
    //ads_init_data.mode2.gpio_con1 = 1;
    
    ads_init_data.ref.rmuxn = 0b10;
    ads_init_data.ref.rmuxp = 0b10;
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
ads_init_data.pga.gain = 0b111;
 

ads_init_data.inpmux.muxp = 0b0011;
ads_init_data.inpmux.muxn = 0b0100;

WriteRegisterByte(spiHandle, MODE0_ADDR, *(uint8_t*) &ads_init_data.mode0);
//DelayInMillisecond(1);

DelayInMillisecond(1);
WriteRegisterByte(spiHandle, MODE2_ADDR, *(uint8_t*) &ads_init_data.mode2);
DelayInMillisecond(1);
WriteRegisterByte(spiHandle, REF_ADDR, *(uint8_t*) &ads_init_data.ref);
WriteRegisterByte(spiHandle, PGA_ADDR, *(uint8_t*) &ads_init_data.pga);
WriteRegisterByte(spiHandle, INPMUX_ADDR, *(uint8_t*) &ads_init_data.inpmux);

WriteRegisterByte(spiHandle, MODE1_ADDR, *(uint8_t*) &ads_init_data.mode1);
}

bool WriteRegisterByte(DRV_HANDLE spiHandle, uint8_t address, uint8_t data)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    //uint8_t data1 = 0b00000001; //data reg
    uint8_t operation = 0x40 + address;
    uint8_t nop = 0x00;
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
    uint8_t echoByte2; // Echo for data
           
    DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, sizeof(ffByte), 0, 0, 0); 
    DRV_SPI_BufferAddWriteRead2(spiHandle, &data, sizeof(data), &echoByte1, 1, 0, 0, 0);
    //Delay50ns();
    //DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &echoByte2, 1, 0, 0, 0);
     
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
    
    return ((ffByte = 0xFF) && (echoByte1 == operation));
}

uint8_t ReadRegisterByte(DRV_HANDLE spiHandle, uint8_t address, uint8_t *data)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    //uint8_t data1 = 0b00000001; //data reg
    uint8_t operation = 0x20 + address;
    uint8_t nop = 0x00;
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
    uint8_t dataByte; // Echo for data
      
    DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, sizeof(ffByte), 0, 0, 0);     
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &echoByte1, 1, 0, 0, 0);
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &dataByte, 1, 0, 0, 0);
     
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
    
    *data = dataByte;
    return ((ffByte = 0xFF) && (echoByte1 == nop) && (dataByte == *data));
}

void Start(DRV_HANDLE spiHandle)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    //uint8_t data1 = 0b00000001; //data reg
    uint8_t operation = 0x08;
    uint8_t nop = 0x00;
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
      
    DRV_SPI_BUFFER_HANDLE drvSPIBufferHandle;
    drvSPIBufferHandle = DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, sizeof(ffByte), 0, 0, 0);     
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &echoByte1, 1, 0, 0, 0);

     
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
}

void Stop(DRV_HANDLE spiHandle)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    //uint8_t data1 = 0b00000001; //data reg
    uint8_t operation = 0x0A;
    uint8_t nop = 0x00;
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
      
    DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, sizeof(ffByte), 0, 0, 0);     
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &echoByte1, 1, 0, 0, 0);

     
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
}

void Reset(DRV_HANDLE spiHandle)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    //uint8_t data1 = 0b00000001; //data reg
    uint8_t operation = 0x06;
    uint8_t nop = 0x00;
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
      
    DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, sizeof(ffByte), 0, 0, 0);     
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, sizeof(nop), &echoByte1, 1, 0, 0, 0);

     
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
}

uint32_t ReadData(DRV_HANDLE spiHandle, uint8_t *status)
{
    DelayInMillisecond(1);
    CS_PIN = 0;
    Delay50ns();
    
    uint32_t result = 0;
    
    uint8_t operation = 0x12;
    uint8_t nop = 0x00;
    
    uint8_t ffByte; // Must be 0xFF
    uint8_t echoByte1; // Echo for operation
    uint8_t statusByte; // Status
    
    uint8_t dataByte1;
    uint8_t dataByte2;
    uint8_t dataByte3;
    DRV_SPI_BufferAddWriteRead2(spiHandle, &operation, 1, &ffByte, 1, 0, 0, 0);     
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, 1, &echoByte1, 1, 0, 0, 0);
    
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, 1, &dataByte1, 1, 0, 0, 0);
    result += dataByte1;
    result = result << 8;
    
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, 1, &dataByte2, 1, 0, 0, 0);
    result += dataByte2;
    result = result << 8;
    
    DRV_SPI_BufferAddWriteRead2(spiHandle, &nop, 1, &dataByte3, 1, 0, 0, 0);
    result += dataByte3;
       
    DelayInMillisecond(1);
    CS_PIN = 1;
    DelayInMillisecond(1);
    
    return result;
}