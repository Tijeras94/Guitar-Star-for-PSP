/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_FILEWRITE_H
#define _JDKMIDI_FILEWRITE_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/file.h"

namespace jdkmidi
{
  
  class MIDIFileWriteStream;
  class MIDIFileWriteStreamFile;
  class MIDIFileWrite;
  
  class MIDIFileWriteStream	
    { 
    public:
      MIDIFileWriteStream();
      virtual ~MIDIFileWriteStream();
      
      virtual long Seek( long pos, int whence=SEEK_SET ) = 0;
      virtual int WriteChar( int c ) = 0;	
    }; 
  
  class MIDIFileWriteStreamFile : public MIDIFileWriteStream 
    {
    public:
      MIDIFileWriteStreamFile( FILE *f_ );
      virtual ~MIDIFileWriteStreamFile();
      
      long Seek( long pos, int whence=SEEK_SET );
      int WriteChar( int c );
    private:
      FILE *f;
    }; 
  
  
  
  class MIDIFileWrite : protected MIDIFile	
    {
    public:
      MIDIFileWrite( MIDIFileWriteStream *out_stream_ );
      virtual   	~MIDIFileWrite();
      
      
      bool ErrorOccurred()          	{ return error;         }
      unsigned long   GetFileLength()         { return file_length;   }
      unsigned long   GetTrackLength()        { return track_length;  }
      void    ResetTrackLength()      { track_length=0;       }
      void    ResetTrackTime()        { track_time=0;         }
      
      void    WriteFileHeader(
        int format,
        int ntrks,
        int division
        );
      
      void    WriteTrackHeader( unsigned long length );
      
      void    WriteEvent( const MIDITimedMessage &m );
      void    WriteEvent( unsigned long time, const MIDISystemExclusive *e );
      void    WriteEvent( unsigned long time, unsigned short text_type, const char *text );
      void	WriteEvent( const MIDITimedBigMessage &m );
      
      void    WriteMetaEvent( unsigned long time, unsigned char type, const unsigned char *data, long length );
      void    WriteTempo( unsigned long time, long tempo );
      void    WriteKeySignature( unsigned long time, char sharp_flat, char minor );
      void    WriteTimeSignature(
        unsigned long time,
        char numerator=4,
        char denominator_power=2,
        char midi_clocks_per_metronome=24,
        char num_32nd_per_midi_quarter_note=8
        );
      
      void    WriteEndOfTrack(unsigned long time );
      
      virtual void    RewriteTrackLength();
      
    protected:
      virtual	void    Error(char *s);
      
      void    WriteCharacter( uchar c )
        {
          if( out_stream->WriteChar( c )<0 )
            error=true;
        }
      
      void    Seek( long pos )
        {
          if( out_stream->Seek( pos )<0 )
            error=true;
        }
      
      void    IncrementCounters( int c )   { track_length+=c; file_length+=c; }
      
      void    WriteShort( unsigned short c );
      void    Write3Char( long c );
      void    WriteLong( unsigned long c );
      
      int   	WriteVariableNum( unsigned long n );
      
      void    WriteDeltaTime( unsigned long time );
      
    private:
      bool error;
      bool within_track;
      unsigned long   file_length;
      unsigned long   track_length;
      unsigned long   track_time;
      unsigned long   track_position;
      uchar   running_status;
      
      MIDIFileWriteStream *out_stream;
      
    }; 
}

#endif


