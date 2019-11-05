/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_MATRIX_H
#define _JDKMIDI_MATRIX_H


#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"

namespace jdkmidi
{
  
  class 	MIDIMatrix
    {
    public:
      MIDIMatrix();
      virtual	~MIDIMatrix();
      
      virtual	bool	Process( const MIDIMessage &m );
      
      virtual	void	Clear();
      
      int GetTotalCount() const
        { 
          return total_count; 					
        }
      int	GetChannelCount( int channel ) const
        { 
          return channel_count[channel]; 		
        }
      
      int	GetNoteCount( int channel, int note ) const
        { 
          return note_on_count[channel][note];	
        }
      
      bool GetHoldPedal( int channel ) const
        { 
          return hold_pedal[channel]; 			
        }
      
      
    protected:
      virtual	void	DecNoteCount( const MIDIMessage &m, int channel, int note );
      virtual	void	IncNoteCount( const MIDIMessage &m, int channel, int note );
      virtual	void	ClearChannel( int channel );
      virtual	void	OtherMessage( const MIDIMessage &m );
      
      void	SetNoteCount(unsigned char chan,unsigned char note,unsigned char val)
        { 
          note_on_count[chan][note]=val; 	
        }
      void	SetChannelCount( unsigned char chan, int val )
        { 
          channel_count[chan]=val;		
        }
      
    private:
      unsigned char	note_on_count[16][128];
      int	channel_count[16];
      bool hold_pedal[16];
      int total_count;
    };
  
}


#endif


