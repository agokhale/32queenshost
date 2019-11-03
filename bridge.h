#ifndef _bridgeh
#define _bridgeh

#include <ctype.h>

#include "mutil.h"
#define kcmd_chirp 		0
#define kcmd_echo 		1
#define kcmd_twistart 		2
#define kcmd_twiresult 		3
#define kcmd_twiwrite		4
#define kcmd_i2scan		5
#define kcmd_twiread		6
#define kcmd_core 		7
#define kcmd_ok  		0x08
#define kcmd_ok  		0x08

#define kbridge_maxframesize 8192
#define kbridge_rxbufsize 8192
#define kbridge_stall_timeout_s 1

typedef struct brctx_s {
	int fd; // the serial port's file descriptor
	FILE  * 	rxtracefile;
	FILE  * 	txtracefile;
	u32 rxbytes;
	u32 txbytes;
	u32 txframes;
	u32 rxframes;
	struct last_frame_stat_s  
		{
		u32 writelen;
		u32 readbacklen; 
		u32 getframe_usec;
		} last_frame_stat ;
	} brctx_t;	
#endif //_bridgeh
//______________________private calls

//_____________________bridge api
int bridge_do (brctx_t* , u8* inbuf, u8** outbuf); //semi blocking call 
brctx_t *  bridge_initctx ( const char * serialdevpath, unsigned int baudrate);
void bridge_flush( brctx_t* ); 
void bridge_dump_frame( u8 * );
void bridge_await_life ( struct brctx_s*); 
int bridge_getframe (brctx_t* , u8**  outbuf); 
void bridge_try_chirp( brctx_t * ); 
