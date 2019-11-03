#ifndef _i2h
#define _i2h
#include "bridge.h"

struct i2msg_s 
	{ 
	u8 retc;  
	u8 cmd;
	size_t len;  
	u8 addr; 
	u8 *data; 
	} __packed;
typedef struct i2msg_s i2msg; 

#define ki2cmd_start 2
#define ki2cmd_statusret 3
#define ki2cmd_write 4 
#define ki2cmd_read 6
#define ki2cmd_stop 0xe 
#define ki2cmd_tx_once 0xf 

void 		i2dump ( i2msg * );
void 		i2dump_test (); 
char* 		i2perror ( u8 );
i2msg * 	i2alloc ( size_t  );
void 		i2free ( i2msg*);
i2msg * 	i2tx (int ,  i2msg * );
i2msg * 	i2corereq ( int ); 
int		i2write (int,  i2msg*);
i2msg*		i2read (int,  i2msg*);
u8* 		i2busscan ( ); 
i2msg *  	i2msg_do ( brctx_t *, i2msg*);
void		test_i2msgrx ( u8 addr); 
void		test_i2msgtx (u8 addr); 
#endif //________________________i2h //_+
