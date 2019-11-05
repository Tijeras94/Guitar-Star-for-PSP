#include "jdkmidi/world.h"
#include "jdkmidi/driverdump.h"

namespace jdkmidi
{
  
  
  MIDIDriverDump::MIDIDriverDump(int queue_size, FILE *outfile )
    : 
    MIDIDriver( queue_size ),
    f(outfile)
  {
  }
  
  MIDIDriverDump::~MIDIDriverDump()
  {
  }
  
  
  bool MIDIDriverDump::HardwareMsgOut( const MIDITimedBigMessage &msg )
  {
    char buf[256];
    
    fprintf( f, "OUTPUT: %s\n", msg.MsgToText(buf) );
    return true;
  }
  
  
  void MIDIDriverDump::TimeTick( unsigned long sys_time )
  {
    fprintf( f, "TICK  : %8ld\n", sys_time );
    MIDIDriver::TimeTick( sys_time );
  }
  
  
}
