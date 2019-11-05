/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_FILEREADMULTITRACK_H
#define _JDKMIDI_FILEREADMULTITRACK_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/file.h"
#include "jdkmidi/fileread.h"
#include "jdkmidi/multitrack.h"

namespace jdkmidi
{
  class MIDIFileReadMultiTrack : public MIDIFileEvents 
    {
    public:
      MIDIFileReadMultiTrack( MIDIMultiTrack *mlttrk );
      
      virtual ~MIDIFileReadMultiTrack();
      
      
//
// The possible events in a MIDI Files
//
      
      virtual void    mf_sysex( MIDIClockTime time, const MIDISystemExclusive &ex );
      
      virtual void    mf_arbitrary( MIDIClockTime time, int len, unsigned char *data );
      virtual void    mf_metamisc( MIDIClockTime time, int, int, unsigned char * 	);
      virtual void    mf_seqnum( MIDIClockTime time, int );
      virtual void    mf_smpte( MIDIClockTime time, int, int, int, int, int );
      virtual void    mf_timesig( MIDIClockTime time, int, int, int, int );
      virtual void    mf_tempo( MIDIClockTime time, unsigned long tempo );
      virtual void    mf_keysig(MIDIClockTime time, int, int );
      virtual void    mf_sqspecific( MIDIClockTime time, int, unsigned char * );
      virtual void    mf_text( MIDIClockTime time, int, int, unsigned char * );
      virtual void    mf_eot( MIDIClockTime time );
      
//
// the following methods are to be overridden for your specific purpose
//
      
      virtual void    mf_error( char * );
      
      virtual void    mf_starttrack( int trk );
      virtual void    mf_endtrack( int trk );
      virtual void    mf_header( int, int, int );
      
//
// Higher level dispatch functions
//
      
      virtual	void    ChanMessage( const MIDITimedMessage &msg );
      
    protected:
      
      void AddEventToMultiTrack( 
        const MIDITimedMessage &msg, 
        MIDISystemExclusive *sysex, 
        int dest_track 
        );
      
      MIDIMultiTrack *multitrack;
      int cur_track;
      
      int the_format;
      int num_tracks;
      int division;
      
    }; 
  
}


#endif
