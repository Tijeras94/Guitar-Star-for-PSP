/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_FILESHOW_H
#define _JDKMIDI_FILESHOW_H

#include "jdkmidi/fileread.h"

namespace jdkmidi
{
  
  class  MIDIFileShow : public MIDIFileEvents	
    {
    public:
      MIDIFileShow( FILE *out_ );
      virtual	~MIDIFileShow();
      
    protected:
      
      virtual	void	show_time( MIDIClockTime time );
      
      virtual void    mf_error( char * );
      
      virtual void    mf_starttrack( int trk );
      virtual void    mf_endtrack( int trk );
      virtual void    mf_header( int, int, int );
      
      
//
// The possible events in a MIDI Files
//
      
      virtual void    mf_system_mode(const MIDITimedMessage &msg 	);
      virtual void    mf_note_on(const MIDITimedMessage &msg 	);
      virtual void    mf_note_off(const MIDITimedMessage &msg 	);
      virtual void    mf_poly_after(const MIDITimedMessage &msg 	);
      virtual void    mf_bender(const MIDITimedMessage &msg 	);
      virtual void    mf_program(const MIDITimedMessage &msg 	);
      virtual void    mf_chan_after(const MIDITimedMessage &msg 	);
      virtual void    mf_control(const MIDITimedMessage &msg 	);
      virtual void    mf_sysex(MIDIClockTime time, const MIDISystemExclusive &ex	);
      
      virtual void    mf_arbitrary(MIDIClockTime time, int len, unsigned char *data 	);
      virtual void    mf_metamisc(MIDIClockTime time, int, int, unsigned char * 	);
      virtual void    mf_seqnum(MIDIClockTime time, int 			);
      virtual void    mf_smpte(MIDIClockTime time, int, int, int, int, int );
      virtual void    mf_timesig(MIDIClockTime time, int, int, int, int );
      virtual void    mf_tempo(MIDIClockTime time, unsigned long tempo			);
      virtual void    mf_keysig(MIDIClockTime time, int, int 			);
      virtual void    mf_sqspecific(MIDIClockTime time, int, unsigned char * 		);
      virtual void    mf_text(MIDIClockTime time, int, int, unsigned char *	);
      virtual void    mf_eot( MIDIClockTime time );
      
      FILE *out;	
      int division;
      
    private:
      
    }; 
  
}
#endif

