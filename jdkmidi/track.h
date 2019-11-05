/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_TRACK_H
#define _JDKMIDI_TRACK_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  
  const int MIDITrackChunkSize=512;
  
  class  MIDITrackChunk 	
    {
    public:
      const MIDITimedBigMessage * GetEventAddress( int event_num ) const;
      MIDITimedBigMessage * GetEventAddress( int event_num );
      
    protected:
      
    private:
      MIDITimedBigMessage buf[MIDITrackChunkSize];
    }; 
  
  const int MIDIChunksPerTrack=512;
  
  class  MIDITrack 	
    {
    public:
      MIDITrack( int size=0 );
      
      MIDITrack( const MIDITrack &t );
      
      ~MIDITrack();
      
      void	Clear();
      void	Shrink();
      
      void	ClearAndMerge( const MIDITrack *src1, const MIDITrack *src2 );
      
//	bool	Insert( int start_event, int num_events );
//    bool 	Delete( int start_event, int num_events);
//    void 	Sort();
      
      bool	Expand( int increase_amount=(MIDITrackChunkSize) );
      
      MIDITimedBigMessage * GetEventAddress( int event_num );
      
      const MIDITimedBigMessage * GetEventAddress( int event_num ) const;
      
      const 	MIDITimedBigMessage *GetEvent( int event_num ) const;
      MIDITimedBigMessage *GetEvent( int event_num );
      bool	GetEvent( int event_num, MIDITimedBigMessage *msg ) const;
      
      bool	PutEvent( const MIDITimedBigMessage &msg );
      bool	PutEvent( const MIDITimedMessage &msg, MIDISystemExclusive *sysex );
      bool	SetEvent( int event_num, const MIDITimedBigMessage &msg );
      
      bool	MakeEventNoOp( int event_num );
      
      bool	FindEventNumber( MIDIClockTime time, int *event_num ) const;
      
      int	GetBufferSize()	const;
      int	GetNumEvents() const;
      
    private:
      
//	void 	QSort( int left, int right );
      
      MIDITrackChunk * chunk[MIDIChunksPerTrack];
      
      int buf_size;
      int num_events;
    }; 
  
}

#endif

