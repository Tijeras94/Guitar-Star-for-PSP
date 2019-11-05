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

#include "jdkmidi/edittrack.h"

#ifndef DEBUG_MDETRACK
# define DEBUG_MDETRACK	0
#endif

#if DEBUG_MDETRACK
# undef DBG
# define DBG(a)	a
#endif

namespace jdkmidi
{
  
  
  MIDIEditTrackEventMatcher::MIDIEditTrackEventMatcher()
  {
  }
  
  MIDIEditTrackEventMatcher::~MIDIEditTrackEventMatcher()
  {
  }
  
  
  MIDIEditTrack::MIDIEditTrack( MIDITrack *track_ )
    :
    track( track_ )
  {
    ENTER( "MIDIEditTrack::MIDIEditTrack()" );
    
  }
  
  MIDIEditTrack::~MIDIEditTrack()
  {
    ENTER( "MIDIEditTrack::~MIDIEditTrack()" );
  }
  
  
  void 	MIDIEditTrack::Process(
    MIDIClockTime start_time,
    MIDIClockTime end_time,
    MIDIProcessor *process,
    MIDIEditTrackEventMatcher *match
    )
  {
    // TODO: Process
  }
  
  
  
//
// Truncate erases all events after a certain time. then
// adds appropriate note off's
//
  void	MIDIEditTrack::Truncate( MIDIClockTime start_time )
  {
    ENTER( "TO DO: MIDIEditTrack::Truncate()" );
    
    // TO DO:
  }
  
  
  
  
  
//
// this merge function merges two other tracks into this track.
// this is the faster form of merge
//
  void	MIDIEditTrack::Merge(
    MIDITrack *trk1,
    MIDITrack *trk2,
    MIDIEditTrackEventMatcher *match1,
    MIDIEditTrackEventMatcher *match2
    )
  {
    ENTER( "TO DO: MIDIEditTrack::Merge()" );
    
    // TO DO:
  }
  
  
  
  
  
  
//
// this erase function will erase all events from start to end time
// and can be jagged or not.
//
  void	MIDIEditTrack::Erase(
    MIDIClockTime start,
    MIDIClockTime end,
    bool jagged,
    MIDIEditTrackEventMatcher *match
    )
  {
    ENTER( "TO DO: MIDIEditTrack::Erase()" );
    
    // TO DO:
  }
  
  
  
  
  
//
// this delete function will delete all events like erase and then
// shift the events over
//
  void	MIDIEditTrack::Delete(
    MIDIClockTime start,
    MIDIClockTime end,
    bool jagged,
    MIDIEditTrackEventMatcher *match
    )
  {
    ENTER( "TO DO: MIDIEditTrack::Delete()" );
    
    // TO DO:
  }
  
  
  
  
//
// this insert function will insert 'length' clicks starting at
// the events at start time.
//
  void	MIDIEditTrack::Insert(
    MIDIClockTime start,
    MIDIClockTime length
    )
  {
    ENTER( "TO DO: MIDIEditTrack::Insert()" );
    
    // TO DO:
  }
  
  
  
  
//
// this shift function will shift all event times by an offset.
//
  void	MIDIEditTrack::Shift(
    signed long offset,
    MIDIEditTrackEventMatcher *match
    )
  {
    ENTER( "TO DO: MIDIEditTrack::Shift()" );
    
    // TO DO:
  }
  
  
  
}
