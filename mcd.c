#include "mutil.h"
#include "bridge.h"
#include "i2.h"
#include "colortx.h"
#include "bridge.h"
#include "pca9635.h"
#include "pca9548.h"
#include <math.h>
#include "tiffio.h"
void scan_and_poke (); 
void load_framebuffer();


u32 framebuffer[32][32];
brctx_t *brctx ; 

void load_framebuffer()
{

int height;
int width;
int scanlinesize; 

char * linebuf;
whisper ('1', "loading framebuffer"); 

TIFF * tfile = TIFFOpen ( "inframe.tif","r"); 

if ( !tfile  ) { exit (-88); }

TIFFGetField(tfile, TIFFTAG_IMAGELENGTH, &height);
TIFFGetField(tfile, TIFFTAG_IMAGEWIDTH, &width);
whisper ('3', "image dim: 0x%x, 0x%x ", height, width);
scanlinesize =  TIFFScanlineSize(tfile); 
whisper ( '3', "scanline size 0x%x", scanlinesize); 
linebuf = _TIFFmalloc(TIFFScanlineSize(tfile));

for (int linecur = 0; linecur < height ; linecur ++)
	{
	int res = -1; 
	res = TIFFReadScanline ( tfile, linebuf, linecur, 0);
	//printf ("\n%x.\n",linecur);
	for ( int pcur = 0 ; pcur < scanlinesize; )
		{ 
			// red
			framebuffer [pcur/3][linecur] = (linebuf[pcur] & 0xff) << 16 ; 
			printf ( "%x", (linebuf[pcur++] & 0xff)>>4); 
			// green
			framebuffer [pcur/3][linecur] |= (linebuf[pcur] & 0xff) << 8 ; 
			printf ( "%x", (linebuf[pcur++] & 0xff)>>4); 
			//blue
			framebuffer [pcur/3][linecur] |= (linebuf[pcur] & 0xff) ; 
			printf ( "%x|", (linebuf[pcur++] & 0xff)>>4); 
		}
		
	}

whisper ('3'," uh ok"); 
TIFFClose ( tfile); 
errno =0; 
}


void clearframebuffer ()
{
	whisper ('5',"clear framebuffer"); 
	for ( int i=0; i < 32; i++)
		{
		for (int j =0; j < 32; j++)
			framebuffer [i][j] = 0;
		}
	
}
void dumpframebuffer ()
{
	wtp;
	int i, j;
	whisper ('5',"dumping framebuffer"); 
	for ( i=0; i < 32; i++)
		{
		whisper ('5',"framebuffer row %x", i); 
		wtp;
		for (j =0; j < 32; j++)
			{ 
			whisper ('3', "%x:%x", j, framebuffer[i][j]); 
			}
		printf ("\n"); 
		wtm;
		}
	whisper ('5',"donedumping framebuffer %x*%x", i,j); 
	wtm;
	
}




void unitwrite (u8 addr,  u8 startx, u8 starty) 
{

double gain; 
u8 ledpwm [16];
gain = 0.005;
whisper ( '2', "unitwrite "); 

for ( u8 rr = 15; rr != 0 ; rr -- )
	{ ledpwm [rr] = 0x00;} 

ledpwm[0] = x_white (framebuffer[startx][starty] );
ledpwm[1] = x_white (framebuffer[startx][starty-1] );
ledpwm[2] = x_white (framebuffer[startx][starty-2] );
ledpwm[3] = x_white (framebuffer[startx][starty-3] );

ledpwm[4] = x_yellow (framebuffer[startx][starty] );
ledpwm[5] = x_blue (framebuffer[startx][starty-1] );
ledpwm[6] = x_green (framebuffer[startx][starty-2] );
ledpwm[7] = x_red (framebuffer[startx][starty-3] );

ledpwm[8] = x_yellow (framebuffer[startx][starty-4] );
ledpwm[9] = x_blue (framebuffer[startx][starty-5] );
ledpwm[10] = x_green (framebuffer[startx][starty-6] );
ledpwm[11] = x_red (framebuffer[startx][starty-7] );

ledpwm[15] = x_white (framebuffer[startx][starty-4] );
ledpwm[12] = x_white (framebuffer[startx][starty-5] );
ledpwm[13] = x_white (framebuffer[startx][starty-6] );
ledpwm[14] = x_white (framebuffer[startx][starty-7] );
pca9635_set (brctx, addr, ledpwm); 
}
void write_array()
{
	whisper ('1', "set page"); 
	pca9548_set_page(brctx,0xee, 1);
	whisper ('1', "writing units Left front plane"); 
	unitwrite (0x10, 0,31);
	unitwrite (0x12, 0,23);
	unitwrite (0x14, 0,15);
	unitwrite (0x16, 0,7);

	unitwrite (0x18, 2,31);
	unitwrite (0x1a, 2,23);
	unitwrite (0x1c, 2,15);
	unitwrite (0x1e, 2,7);

	unitwrite (0x20, 4,31);
	unitwrite (0x22, 4,23);
	unitwrite (0x24, 4,15);
	unitwrite (0x26, 4,7);

	unitwrite (0x28, 6,31);
	unitwrite (0x2a, 6,23);
	unitwrite (0x2c, 6,15);
	unitwrite (0x2e, 6,7);

	whisper ('1', "writing units Left back plane"); 
	pca9548_set_page(brctx,0xee, 5);
	unitwrite (0x10, 1,31);
	unitwrite (0x12, 1,23);
	unitwrite (0x14, 1,15);
	unitwrite (0x16, 1,7);

	unitwrite (0x18, 3,31);
	unitwrite (0x1a, 3,23);
	unitwrite (0x1c, 3,15);
	unitwrite (0x1e, 3,7);

	unitwrite (0x20, 5,31);
	unitwrite (0x22, 5,23);
	unitwrite (0x24, 5,15);
	unitwrite (0x26, 5,7);

	unitwrite (0x28, 7,31);
	unitwrite (0x2a, 7,23);
	unitwrite (0x2c, 7,15);
	unitwrite (0x2e, 7,7);

	whisper ('1', "writing units mid Left front plane"); 
	pca9548_set_page(brctx,0xee, 0);

	unitwrite (0x10, 8,31);
	unitwrite (0x12, 8,23);
	unitwrite (0x14, 8,15);
	unitwrite (0x16, 8,7);

	unitwrite (0x18, 10,31);
	unitwrite (0x1a, 10,23);
	unitwrite (0x1c, 10,15);
	unitwrite (0x1e, 10,7);

	unitwrite (0x20, 12,31);
	unitwrite (0x22, 12,23);
	unitwrite (0x24, 12,15);
	unitwrite (0x26, 12,7);

	unitwrite (0x2c, 14,31); // order is fracked  in hardwareXXXXXXX
	unitwrite (0x28, 14,23);
	unitwrite (0x2a, 14,15);
	unitwrite (0x2e, 14,7);

	whisper ('1', "writing units mid Left back plane"); 
	pca9548_set_page(brctx,0xee, 4);

	unitwrite (0x10, 9,31);
	unitwrite (0x12, 9,23);
	unitwrite (0x14, 9,15);
	unitwrite (0x16, 9,7);

	unitwrite (0x18, 11,31);
	unitwrite (0x1a, 11,23);
	unitwrite (0x1c, 11,15);
	unitwrite (0x1e, 11,7);

	unitwrite (0x20, 13,31);
	unitwrite (0x22, 13,23);
	unitwrite (0x24, 13,15);
	unitwrite (0x26, 13,7);

	unitwrite (0x28, 15,31);
	unitwrite (0x2a, 15,23);
	unitwrite (0x2c, 15,15);
	unitwrite (0x2e, 15,7);

	whisper ('1', "writing units mid right front plane"); 
	pca9548_set_page(brctx,0xee, 6);

	unitwrite (0x10, 16,31);
	unitwrite (0x12, 16,23);
	unitwrite (0x14, 16,15);
	unitwrite (0x16, 16,7);

	unitwrite (0x18, 18,31);
	unitwrite (0x1a, 18,23);
	unitwrite (0x1c, 18,15);
	unitwrite (0x1e, 18,7);

	unitwrite (0x20, 20,31);
	unitwrite (0x22, 20,23);
	unitwrite (0x24, 20,15);
	unitwrite (0x26, 20,7);

	unitwrite (0x28, 22,31); 
	unitwrite (0x2a, 22,23);
	unitwrite (0x2c, 22,15);
	unitwrite (0x2e, 22,7);

	
	whisper ('1', "writing units mid right back plane"); 
	pca9548_set_page(brctx,0xee, 3);

	unitwrite (0x10, 17,31);
	unitwrite (0x12, 17,23);
	unitwrite (0x14, 17,15);
	unitwrite (0x16, 17,7);

	unitwrite (0x18, 19,31);
	unitwrite (0x1a, 19,23);
	unitwrite (0x1c, 19,15);
	unitwrite (0x1e, 19,7);

	unitwrite (0x20, 21,31);
	unitwrite (0x22, 21,23);
	unitwrite (0x24, 21,15);
	unitwrite (0x26, 21,7);

	unitwrite (0x28, 23,31);
	unitwrite (0x2a, 23,23);
	unitwrite (0x2c, 23,15);
	unitwrite (0x2e, 23,7);

	whisper ('1', "writing units right front plane"); 
	pca9548_set_page(brctx,0xee,7 );

	unitwrite (0x10, 24,31);
	unitwrite (0x12, 24,23);
	unitwrite (0x14, 24,15);
	unitwrite (0x16, 24,7);

	unitwrite (0x1a, 26,31);
	unitwrite (0x18, 26,23);
	unitwrite (0x1c, 26,15); // oreder is fracked here too
	unitwrite (0x1e, 26,7);

	unitwrite (0x20, 28,31);
	unitwrite (0x22, 28,23);
	unitwrite (0x24, 28,15);
	unitwrite (0x26, 28,7);

	unitwrite (0x28, 30,31);
	unitwrite (0x2a, 30,23);
	unitwrite (0x2c, 30,15);
	unitwrite (0x2e, 30,7);

	whisper ('1', "writing units right back plane"); 
	pca9548_set_page(brctx,0xee, 2);

	unitwrite (0x10, 25,31);
	unitwrite (0x12, 25,23);
	unitwrite (0x14, 25,15);
	unitwrite (0x16, 25,7);

	unitwrite (0x18, 27,31);
	unitwrite (0x1a, 27,23);
	unitwrite (0x1c, 27,15);
	unitwrite (0x1e, 27,7);

	unitwrite (0x20, 29,31);
	unitwrite (0x22, 29,23);
	unitwrite (0x24, 29,15);
	unitwrite (0x26, 29,7);

	unitwrite (0x28, 31,31);
	unitwrite (0x2a, 31,23);
	unitwrite (0x2c, 31,15);
	unitwrite (0x2e, 31,7);

}


//find all the chips on all the i2c pages
void busscanner ()
	{
	u8 * bus_census;
	whisper ( '4',"scanning all pages for i2c devices ");
	for ( u8 i2cpage=0;i2cpage<=7;i2cpage++) 
		{ wtp;
		whisper ( '4',"page:%i", i2cpage);
		pca9548_set_page(brctx, 0xee, i2cpage);
		bus_census = i2busscan (brctx); 
		whisper ('3', "i2c bus: \n%s", mprettyhex (bus_census, 128));
		wtm;
		}
	}
int main ( int argc, char** argv)
	{
	int ret = 0;
	int retc; 
	whisper_set ("123456sxfF");
	whisper_set ("123456789sxfF");
	retc =0; 
	errno=0; 
	brctx = bridge_initctx (getenv ("serialdev"), 250000); 
	bridge_try_chirp( brctx);	
	busscanner(); 
	/*
	for ( u32 j = 0; j <= 7 ; j++) {
		pca9548_set_page(0xee,j);
		if ( (j == 0) || (j == 4))
			{
			pca9635_init ( 0x10, kpca9635_driver_mode_totem_pole ); 
			pca9635_init ( 0x12, kpca9635_driver_mode_totem_pole ); 
			pca9635_init ( 0x14, kpca9635_driver_mode_totem_pole ); 
			pca9635_init ( 0x16, kpca9635_driver_mode_totem_pole ); 
			}
	}
	scan_and_poke (); 
	*/
	///usleep (1000000); 	
	whisper('1', "this is a bug in whisper!"); 
	return (ret);
	}

void initialize_array () 
{
for (u32 i=0 ; i < 9; i++){
	pca9548_set_page(brctx, 0xee,i%8);
	printf("\e[1;1H\e[2J");
	for ( u8 addr = 0x10; addr < 0x30; addr += 2)
		{
		pca9635_init (brctx, addr, kpca9635_driver_mode_totem_pole ); 
		}
	fflush(0); 
	}
}
void scan_and_poke ()
{
	initialize_array();
clearframebuffer(); 
load_framebuffer ();
whisper ('6',"backck to work"); 
//dumpframebuffer(); 
whisper ('6',"really back to work"); 
write_array(); 
whisper ('6',"ck to work"); 
fflush (0); 
}

