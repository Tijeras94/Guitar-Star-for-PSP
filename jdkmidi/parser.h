/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_PARSER_H
#define _JDKMIDI_PARSER_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  
  class  MIDIParser
    {
    public:
      MIDIParser( ushort max_sysex_size=384 );
      virtual		~MIDIParser();
      
      void		Clear()
        {
          state=FIND_STATUS;
        }
      
      virtual	bool	Parse( uchar b, MIDIMessage *msg );
      
      MIDISystemExclusive *GetSystemExclusive() const 	{ return sysex;	}
      
    protected:
      
      //
      // The states used for parsing messages.
      //
      
      enum State
        {
          FIND_STATUS,		// ignore data bytes
          FIRST_OF_ONE,		// read first data byte of a one data byte msg
          FIRST_OF_TWO,		// read first data byte of two data byte msg
          SECOND_OF_TWO,		// read second data byte of two data byte msg
          FIRST_OF_ONE_NORUN,	// read one byte message, do not allow 
          // running status (for MTC)
          SYSEX_DATA		// read sysex data byte
        };
      
      MIDIMessage tmp_msg;
      MIDISystemExclusive *sysex;	
      State 	state;
      
      bool	ParseSystemByte( uchar b, MIDIMessage *msg );
      bool	ParseDataByte( uchar b, MIDIMessage *msg );
      void	ParseStatusByte( uchar b );
    };
  
  
}

#endif


