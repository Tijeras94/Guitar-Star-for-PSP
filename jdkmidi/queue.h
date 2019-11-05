#ifndef _JDKMIDI_QUEUE_H
#define _JDKMIDI_QUEUE_H

#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  
  class MIDIQueue
    {
    public:
      MIDIQueue( int num_msgs );
      virtual ~MIDIQueue();
      
      void Clear();
      
      bool CanPut() const;
      
      bool CanGet() const;
      
      bool IsFull() const 
        {
          return !CanPut();
        } 
      
      
      void Put( const MIDITimedBigMessage &msg ) 
        {
          buf[next_in] = msg;
          
          next_in = (next_in+1)%bufsize;
        } 
      
      MIDITimedBigMessage Get() const 
        {
          return MIDITimedBigMessage( buf[next_out] );
        } 
      
      void Next() 
        {
          next_out = (next_out+1) % bufsize;
        } 
      
      const MIDITimedBigMessage *Peek() const 
        {
          return &buf[next_out];
        } 
      
    protected:
      MIDITimedBigMessage *buf;
      int bufsize;
      volatile int next_in;
      volatile int next_out;
    };
  
}

#endif
