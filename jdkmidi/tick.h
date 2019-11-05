#ifndef _JDKMIDI_TICK_H
#define _JDKMIDI_TICK_H

namespace jdkmidi
{
  class MIDITick
    {
    public:
      MIDITick()
        {	
        }
      
      virtual ~MIDITick();
      
      virtual void TimeTick( unsigned long sys_time ) = 0;
    };
}

#endif
