#ifndef _JDKMIDI_SONG_H
#define _JDKMIDI_SONG_H

#include "jdkmidi/multitrack.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/filereadmultitrack.h"

namespace jdkmidi
{  
  class MIDISong
    {
    public:
      MIDISong( int max_tracks );
      virtual ~MIDISong();
      
      bool Load( const char *fname );
      
//	bool Save( const char *fname );	
      
      MIDIMultiTrack *GetMultiTrack()
        {
          return multitrack;
        }
      
      const MIDIMultiTrack *GetMultiTrack() const
        {
          return multitrack;
        }
      
      MIDISequencer *GetSeq()
        {
          return sequencer;
        }
      
      const MIDISequencer *GetSeq() const
        {
          return sequencer;
        }
      
    protected:
      
      MIDIMultiTrack track;	
      MIDISequencer seq;
      
      char title;
    };
}

#endif
