#include "mutil.h"
#include "math.h"
#include "colortx.h"

// rescale a u8 to a 0-1 float
float u8tof( u8 in)  {
	float outv = 0;
	outv =   (in * ( 1.0 / 255.0 ));
	assert ( outv <= 1.0 );
	assert ( outv >= 0.0 );
	return ( outv); 
}

u8 ftou8 ( float in ) 
	{
	u8 outval = 0; 
	assert ( in >=0.0 );
	assert ( in <= 1.0 ); 
	outval  = ( u8 )  (in * 255.0);  
	return (outval); 
	}
float redofsample ( u32 incolor ) {
	u8 outv = 0; 
	outv = ( incolor >> 16)  & 0xff ;
	return ( u8tof (outv)); 
}
float greenofsample ( u32 incolor ) {
	u8 outv = 0; 
	outv = ( incolor >> 8)  & 0xff ;
	return ( u8tof (outv)); 
}
float blueofsample ( u32 incolor ) {
	u8 outv = 0; 
	outv = ( incolor )  & 0xff ;
	return u8tof (outv); 
}
float gain ( float  inval ) {
	return ( 0.6 * inval );
}

float f_white ( u32 incolor) {
	float rs,bs,gs, green_emphasis,Y,   output; 
	green_emphasis = 1; 	
	
	rs = redofsample( incolor ); 
	gs = greenofsample( incolor); 
	bs = blueofsample( incolor); 
	
	//output  =  powf ( ( rs *  ( green_emphasis * gs ) * bs ),  ( 1 / (green_emphasis + 2 ) )) ;
	//output = gain ( output ); 
	
	//Y' =  0.299 \times R + 0.587 \times G + 0.114 \times B
	Y = (( 0.299 ) * rs) +  ( (0.587) * gs ) + (( 0.114) * bs )  ;
	output = powf (Y , 6);
	output = gain (output) ;
	return ( output ) ;
}
u8  x_white ( u32 incolor ) {
	return (  (u8) ftou8 ( f_white ( incolor ) ) ); 
}

float colorgain ( float inval ) {
	return ( 0.9 * inval); 
}

u8 x_red ( u32 incolor) {
	float output; 
	output  = redofsample  ( incolor ); 		 
	output = powf ( output, 67); 
	output  = colorgain ( output ); 
	output  = 0.6 * ( output );  // hack for hi eff red devicess
	return ((u8) ftou8 ( output )) ;
	//return (0); 
}

u8 x_green ( u32 incolor) {
	float output; 
	output  = greenofsample  ( incolor ) ;		 
	output = powf ( output, 5); 
	output  = colorgain (  output ); 
	return ((u8) ftou8 ( output )) ;
}

float f_blue ( u32 incolor) {
	float output; 
	output  = blueofsample  ( incolor ); 		 
	output = powf ( output, 4); 
	output  = colorgain (  output ); 
	return ( output ) ;
}
u8  x_blue ( u32 incolor ) {
	return ( ftou8 ( f_blue ( incolor ) ) ); 
}

u8 x_yellow( u32 incolor) {
	u8 out= 0; 
	float white_m_blue;
	white_m_blue = f_white (incolor) -  f_blue(incolor);
	// provide a clipping fn
	if (white_m_blue < 0 ) 
		{ white_m_blue = 0; }
	if ( white_m_blue > 1 ) 
		{white_m_blue = 1; }
	
	out = ftou8 ( white_m_blue); 
	return (out); 
}

