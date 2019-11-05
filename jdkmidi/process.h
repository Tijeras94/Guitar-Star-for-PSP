#ifndef _JDKMIDI_PROCESS_H
#define _JDKMIDI_PROCESS_H

#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  class MIDIProcessor;
  class MIDIMultiProcessor;
  class MIDIProcessorTransposer;
  class MIDIProcessor;
  
  
  class MIDIProcessor 
    {
    public:
      MIDIProcessor();
      virtual ~MIDIProcessor();
      
      virtual bool Process( MIDITimedBigMessage *msg ) = 0;
    }; 
  
  class MIDIMultiProcessor : public MIDIProcessor 
    {
    public:
      MIDIMultiProcessor( int num_processors );
      virtual ~MIDIMultiProcessor();
      
      // MIDIProcessors given to a MIDIMultiProcessor are NOT owned
      // by MIDIMultiProcessor.
      
      void SetProcessor( int position, MIDIProcessor *proc )
        {
          processors[position]=proc;
        }   
      
      MIDIProcessor *GetProcessor( int position )
        {
          return processors[position];
        }			
      
      const MIDIProcessor *GetProcessor( int position ) const
        {
          return processors[position];
        }
      
      virtual bool Process( MIDITimedBigMessage *msg );
      
    private:
      MIDIProcessor **processors;
      int num_processors;
    }; 
  
  class MIDIProcessorTransposer : public MIDIProcessor 
    {
    public:
      MIDIProcessorTransposer();
      virtual ~MIDIProcessorTransposer();
      
      void SetTransposeChannel( int chan, int trans )
        {
          trans_amount[chan] = trans;
        }
      
      int GetTransposeChannel( int chan ) const
        {
          return trans_amount[chan];
        }
      
      void SetAllTranspose( int trans );
      
      virtual bool Process( MIDITimedBigMessage *msg );
    private:
      int trans_amount[16];
    }; 
  
  class MIDIProcessorRechannelizer : public MIDIProcessor 
    {
    public:
      MIDIProcessorRechannelizer();
      virtual ~MIDIProcessorRechannelizer();
      
      void SetRechanMap( int src_chan, int dest_chan )
        {
          rechan_map[src_chan] = dest_chan;
        }
      
      int GetRechanMap( int src_chan ) const
        {	
          return rechan_map[src_chan];
        }
      
      void SetAllRechan( int dest_chan );
      
      virtual bool Process( MIDITimedBigMessage *msg );
      
    private:
      
      int rechan_map[16];
    }; 
}

#endif
