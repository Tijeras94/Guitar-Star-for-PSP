/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_EDITTRACK_H
#define _JDKMIDI_EDITTRACK_H

#include "jdkmidi/track.h"
#include "jdkmidi/matrix.h"
#include "jdkmidi/process.h"

namespace jdkmidi
{
  
  class	MIDIEditTrackEventMatcher
    {
    public:
      MIDIEditTrackEventMatcher();
      virtual ~MIDIEditTrackEventMatcher();
      
      virtual bool Match( const MIDITimedBigMessage &ev ) =0;
    };
  
  
  class 	MIDIEditTrack 
    {
    public:
      MIDIEditTrack( MIDITrack *track_ );
      virtual	~MIDIEditTrack();
      
      //
      // Process applies a MIDI process to all events that are matched
      //
      
      void 	Process(
        MIDIClockTime start_time,
        MIDIClockTime end_time,
        MIDIProcessor *process,
        MIDIEditTrackEventMatcher *match
        );
      
      //
      // Truncate erases all events after a certain time. then
      // adds appropriate note off's
      //
      void    Truncate(MIDIClockTime start_time );
      
      
      //
      // this merge function merges two other tracks into this track.
      // this is the faster form of merge
      //
      void	Merge(
        MIDITrack *trk1, MIDITrack *trk2,
        MIDIEditTrackEventMatcher *match1,
        MIDIEditTrackEventMatcher *match2 );
      
      
      //
      // this erase function will erase all events from start to end time
      // and can be jagged or not.
      //
      void	Erase(
        MIDIClockTime start,
        MIDIClockTime end,
        bool jagged=true,
        MIDIEditTrackEventMatcher *match=0
        );
      
      
      
      //
      // this delete function will delete all events like erase and then
      // shift the events over
      //
      void    Delete(
        MIDIClockTime start,
        MIDIClockTime end,
        bool jagged=true,
        MIDIEditTrackEventMatcher *match=0
        );
      
      
      //
      // this insert function will insert 'length' clicks starting at
      // the events at start time.
      //
      void	Insert(
        MIDIClockTime start,
        MIDIClockTime length
        );
      
      
      //
      // this shift function will shift all event times by an offset.
      //
      void	Shift(
        signed long offset,
        MIDIEditTrackEventMatcher *match=0
        );
      
    protected:
      
      MIDIMatrix matrix;
      MIDITrack *track;
      
    private:
      
    }; 
  
  
}

#endif


