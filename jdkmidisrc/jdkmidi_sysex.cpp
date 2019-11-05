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

#include "jdkmidi/sysex.h"


#ifndef DEBUG_MDSYSEX
# define DEBUG_MDSYSEX	0
#endif

#if DEBUG_MDSYSEX
# undef DBG
# define DBG(a)	a
#endif

namespace jdkmidi
{
  
  
  MIDISystemExclusive::MIDISystemExclusive( int size_ )
  {
    ENTER( "MIDISystemExclusive::MIDISystemExclusive" );
    
    buf=new uchar[size_];
    
    if( buf )
      max_len=size_;
    else
      max_len=0;
    
    cur_len=0;
    chk_sum=0;
    deletable=true;
  }
  
  MIDISystemExclusive::MIDISystemExclusive( const MIDISystemExclusive &e )	
  {
    buf = new unsigned char [e.max_len];
    max_len = e.max_len;
    cur_len = e.cur_len;
    chk_sum = e.chk_sum;
    deletable = true;
    
    for( int i=0; i<cur_len; ++i )
    {
      buf[i] = e.buf[i];
    }
  } 
  
  MIDISystemExclusive::~MIDISystemExclusive()
  {
    ENTER( "MIDISystemExclusive::~MIDISystemExclusive" );
    
    if( deletable )
      delete [] buf;
  }
  
  
}
