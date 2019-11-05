/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_MULTITRACK_H
#define _JDKMIDI_MULTITRACK_H

#include "jdkmidi/track.h"

namespace jdkmidi
{
  
  class MIDIMultiTrack;
  class MIDIMultiTrackIteratorState;
  class MIDIMultiTrackIterator;
  
  class MIDIMultiTrack 	
    {
    public:
      
      MIDIMultiTrack( int max_num_tracks_=64, bool deletable_=true );
      virtual	~MIDIMultiTrack();
      
      void SetTrack( int trk, MIDITrack *t );
      MIDITrack *GetTrack( int trk );
      const MIDITrack *GetTrack( int trk ) const;
      int	GetNumTracks()								{ return num_tracks;	}
      
      
      void Clear();
      
      int GetClksPerBeat() 	
        {
          return clks_per_beat;
        }
      
      void SetClksPerBeat( int c )
        {
          clks_per_beat = c;		
        }
      
    protected:
      
      MIDITrack **tracks;		
      const int num_tracks;
      bool	deletable;
      
      int 	clks_per_beat;	
      
    private:
      
    }; 
  
  class MIDIMultiTrackIteratorState  
    {
    public:	
      
      MIDIMultiTrackIteratorState( int num_tracks_=64);
      MIDIMultiTrackIteratorState( const MIDIMultiTrackIteratorState &m );	
      virtual ~MIDIMultiTrackIteratorState();
      
      const MIDIMultiTrackIteratorState & operator = ( const MIDIMultiTrackIteratorState &m );	
      
      int	GetNumTracks()								{ return num_tracks;		}
      int	GetCurEventTrack()							{ return cur_event_track;	}
      MIDIClockTime GetCurrentTime()					{ return cur_time;			}
      
      void Reset();
      int FindTrackOfFirstEvent();
      
      MIDIClockTime cur_time;	
      int cur_event_track;
      int num_tracks;
      int *next_event_number;	
      MIDIClockTime *next_event_time;
    }; 
  
  class MIDIMultiTrackIterator  
    {
    public:	
      
      MIDIMultiTrackIterator( MIDIMultiTrack *mlt );
      virtual ~MIDIMultiTrackIterator();
      
      
      void GoToTime( MIDIClockTime time );		
      
      bool GetCurEventTime( MIDIClockTime *t);
      bool GetCurEvent( int *track, MIDITimedBigMessage **msg );
      bool GoToNextEvent();
      
      bool GoToNextEventOnTrack( int track );
      
      const MIDIMultiTrackIteratorState &GetState() const 
        {
          return state;
        } 
      
      MIDIMultiTrackIteratorState &GetState() 
        {
          return state;
        } 
      
      void SetState( const MIDIMultiTrackIteratorState &s ) 
        {
          state = s;
        } 
      
      MIDIMultiTrack * GetMultiTrack()  				{ return multitrack;	}
      const MIDIMultiTrack * GetMultiTrack() const 	{ return multitrack; 	}	
      
    protected:
      
      MIDIMultiTrack *multitrack;	
      MIDIMultiTrackIteratorState state;
    }; 
  
}

#endif


