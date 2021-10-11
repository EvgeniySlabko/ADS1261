#ifndef CONFIGURATIONS_H
#define	CONFIGURATIONS_H

//Register addreses
#define ADS_ID_ADDR                     0x00
#define ADS_STATUS_ADDR                 0x01
#define ADS_MODE0_ADDR                  0x02
#define ADS_MODE1_ADDR                  0x03
#define ADS_MODE2_ADDR                  0x04
#define ADS_MODE3_ADDR                  0x05
#define ADS_REF_ADDR                    0x06

#define ADS_OFCAL0_ADDR                 0x07
#define ADS_OFCAL1_ADDR                 0x08
#define ADS_OFCAL2_ADDR                 0x09

#define ADS_FSCAL0_ADDR                 0x0A
#define ADS_FSCAL1_ADDR                 0x0B
#define ADS_FSCAL2_ADDR                 0x0C

#define ADS_IMUX_ADDR                   0x0D
#define ADS_IMAG_ADDR                   0x0E
#define ADS_PGA_ADDR                    0x10
#define ADS_INPMUX_ADDR                 0x11
#define ADS_INPBIAS_ADDR                0x12

// Commands
#define ADS_NOP                         0x00
#define ADS_RESET                       0x06
#define ADS_START                       0x08
#define ADS_STOP                        0x0A
#define ADS_RDATA                       0x12
#define ADS_SYOCAL                      0x16
#define ADS_GANCAL                      0x17
#define ADS_SFOCAL                      0x19
#define ADS_RREG                        0x20
#define ADS_WREG                        0x40
#define ADS_LOCK                        0xF2
#define ADS_UNLOCK                      0xF5

typedef struct Mode0 {
    unsigned filter: 3;
    unsigned dataRate: 5;
} Mode0;

typedef struct Mode1 {
    unsigned delay: 4;
    unsigned convrt: 1;
    unsigned chop: 2;
    unsigned : 1;
} Mode1;

typedef struct Mode2 {
    unsigned gpio_dir0: 1;
    unsigned gpio_dir1: 1;
    unsigned gpio_dir2: 1;
    unsigned gpio_dir3: 1;
    unsigned gpio_con0: 1;
    unsigned gpio_con1: 1;
    unsigned gpio_con2: 1;
    unsigned gpio_con3: 1;
} Mode2;

typedef struct Mode3 {
    unsigned gpio_dat0: 1;
    unsigned gpio_dat1: 1;
    unsigned gpio_dat2: 1;
    unsigned gpio_dat3: 1;
    unsigned spitim: 1;
    unsigned crcenb: 1;
    unsigned statent: 1;
    unsigned pwdn: 1;
} Mode3;

typedef struct Ref {
    unsigned rmuxn: 2;
    unsigned rmuxp: 2;
    unsigned refenb: 1;
    unsigned : 3;
} Ref;

typedef struct IMux {
    unsigned imux1: 4;
    unsigned imux2: 4;
} IMux;

typedef struct IMag {
    unsigned imag1: 4;
    unsigned iag2: 4;
} IMag;

typedef struct PGA {
    unsigned gain: 3;
    unsigned : 4;
    unsigned bypass: 1;
} PGA;

typedef struct InpMux {
    unsigned muxn: 4;
    unsigned muxp: 4;
} InpMux;

typedef struct InpBias {
    unsigned bocs: 3;
    unsigned bocsp: 1;
    unsigned vbias: 1;
    unsigned : 3;
} InpBias;

typedef struct ADSInitData
{
    Mode0 mode0;
    Mode1 mode1;
    Mode2 mode2;
    Mode3 mode3;
    Ref ref;
    IMux imux;
    IMag imag;
    PGA pga;
    InpMux inpmux;
    InpBias inpbias;
    
    uint8_t ofScaleL;
    uint8_t ofScaleM;
    uint8_t ofScaleH;
    
    uint8_t fScaleL;
    uint8_t fScaleM;
    uint8_t fScaleH;
    
} ADSInitData;

// Configurations.

//DataRate
#define DR1 0b00000  // 2.5 SPS
#define DR2 0b00001  // 5 SPS
#define DR3 0b00010  // 10 SPS
#define DR4 0b00011  // 16.6 SPS
#define DR5 0b00100  // 20 SPS (default)
#define DR6 0b00101  // 50 SPS
#define DR7 0b00110  // 60 SPS
#define DR8 0b00111  // 100 SPS
#define DR9 0b01000  // 400 SPS
#define DR10 0b01001 // 1200 SPS
#define DR11 0b01010 // 2400 SPS
#define DR12 0b01011 // 4800 SPS
#define DR13 0b01100 // 7200 SPS
#define DR14 0b01101 // 14400 SPS
#define DR15 0b01110 // 19200 SPS
#define DR16 0b01111 // 25600 SPS
#define DR17 0b10000  // 11111: 40000 SPS (fCLK = 10.24 MHz)

//DigitalFilter
#define DF1 0b000       // sinc1
#define DF2 0b001       // sinc2
#define DF3 0b010       // sinc3
#define DF4 0b011       // sinc4
#define DF5 0b100       // FIR (default)

//Conversation start delay 
#define CD1 0000 // 0 탎 (not for 25600 SPS or 40000 SPS operation)
#define CD2 0001 //  50 탎 (default)
#define CD3 0010 //  59 탎
#define CD4 0011 //  67 탎
#define CD5 0100 //  85 탎
#define CD6 0101 //  119 탎
#define CD7 0110 //  189 탎
#define CD8 0111 // 328 탎
#define CD9 1000 //  605 탎
#define CD10 1001 //  1.16 ms
#define CD11 1010 //  2.27 ms
#define CD12 1011 //  4.49 ms
#define CD13 1100 //  8.93 ms
#define CD14 1101 //  17.8 ms

//Select the gain.
#define GL1 0b000  //: 1 (default)
#define GL2 0b001   //: 2
#define GL3 0b010   //: 4
#define GL4 0b011   //: 8
#define GL5 0b100   //: 16
#define GL6 0b101   //: 32
#define GL7 0b110   //: 64
#define GL8 0b111   //: 128

// Conversion mode
#define CONTINUOUS_CONVERSION_MODE       0b0    //0: Continuous conversions (default)
#define PUSLSE_CONVERSION_MODE           0b1    //1: Pulse (one shot) conversion



#endif	/* CONFIGURATIONS_H */

