#include "jdkmidi/world.h"
#include "jdkmidi/queue.h"

namespace jdkmidi
{
  
  
  MIDIQueue::MIDIQueue( int num_msgs ) 
    :
    buf( new MIDITimedBigMessage[ num_msgs ] ),
    bufsize( num_msgs ),
    next_in(0),
    next_out(0)
  {
    
  }
  
  
  MIDIQueue::~MIDIQueue()
  {
    delete [] buf;
  }
  
  void MIDIQueue::Clear()
  {
    next_in=0; 
    next_out=0; 	
  }
  
  bool MIDIQueue::CanPut() const
  {
    return next_out != ((next_in+1)%bufsize);
  }
  
  bool MIDIQueue::CanGet() const
  {
    return next_in != next_out;
  }
  
  
  
}
