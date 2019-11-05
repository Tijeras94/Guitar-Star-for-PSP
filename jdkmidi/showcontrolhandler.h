
#ifndef _JDKMIDI_SHOWCONTROLHANDLER_H
#define _JDKMIDI_SHOWCONTROLHANDLER_H



#include "jdkmidi/showcontrol.h"


namespace jdkmidi
{
  
  class MIDISCHandle
    {
    public:
      MIDISCHandle();
      virtual ~MIDISCHandle();
      
      virtual bool Dispatch( const MIDIShowControlPacket &p );
      
      virtual bool Go();
      virtual bool Go( const MIDICue & q_number );
      virtual bool Go( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool Go( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool Stop();
      virtual bool Stop( const MIDICue & q_number );
      virtual bool Stop( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool Stop( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool Resume();
      virtual bool Resume( const MIDICue & q_number );
      virtual bool Resume( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool Resume( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool TimedGo( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff
        );
      virtual bool TimedGo( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff,
        const MIDICue & q_number
        );
      virtual bool TimedGo( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff,
        const MIDICue & q_number, const MIDICue & q_list
        );
      
      virtual bool TimedGo( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff,
        const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path 
        );
      
      virtual bool Load( const MIDICue & q_number );
      virtual bool Load( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool Load( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool Set( ulong ctrl_num, ulong ctrl_val );
      
      virtual bool Set( 
        ulong ctrl_num, 
        ulong ctrl_val,
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff 
        );
      
      virtual bool Fire( uchar macro_num );
      virtual bool AllOff();
      virtual bool Restore();
      virtual bool Reset();
      virtual bool GoOff();
      virtual bool GoOff( const MIDICue & q_number );
      virtual bool GoOff( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool GoOff( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool GoJam();
      virtual bool GoJam( const MIDICue & q_number );
      virtual bool GoJam( const MIDICue & q_number, const MIDICue & q_list );
      virtual bool GoJam( const MIDICue & q_number, const MIDICue & q_list, const MIDICue & q_path );
      virtual bool StandbyPlus();
      virtual bool StandbyPlus( const MIDICue & q_list );
      virtual bool StandbyMinus();
      virtual bool StandbyMinus( const MIDICue & q_list );
      virtual bool SequencePlus();
      virtual bool SequencePlus( const MIDICue & q_list );
      virtual bool SequenceMinus();
      virtual bool SequenceMinus( const MIDICue & q_list );
      virtual bool StartClock();
      virtual bool StartClock( const MIDICue & q_list );
      virtual bool StopClock();
      virtual bool StopClock( const MIDICue & q_list );
      virtual bool ZeroClock();
      virtual bool ZeroClock( const MIDICue & q_list );
      virtual bool SetClock( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff
        );
      virtual bool SetClock( 
        uchar hr, uchar mn, uchar sc, uchar fr, uchar ff,
        const MIDICue & q_list
        );
      virtual bool MTCChaseOn();
      virtual bool MTCChaseOn( const MIDICue & q_list );
      virtual bool MTCChaseOff();
      virtual bool MTCChaseOff( const MIDICue & q_list );
      virtual bool OpenQList( const MIDICue & q_list );
      virtual bool CloseQList( const MIDICue & q_list );
      virtual bool OpenQPath( const MIDICue & q_path );
      virtual bool CloseQPath( const MIDICue & q_path );
      
    };
  
}

#endif
