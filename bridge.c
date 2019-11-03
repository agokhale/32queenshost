#include "muserial.h"
#include "bridge.h"
#include <ctype.h>
//__________________________________________________________________________
/* Prepare a nymph based bridge for use
	- open its io
	- prepare debugging logs
	- clean up stale data in the buffer
	- verify it's running and understand some timing data
	return a running bridge
*/

struct brctx_s * bridge_initctx ( const char * serialdevpath, unsigned int baudrate)
	{
	wtp;
	int chirpcount = 30; 
	struct brctx_s * brctx = malloc ( sizeof ( struct brctx_s ) ); 
	//brctx.serial_fd = muserial_init ( getenv ("serialdev"),250000, O_RDWR | O_NOCTTY | O_NDELAY  ); 
	brctx->fd = muserial_init ( (char*) serialdevpath, baudrate, O_RDWR | O_NOCTTY | O_NDELAY  ); 
	muserial_flush ( brctx->fd );  // discard input that happened in the past
	brctx->rxtracefile  = fopen ( "rxtrace.trace", "w");
	assertp (brctx->rxtracefile ); 
	brctx->txtracefile  = fopen ( "txtrace.trace", "w");
	assertp (brctx->txtracefile ); 
	bridge_await_life ( brctx ); 
	while ( chirpcount -- )
		{
		bridge_try_chirp( brctx ); 
		}
	wtm;
	return brctx; 
	}

//__________________________________________________________________________
void bridge_try_chirp ( struct brctx_s *brctx) 
{
u8 chirp_cmd_frame[2] ;
u8 *chirp_response; 
whisper ('9', "chiriping to chip"); 
chirp_cmd_frame[0] = 0x02;
chirp_cmd_frame[1] = 0x00;
bridge_do ( brctx,  chirp_cmd_frame, &chirp_response ); 
if ( chirp_response[1] != 0x03)  // full responce is 0x02, 0x03
	{
	whisper ('f' , "chirp return malformed; command processor not available"); 
	bridge_dump_frame( chirp_response); 
	muserial_flush ( brctx->fd); 
	assertp ( 0 ); 
	} 
}

//__________________________________________________________________________
int bridge_getframe (brctx_t *  brctx, u8**  outbuf) 
// outbuf is a handle to an unallocated u8[] will allocate 
// return 0 on success
// a frame is thusly 
//  f[0] the total  size of the  frame including the allocation for it's self's size 
//  f [.....]
//  f [ f[0] - 1 ] // last index is f[0] - 1
// eg  : 07 0a68656c 6c6f 
// aka : 0x07'\nhello'
{
int ret = -1;
int readlen = -1;
int sleep_iterations = 0; 
struct timeval starttime;
struct timeval endtime;
struct timeval difftime;
u8 done = 0;
int input_cursor_byte  = 0 ;
gettimeofday ( &starttime, NULL );  
*outbuf = calloc ( kbridge_maxframesize,1 ); //allocate the output buffer 
while ( !done )
	{
	readlen = muserial_softread (brctx->fd,  (*outbuf)+input_cursor_byte ,  kbridge_maxframesize , 4000 /*uSec*/) ;
	if ( readlen > 0  ) 
		{
		fwrite (  (*outbuf)+input_cursor_byte, sizeof(u8),  readlen, brctx->rxtracefile); //copy out to the tracefile
		input_cursor_byte += readlen;	
		// Compare the claimed frame length  (first buffer byte) to the cursor
		// the cursor will have walked off the end of the frame so 
		if  (input_cursor_byte == (int) ** outbuf)  
			{
			// This frame verifies
			gettimeofday ( &endtime , NULL ); 
			timersub ( &endtime, &starttime , &difftime ); 
			whisper ('9', "frame verification complete in %is +%i usec ", difftime.tv_sec, difftime.tv_usec ); 
			if ( muserial_rx_qdepth( brctx->fd ) > 0 ) 
				{
				whisper ('f', " serial rx q depth is %i is something wrong? ",  muserial_rx_qdepth( brctx->fd )); 
				assert ( 0 ); 
				}
			ret = 0; 	
			break;
			}
		}
	else	 
		{
		errno = 0; 
		}
	sleep_iterations++; 
	usleep (10000); 
	gettimeofday ( &endtime , NULL ); 
	timersub ( &endtime, &starttime , &difftime ); 
	if ( (difftime.tv_usec > 330000)  && (input_cursor_byte > 0) )  // 300ms is too short - usb rtt may be interfering
		{ wtp;
		ret = -1; 
		whisper ('6',"bridge stall seconds: %i +  %i usec , qdepth: %i", 
			difftime.tv_sec, difftime.tv_usec, muserial_rx_qdepth ( brctx->fd ));
		whisper ('6',"buffer received bytes:0x%x expected: 0x%x", 
			input_cursor_byte,((**outbuf) & 0xff) + 1 ); 
		whisper ('6',"sleeps:  %i", sleep_iterations); 
		whisper ('6',"bridge buffer contents: "); 
		whisper ('6',"%s", mprettyhex ((u8*)*outbuf, 0x20) ); 
		muserial_flush( brctx->fd );
		assert ( 0 ); 
		wtm;
		}
	}

fflush (brctx->rxtracefile);
fflush (brctx->txtracefile);
return (ret); 
}
//__________________________________________________________________________
int bridge_do (brctx_t* brctx, u8* inbuf, u8** outbuf)
//called from OUTSIDE the bridge framework mostly!!!
{
wtp;
int ret =  1;
size_t framesize = *inbuf; // The first byte is the framesize
if (inbuf[0] == 0 )  
	{
	whisper ('f', "bridge cowardly refuses to write 0 bytes");
	if (whisper_worthy ('9')) bridge_dump_frame( inbuf); 
	assert ( 0 ); 
	}
if (whisper_worthy ('9')) 
	{ bridge_dump_frame( inbuf); }
assert( write ( brctx->fd, inbuf, framesize ) ); // write the frame payload
// and copy it to the tracefile 
assert( fwrite (  inbuf, sizeof(u8), framesize, brctx->txtracefile) );
ret =  bridge_getframe ( brctx, outbuf ); 
wtm;
return (ret); 
}

//___________________________________________________________________________
void bridge_dump_frame (u8* p)
	{
	char * rdesc;
	printf ("frame[len:0x%x]---------------------------------------------------------- \n", p[0]);
	rdesc = mprettyhex(p+1, p[0] - 1);  //start at [p[1]]  and run to p[0] - 1
	printf ("%s \n",rdesc); 
	free (rdesc); 
	}
//___________________________________________________________________________
void bridge_await_life (  brctx_t *  brctx) 
{
const u8 helloframe_control[] = {0x08, '\n','h','e','l','l','o', 0x7e}; 
//  is 0x7e going to flush the ftdi bridge chip? there is an ftdi technote about this
//http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-04_DataLatencyFlow.pdf
u8 **first_frame_handle;
u8 *ffptr; 
first_frame_handle = &ffptr;
int done = 0; 
whisper ( '3', "awaiting bridge hello" ); 
wtp;
while ( ! done )
	{
	if ( 0 == bridge_getframe ( brctx, first_frame_handle)  ) 
		{
		if  ( memcmp( ffptr, helloframe_control, helloframe_control[0] ) != 0  )	
			{
			whisper('f', "bridge hello malformed, helloframe is not %s", mprettyhex (helloframe_control, helloframe_control[0])) ; 
			whisper ('f', " recieved frame instead is this"); 
			bridge_dump_frame (ffptr);
			assert( 0 ); 
			}
		done = 1; 
		}
	}
wtm;
free (*first_frame_handle);
}

//________________________________________________________________________________________________
void bridge_flush ( brctx_t * brctx ) 
{
	size_t  const kflush_force_size = 32768;
	int flushed_byte_count; 
	int flush_flags = 1; 
	char * flush_recovered_bytes;
	whisper ( '6', " fnord tty reports %i flushable bytes", muserial_rx_qdepth( brctx->fd )); 
	// pedantic fluush  there is a quieter one in muserial_flush	
	flush_recovered_bytes = calloc(  kflush_force_size , 1); 
	flushed_byte_count = read ( brctx->fd , flush_recovered_bytes, kflush_force_size ); 
	if ( flushed_byte_count == kflush_force_size ) {
		whisper ( '3', "possible incomplete flush"); 
	}
	whisper ( '3', "flushed %i bytes", flushed_byte_count); 

	errno = 0; 
	assert( 0 == ioctl (brctx->fd,  TIOCFLUSH, &flush_flags) );  //are we sure we flushed before leaving?
	free( flush_recovered_bytes); 
} 
