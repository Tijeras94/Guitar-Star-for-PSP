/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#include "jdkmidi/world.h"

#include "jdkmidi/file.h"

#if DEBUG_MDFILE
# undef DBG
# define DBG(a) a
#endif

namespace jdkmidi
{
  
  
  MIDIFile::MIDIFile()	
  {
  } 
  
  MIDIFile::~MIDIFile()	
  {
  } 
  
  unsigned long MIDIFile::ConvertTempoToFreq(
    short division,
    MIDITempo &tempo
    )	
  {
    if( division>0 )
    {
      long clocks_per_beat = (long)division*1000;
      long micro_sec_per_beat = tempo.GetMIDIFileTempo()/1000;
      
      return (unsigned long) clocks_per_beat/micro_sec_per_beat;
    }
    else
    {
      // TO DO: handle smpte frame rate references
      return 120;
    }
  } 
  
  
  unsigned long	MIDIFile::ReadVariableLengthNumber( unsigned char **in )	
  {
    unsigned long num=0;
    unsigned char *t=*in;
    
    do
    {
      num <<= 7;
      num |= (*t);
    } while( (*t++)&0x80 );
    
    *in=t;
    return num;
  } 
  
  unsigned char *	MIDIFile::WriteVariableLengthNumber( unsigned long num, unsigned char *out )	
  {
    register unsigned long buffer;
    
    buffer = num & 0x7f;
    while( (num >>=7 )>0 )
    {
      buffer <<= 8;
      buffer |=0x80;
      buffer += (num & 0x7f);
    }
    
    do
    {
      *out++ = (unsigned char)buffer;
      if( buffer & 0x80 )
        buffer >>=8;
      else
        break;          	
    } while( true );
    
    return out;
  } 
  
}
