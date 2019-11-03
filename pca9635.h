#ifndef _pca9635
#define  _pca9635
#include "mutil.h"
#include "bridge.h"
//	http://www.nxp.com/documents/data_sheet/PCA9635.pdf
//registers-----------------

#define pcareg_mode1 0
#define pcareg_mode1_autoinc2 	0x80 
#define pcareg_mode1_autoinc1 	0x40 
#define pcareg_mode1_autoinc0 	0x20 
#define pcareg_mode1_pwroff 	0x10
#define pcareg_mode1_sub2 	0x08
#define pcareg_mode1_sub1 	0x04
#define pcareg_mode1_sub0 	0x02
#define pcareg_mode1_allcall 	0x01


#define pcareg_mode2 1
#define pcareg_mode2_dmblink	0x20
#define pcareg_mode2_invert	0x10
#define pcareg_mode2_och_on_ack	0x08 	// or on stop
#define pcareg_mode2_totem	0x04  	// or opendrain 
#define pcareg_mode2_outne1	0x02  	//  enable drivers 
#define pcareg_mode2_outne0	0x01  	// 


#define pcareg_pwm(A)	((A < 16)? (2 + A): 17)
#define pcareg_grppwm	 0x12
#define pcareg_grpfreq	 0x13
#define pcareg_ledout0 0x14
#define pcareg_ledout1 0x15
#define pcareg_ledout2 0x16
#define pcareg_ledout3 0x17

/*
7.3.1 Mode register 1, MODE1 
Table 5. MODE1 - Mode register 1 (address 00h) bit description 
Legend: * default value. 
Bit Symbol Access Value Description 
7 AI2 read only 0 Register Auto-Increment disabled. 
1* Register Auto-Increment enabled. 
6 AI1 read only 0* Auto-Increment bit1=0. 
1 Auto-Increment bit1=1. 
5 AI0 read only 0* Auto-Increment bit0=0. 
1 Auto-Increment bit0=1. 
4 SLEEP R/W 0 Normal mode[1]. 
1* Low power mode. Oscillator off[2]. 
3 SUB1 R/W 0* PCA9635 does not respond to I2C-bus subaddress 1. 
1 PCA9635 responds to I2C-bus subaddress 1. 
2 SUB2 R/W 0* PCA9635 does not respond to I2C-bus subaddress 2. 
1 PCA9635 responds to I2C-bus subaddress 2. 
1 SUB3 R/W 0* PCA9635 does not respond to I2C-bus subaddress 3. 
1 PCA9635 responds to I2C-bus subaddress 3. 
0 ALLCALL R/W 0 PCA9635 does not respond to LED All Call I2C-bus address. 
7.3.2 Mode register 2, MODE2
Table 6. MODE2 - Mode register 2 (address 01h) bit description 

Bit Symbol Access Value Description 
7 - read only 0* reserved 
6 - read only 0* reserved 
5 DMBLNK R/W 0* group control = dimming. 
1 group control = blinking. 
4 INVRT[1] R/W 0* Output logic state not inverted. Value to use when no external driver used. 
Applicable whenOE=0. 
1 Output logic state inverted. Val
3 OCH R/W 0* Outputs change on STOP command.[2] 
1 Outputs change on ACK. 
2 OUTDRVR/W 0 The 16 LED outputs agured with an open-drain structure
1to0 OUTNE[1:0][3] R/W 00 WhenOE = 1 (output drivers not enabled), LEDn = 0. 
01* WhenOE = 1 (output drivers not enabled): 
LEDn = 1 when OUTDRV=1 
LEDn=high-impedance when OUTDRV=0 (same as OUTNE[1:0]=10) 
10 WhenOE = 1 (output drivers not enabled), LEDn = high-impedance. 
11 reserved 
Bit Symbol Access Value Description 


02 0 0 0 1 0 PWM0 read/write brightness control LED0 
03 0 0 0 1 1 PWM1 read/write brightness control LED1 
04 0 0 1 0 0 PWM2 read/write brightness control LED2 
05 0 0 1 0 1 PWM3 read/write brightness control LED3 
06 0 0 1 1 0 PWM4 read/write brightness control LED4 
07 0 0 1 1 1 PWM5 read/write brightness control LED5 
08 0 1 0 0 0 PWM6 read/write brightness control LED6 
09 0 1 0 0 1 PWM7 read/write brightness control LED7 
0A 0 1 0 1 0 PWM8 read/write brightness control LED8 
0B 0 1 0 1 1 PWM9 read/write brightness control LED9 
0C 0 1 1 0 0 PWM10 read/write brightness control LED10 
0D 0 1 1 0 1 PWM11 read/write brightness control LED11 
0E 0 1 1 1 0 PWM12 read/write brightness control LED12 
0F 0 1 1 1 1 PWM13 read/write brightness control LED13 
10 1 0 0 0 0 PWM14 read/write brightness control LED14 
11 1 0 0 0 1 PWM15 read/write brightness control LED15 
12 1 0 0 1 0 GRPPWM read/write group duty cycle control 
13 1 0 0 1 1 GRPFREQ read/write group frequency 
14 1 0 1 0 0 LEDOUT0 read/write LED output state 0 
15 1 0 1 0 1 LEDOUT1 read/write LED output state 1 
16 1 0 1 1 0 LEDOUT2 read/write LED output state 2 
17 1 0 1 1 1 LEDOUT3 read/write LED output state 3 
18 1 1 0 0 0 SUBADR1 read/write I2C-bus subaddress 1 
19 1 1 0 0 1 SUBADR2 read/write I2C-bus subaddress 2 
1A 1 1 0 1 0 SUBADR3 read/write I2C-bus subaddress 3 
1B 1 1 0 1 1 ALLCALLADR read/write LEDAll Call I2C-busaddress
*/

u8 pca9635_init (brctx_t *, u8  i2c_addr, u32 driver_mode ); 
#define kpca9635_driver_mode_totem_pole 0 
#define kpca9635_driver_mode_open_drain 1

void pca9635_set (brctx_t *,  u8 i2c_addr,  u8 *data ); 

#endif //_pca9635
