#include "mutil.h"
#include "bridge.h"
#include "i2.h"
#define kbridgecoresize 0x130
#define kbridgecoretimeout 30564
#define ki2writetime 10000
char * i2perror ( u8 ie)
	{
	char * obuf = calloc ( 100, sizeof(char)); 
	switch (ie)
		{
		case 0x08 : sprintf (obuf ,"start txok"); break;
		case 0x10 : sprintf (obuf ,"repstart txok"); break;
		case 0x18 : sprintf (obuf ,"addr txok"); break;
		case 0x20 : sprintf (obuf ,"slave write nack - is chip here? "); break;
		case 0x28 : sprintf (obuf ,"datatxok"); break;
		case 0x30 : sprintf (obuf ,"datatxnack"); break;
		case 0x38 : sprintf (obuf ,"address arb lost"); break;
		case 0x40 : sprintf (obuf ,"sla+r ok"); break;
		case 0x48 : sprintf (obuf ,"nack"); break;
		case 0x50 : sprintf (obuf ,"data rxok"); break;
		case 0x58 : sprintf (obuf ,"data rxerr nacktx"); break;
		case 0x68 : sprintf (obuf ,"arblost ??"); break;
		case 0x88 : sprintf (obuf ,"sla-w nack "); break;
		case 0xf8 : sprintf (obuf ,"no info"); break;
		default: sprintf (obuf ,"huh ?:%2x", ie);
		}
	return (obuf);
	}
	
void i2dumpi2err (i2msg *m )
	{
	whisper ('8', "i2c error: %x, %s", m->data[4], i2perror (m->data[4])); 
	}

void i2dump ( i2msg * m) 
{
assertp (m); 	
whisper ('8' , "i2dump : address: 0x%x len: 0x%x", m->addr, m->len); 
switch (m->retc)
	{
	case ki2cmd_statusret: 
		whisper ( '3' ,"i2c status cods %x %s",m->data[0], i2perror ( m->data[0])); 	
	break;
	case ki2cmd_write:
		whisper ( '3' ,"i2c write ok ");
	break;

	break;	
	default: 
		whisper ('8',"data:%s", mprettyhex ( m->data, m->len)); 
		break;
	}
}
u32 gi2msgalloccount;
//i2alloc sets up a structure in the format that is digestible to the i2c and bridge 
// logic
i2msg* i2alloc  (size_t siz)
//siz is the length of the allocated data
	{
	i2msg * retc; 			
	retc = malloc ( sizeof ( i2msg )); 
	retc->data  = calloc (sizeof(u8), siz ); 
	retc->len = siz;
	gi2msgalloccount ++; 
	return (retc);
	}
void i2free ( i2msg * im)
	{
	assertp ( im); 	
	assertp ( im->data); 
	free (im->data);		
	im->data = NULL;	
	gi2msgalloccount --; 
	assert ( gi2msgalloccount > 0); 
	free (im); 
	}
//________________________________________________________________
// returns an u8[128] where 1 means there was a device that acknowleged the address
u8* i2busscan ( brctx_t *  brctx)
	{
	u8 ibuf[] = {0x01, 0x05} ; //dotwiscan from main.asm
	u8 *obuf = NULL ;
	u8 *twiscan_result; //aligned to the twi scan results
	u8 *bus_status;
	u32 i =0; 
	wtp;	
	printf ("starting busscan\n");
	bus_status = calloc ( 128, sizeof(u8));
	assert ( 0 == bridge_do( brctx, ibuf,&obuf)); 
	assertp ( obuf ); 
	twiscan_result = obuf+1; // payload starts right after size
	// the frame looks like 0x80, .......
	if ( obuf[0] != 0x80 ) 
		{
		whisper ('f' , "bus scan frame maformed"); 
		bridge_dump_frame ( obuf); 
		//assert(0); 
		}
	for (i = 0; i <= 0xff; i+=2)
		{
		if (twiscan_result[i >> 1] != 0x20) // not slave addr nack?
			{
			char * descstring = NULL; 
			descstring = i2perror ( twiscan_result[i >> 1]); 
			// read addresses are all even, so multiply the index by 2 
			// to 
			u8 twi_addr = i ;
			whisper ( '6', "addrress :0x%x :%s", twi_addr, descstring); 
			free (descstring) ; 
			bus_status [ i >> 1 ] = 1; 
			}
		}
	wtm;
	free (obuf); 
	return ( bus_status ); 
	}
//________________________________________________________________
i2msg* i2msg_do  (brctx_t * brctx, i2msg* inmsg)
{
i2msg * ret = NULL;
u8 * ibuf = calloc (kbridge_rxbufsize, sizeof (u8)); 
u8 * obuf = NULL; 
assert ( (inmsg->addr & 0x01 ) == 0);  //addresses must be even
switch ( inmsg->cmd )
	{
	case ki2cmd_write: 
		ibuf[0] = inmsg->len + 3;
		ibuf[1] = ki2cmd_write;
		ibuf[2] = inmsg->addr;
		bcopy ( inmsg->data, ibuf+3 , inmsg->len) ;
		ret = i2alloc ( 1 ); 
	break;
	case ki2cmd_read:
		ibuf[0] = 0x04;
		ibuf[1] = ki2cmd_read;
		ibuf[2] = inmsg->addr;
		// we pass the write address and the bridge calculates the read addres by adding one
		ibuf[4] = inmsg->len; // that's the readback length, not the request length
		ret = i2alloc ( inmsg->len ); 
	break;
	default:
		whisper ('4', "unknown i2msg command selector 0x%x" , inmsg->cmd); 
		assert (-1); 
		exit (1); 
	}
bridge_do ( brctx, ibuf , &obuf ); 
bcopy (obuf+2, ret->data , inmsg->len); 
ret->retc = obuf[1];
ret->addr = inmsg->addr+1;

switch ( inmsg->cmd )
	{
	case ki2cmd_read:
		// The output must be of the size that we asked for+1 (for 0x06) or a twi error
		if (!( (obuf[0] == inmsg->len + 1 ) || (obuf[1] == 0x03 ) ))
			{ wtp;
			whisper ( '4',"read results malformed address:0x%x", inmsg->addr); 
			if (whisper_worthy('8'))
				{
				whisper ('7', "i2msgdo message:"); bridge_dump_frame (ibuf);
				whisper ('7', "i2msgdo result:"); bridge_dump_frame ( obuf);
				}
			wtm; }
		break;
	case ki2cmd_write:
		if ( obuf[0] != 3 || obuf[1] != ki2cmd_write || obuf[2] != 0 )
			{
			whisper ('4', "write result malformed address: 0x%x", inmsg->addr); 
			if  ( obuf[1] == kcmd_twiresult ) 
				// we can get an twiresult 0x03 instead of ok
				{
				whisper ( 'f' , "i2c errror %s", i2perror ( obuf[2])); 
				}
			else 	
				{
				whisper ('7', "i2msgdo message:"); bridge_dump_frame (ibuf);
				whisper ('7', "i2msgdo result:"); bridge_dump_frame ( obuf);
				}
			assert (0); 
			}
		break; 
	default:
	break;
	}

free (ibuf);
free (obuf); 
return (ret); 
}
