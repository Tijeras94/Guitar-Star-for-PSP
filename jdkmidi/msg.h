/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_MSG_H
#define _JDKMIDI_MSG_H

#include "jdkmidi/midi.h"
#include "jdkmidi/tempo.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  
  class MIDIMessage;
  class MIDIBigMessage;
  
  class MIDITimedMessage;
  class MIDIDeltaTimedMessage;
  
  class MIDITimedBigMessage;
  class MIDIDeltaTimedBigMessage;
  
  class 	MIDIMessage
    {
    public:
      
      //
      // The Constructors
      //
      
      MIDIMessage();
      
      MIDIMessage( const MIDIMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDIMessage & m );
      
      char *	MsgToText( char *txt ) const; 
      
      //
      // The equal operator
      //
      
      const MIDIMessage	& operator = ( const MIDIMessage &m );
      
      //
      // The Query methods.
      //
      
      char	GetLength() const;
      
      unsigned char	GetStatus() const	{ return (unsigned char)status;	}
      unsigned char	GetChannel() const	{ return (unsigned char)(status&0x0f);	}
      unsigned char	GetType() const		{ return (unsigned char)(status&0xf0);	}
      
      unsigned char	GetMetaType() const	{ return byte1;		}
      
      unsigned char	GetByte1() const	{ return byte1;		}
      unsigned char	GetByte2() const	{ return byte2;		}
      unsigned char	GetByte3() const	{ return byte3;		}
      
      unsigned char	GetNote() const		{ return byte1;		}
      unsigned char	GetVelocity() const	{ return byte2;		}
      unsigned char	GetPGValue() const	{ return byte1;		}
      unsigned char	GetController() const	{ return byte1;		}
      unsigned char	GetControllerValue() const { return byte2;	}
      
      short	GetBenderValue() const;
      
      unsigned short	GetMetaValue()	const;
      unsigned char GetTimeSigNumerator() const;
      unsigned char GetTimeSigDenominator() const;
      
      bool	IsChannelMsg() const;
      
      bool	IsNoteOn() const;
      
      
      bool	IsNoteOff() const;
      
      bool	IsPolyPressure() const;
      
      bool	IsControlChange() const;
      
      bool	IsProgramChange() const;
      
      bool	IsChannelPressure() const;
      
      bool	IsPitchBend() const;
      
      bool	IsSystemMessage() const;
      
      bool	IsSysEx() const;
      
      short	GetSysExNum() const;
      
      bool	IsMTC() const;
      
      bool	IsSongPosition() const;
      
      bool	IsSongSelect() const;
      
      bool 	IsTuneRequest() const;
      
      bool	IsMetaEvent() const;
      
      bool 	IsTextEvent() const;
      
      bool	IsAllNotesOff() const;
      
      
      bool	IsNoOp() const;
      
      
      bool	IsTempo() const;
      
      
      bool	IsDataEnd() const;
      
      
      bool 	IsTimeSig() const;
      
      bool 	IsBeatMarker() const;
      
      //
      // GetTempo() returns the tempo value in 1/32 bpm
      //
      
      unsigned short	GetTempo32() const;
      
      unsigned short	GetLoopNumber() const;
      
      //
      // The 'Set' methods
      //
      
      void	SetStatus( unsigned char s )	{ status=s;		}
      void	SetChannel( unsigned char s )	{status=(unsigned char)((status&0xf0)|s);}
      void	SetType( unsigned char s )		{status=(unsigned char)((status&0x0f)|s);}
      
      void	SetByte1( unsigned char b )		{ byte1=b;		}
      void	SetByte2( unsigned char b )		{ byte2=b;		}
      void	SetByte3( unsigned char b )		{ byte3=b;		}
      
      void	SetNote( unsigned char n ) 		{ byte1=n;		}
      void	SetVelocity(unsigned char v) 	{ byte2=v;		}
      void	SetPGValue(unsigned char v) 	{ byte1=v;		}
      void	SetController(unsigned char c) 	{ byte1=c;		}
      void	SetControllerValue(unsigned char v ) { byte2=v;		}
      
      void	SetBenderValue( short v);
      
      void	SetMetaType( unsigned char t ) ;
      
      void	SetMetaValue( unsigned short v );
      
      void	SetNoteOn( unsigned char chan, unsigned char note, unsigned char vel );
      
      void	SetNoteOff( unsigned char chan, unsigned char note, unsigned char vel );
      
      void	SetPolyPressure( unsigned char chan, unsigned char note, unsigned char pres );
      
      void	SetControlChange( unsigned char chan, unsigned char ctrl, unsigned char val );
      
      void	SetProgramChange( unsigned char chan, unsigned char val );
      
      void	SetChannelPressure( unsigned char chan, unsigned char val );
      
      void	SetPitchBend( unsigned char chan, short val );
      
      void	SetPitchBend( unsigned char chan, unsigned char low, unsigned char high );
      
      void	SetSysEx();
      
      void	SetMTC( unsigned char field, unsigned char v );
      
      void	SetSongPosition( short pos );
      
      void	SetSongSelect(unsigned char sng);
      
      void	SetTuneRequest();
      
      void	SetMetaEvent( unsigned char type, unsigned char v1, unsigned char v2 );
      
      void	SetMetaEvent( unsigned char type, unsigned short v );
      
      void	SetAllNotesOff( unsigned char chan, unsigned char type=C_ALL_NOTES_OFF );
      
      void	SetLocal( unsigned char chan, unsigned char v );
      
      void	SetNoOp();
      
      void	SetTempo32( unsigned short tempo_times_32 );
      
      void	SetText( unsigned short text_num, unsigned char type=META_GENERIC_TEXT );
      
      void	SetDataEnd();
      
      void	SetTimeSig( unsigned char numerator, unsigned char denominator );
      
      void	SetBeatMarker();
      
    protected:
      
      static	char * 	chan_msg_name[16];
      static	char * 	sys_msg_name[16];
      
      unsigned char	status;
      unsigned char	byte1;
      unsigned char	byte2;
      unsigned char	byte3;		// byte 3 is only used for meta-events and to
      // round out the structure size to 32 bits
      
    }; 
  
///////////////////////////////////////////////////////////////////////////
  
  class MIDIBigMessage : public MIDIMessage 
    {
    public:
      //
      // Constructors
      //
      
      MIDIBigMessage();
      
      MIDIBigMessage( const MIDIBigMessage &m );
      
      MIDIBigMessage( const MIDIMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDIBigMessage &m );
      
      void	Copy( const MIDIMessage &m );
      
      //
      // destructors
      //
      
      ~MIDIBigMessage();
      
      //
      // operator =
      //
      
      const MIDIBigMessage &operator = ( const MIDIBigMessage &m );
      
      const MIDIBigMessage &operator = ( const MIDIMessage &m );
      
      //
      // 'Get' methods
      //
      
      MIDISystemExclusive *GetSysEx();
      
      const MIDISystemExclusive *GetSysEx() const;
      
      //
      // 'Set' methods
      //
      
      void CopySysEx( const MIDISystemExclusive *e );
      
      void ClearSysEx();
      
      
      MIDISystemExclusive *sysex;	
    }; 
  
///////////////////////////////////////////////////////////////////////////
  
  class 	MIDITimedMessage : public MIDIMessage 
    {
    public:
      
      //
      // Constructors
      //
      
      MIDITimedMessage();
      
      MIDITimedMessage( const MIDITimedMessage &m );
      
      MIDITimedMessage( const MIDIMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDITimedMessage &m );
      
      //
      // operator =
      //
      
      const MIDITimedMessage &operator = ( const MIDITimedMessage & m );
      
      const MIDITimedMessage &operator = ( const MIDIMessage & m );
      
      //
      // 'Get' methods
      //
      
      MIDIClockTime	GetTime() const;
      
      //
      // 'Set' methods
      //
      
      void	SetTime( MIDIClockTime t );
      
      
      //
      // Compare method for sorting. Not just comparing time.
      //
      
      static int 	CompareEvents(
        const MIDITimedMessage &a,
        const MIDITimedMessage &b
        );
      
    protected:
      
      
      MIDIClockTime time;
    }; 
  
///////////////////////////////////////////////////////////////////////////
  
  class 	MIDIDeltaTimedMessage : public MIDIMessage 
    {
    public:
      
      //
      // Constructors
      //
      
      MIDIDeltaTimedMessage();
      
      MIDIDeltaTimedMessage( const MIDIDeltaTimedMessage &m );
      
      MIDIDeltaTimedMessage( const MIDIMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDIDeltaTimedMessage &m );
      
      //
      // operator =
      //
      
      const MIDIDeltaTimedMessage &operator = ( const MIDIDeltaTimedMessage &m );
      
      const MIDIDeltaTimedMessage &operator = ( const MIDIMessage &m );
      
      //
      // 'Get' methods
      //
      
      MIDIClockTime	GetDeltaTime() const;
      
      //
      // 'Set' methods
      //
      
      void	SetDeltaTime( MIDIClockTime t );
      
      
    protected:
      MIDIClockTime	dtime;
    }; 
  
///////////////////////////////////////////////////////////////////////////
  
  class 	MIDITimedBigMessage : public MIDIBigMessage 
    {
    public:
      
      //
      // Constructors
      //
      
      MIDITimedBigMessage();
      
      MIDITimedBigMessage( const MIDITimedBigMessage &m );
      
      MIDITimedBigMessage( const MIDIBigMessage &m );
      
      MIDITimedBigMessage( const MIDITimedMessage &m );
      
      MIDITimedBigMessage( const MIDIMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDITimedBigMessage &m );
      
      void	Copy( const MIDITimedMessage &m );
      
      //
      // operator =
      //
      
      const MIDITimedBigMessage &operator = ( const MIDITimedBigMessage & m );
      
      const MIDITimedBigMessage &operator = ( const MIDITimedMessage & m );
      
      const MIDITimedBigMessage &operator = ( const MIDIMessage & m );
      
      //
      // 'Get' methods
      //
      
      MIDIClockTime	GetTime() const;
      
      //
      // 'Set' methods
      //
      
      void	SetTime( MIDIClockTime t );
      
      //
      // Compare method, for sorting. Not just comparing time.
      //
      
      static int 	CompareEvents(
        const MIDITimedBigMessage &a,
        const MIDITimedBigMessage &b
        );
      
    protected:   	
      MIDIClockTime	time;
    }; 
  
///////////////////////////////////////////////////////////////////////////
  
  class 	MIDIDeltaTimedBigMessage : public MIDIBigMessage 
    {
    public:
      //
      // Constructors
      //
      
      MIDIDeltaTimedBigMessage();
      
      MIDIDeltaTimedBigMessage( const MIDIDeltaTimedBigMessage &m );
      
      MIDIDeltaTimedBigMessage( const MIDIBigMessage &m );
      
      MIDIDeltaTimedBigMessage( const MIDIMessage &m );
      
      MIDIDeltaTimedBigMessage( const MIDIDeltaTimedMessage &m );
      
      void	Clear();
      
      void	Copy( const MIDIDeltaTimedBigMessage &m );
      
      void	Copy( const MIDIDeltaTimedMessage &m );
      
      //
      // operator =
      //
      
      const MIDIDeltaTimedBigMessage &operator = ( const MIDIDeltaTimedBigMessage &m );
      
      const MIDIDeltaTimedBigMessage &operator = ( const MIDIDeltaTimedMessage &m );
      
      const MIDIDeltaTimedBigMessage &operator = ( const MIDIMessage &m );
      
      //
      // 'Get' methods
      //
      
      MIDIClockTime	GetDeltaTime() const;
      
      //
      // 'Set' methods
      //
      
      void	SetDeltaTime( MIDIClockTime t );
      
      
    protected:
      MIDIClockTime	dtime;
    }; 
  
}


#endif

