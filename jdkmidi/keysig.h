/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_KEYSIG_H
#define _JDKMIDI_KEYSIG_H

#include "jdkmidi/midi.h"

namespace jdkmidi
{
  enum MIDIAccidentalType
    {
      ACCFlat=0,
      ACCNatural,
      ACCSharp	
    };
  
  class	MIDIKeySignature
    {
    public:
      
      MIDIKeySignature();
      MIDIKeySignature( const MIDIKeySignature &k );
      
      void	Reset();
      
      
      bool	IsMajor()			{ return major;		}
      
      void	SetSharpFlats( int sf, bool maj=true )
        { sharp_flat=sf; major=maj; Reset();	}
      
      int	GetSharpFlats()			{ return sharp_flat;	}
      
      MIDIAccidentalType	GetNoteStatus( int white_note )
        { return state[white_note%7];		}
      
      
      bool	ConvertMIDINote( int in_note, int *out_note );
      
    protected:
      
      bool	ProcessWhiteNote( int in_note, int *out_note );
      bool	ProcessBlackNote( int in_note, int *out_note );
      
      MIDIAccidentalType	state[7];
      bool		use_sharps;
      int		sharp_flat;
      bool		major;
      
      static	int	sharp_list[7];
      static	int	flat_list[7];
      
      
    };
}

#endif


