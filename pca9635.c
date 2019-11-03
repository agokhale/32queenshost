// you need this: http://www.nxp.com/documents/data_sheet/PCA9635.pdf
#include "pca9635.h"
#include "i2.h"
u8 pca9635_init ( brctx_t * brctx, u8 i2c_addr, u32 driver_mode )   
{
u8 retcode = 0; //XX use me

i2msg *cmd ;
i2msg *response; 

cmd  = i2alloc ( 0x19 ); 

cmd->cmd = ki2cmd_write; // i2.h
cmd->addr = i2c_addr;

//control register - use autoincrement start at register 0
cmd->data[0]  = 0x80 ;
//mode 1 register config: register 0x00
cmd->data[1]  = pcareg_mode1_autoinc2 ;
//mode 2, register 0x01
cmd->data[2] = pcareg_mode2_outne0;
switch ( driver_mode ) {
	case kpca9635_driver_mode_totem_pole:
		cmd->data[2] |= pcareg_mode2_totem;
		break;
	case kpca9635_driver_mode_open_drain: break;
	default:
		break;
	}
for ( int i = 0x03  ; i < 0x13; i++ )
	{
	cmd->data[i] = 0x10;
	}

cmd->data[0x13] = 0xa5;
cmd->data[0x14] = 0x5a;
//led registers are 00 for this purpose
//ledout driver state are active and controlled via the pwm register ( see p13 )
cmd->data[0x15] = cmd->data[0x16] = cmd->data[0x17] = cmd->data[0x18] = 0xaa;
response = i2msg_do (brctx,  cmd) ; 
return (retcode); 
}

//______________________________________________________________________________
// write all 16 registers
void pca9635_set ( brctx_t * brctx, u8 i2c_addr , u8* data) 
{
i2msg *cmd;
i2msg *response;
cmd  = i2alloc ( 0x11 ); 
cmd->cmd = ki2cmd_write; // i2.h
cmd->addr = i2c_addr;
cmd->data[0] = 0x82; // start at the PWM0 register
bcopy (data , cmd->data+1, 16);
response = i2msg_do (brctx , cmd) ; 
}


