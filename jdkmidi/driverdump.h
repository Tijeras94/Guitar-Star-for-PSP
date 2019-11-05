#ifndef _JDKMIDI_DRIVERDUMP_H
#define _JDKMIDI_DRIVERDUMP_H

#include "jdkmidi/driver.h"

namespace jdkmidi
{
  
  class MIDIDriverDump : public MIDIDriver 
    {
    public:
      MIDIDriverDump(int queue_size, FILE *outfile );
      virtual ~MIDIDriverDump();
      
      virtual bool HardwareMsgOut( const MIDITimedBigMessage &msg );
      
      virtual void TimeTick( unsigned long sys_time );
      
    protected:
      
      FILE *f;
    };
  
}

#endif
