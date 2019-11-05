#include "jdkmidi/world.h"
#include "jdkmidi/process.h"

namespace jdkmidi
{
  
  
  MIDIProcessor::MIDIProcessor()
  {
  }
  
  MIDIProcessor::~MIDIProcessor()
  {
  }
  
  
  
  
  MIDIMultiProcessor::MIDIMultiProcessor( int num ) 
    :
    processors( new MIDIProcessor *[num] ),
    num_processors( num )
  {   
    for( int i=0; i<num_processors; ++i )
    {
      processors[i] = 0;
    }
  }
  
  MIDIMultiProcessor::~MIDIMultiProcessor()
  {
    delete [] processors;
  }
  
  
  
  bool MIDIMultiProcessor::Process( MIDITimedBigMessage *msg )
  {
    for( int i=0; i<num_processors; ++i )
    {
      if( processors[i] )
      {
        if( processors[i]->Process( msg )==false )
        {
          return false;
        }			
      }
      
    }	
    
    return true;
  }
  
  
  
  
  
  MIDIProcessorTransposer::MIDIProcessorTransposer()
  {
    for( int i=0; i<16; ++i )
    {
      trans_amount[i] = 0;
    }
    
  }
  
  MIDIProcessorTransposer::~MIDIProcessorTransposer()
  {
  }
  
  bool MIDIProcessorTransposer::Process( MIDITimedBigMessage *msg )
  {
    if( msg->IsChannelMsg() )
    {		
      if( msg->IsNoteOn() || msg->IsNoteOff() || msg->IsPolyPressure() )
      {
        int trans = trans_amount[ msg->GetChannel() ];
        
        int new_note = ((int)msg->GetNote())+trans;
        
        if( trans>127 || trans<0 )
        {
          // delete event if out of range
          return false;
        }			
        else
        {
          // set new note number
          msg->SetNote( (unsigned char)new_note );
        }
        
      }
    }
    
    return true;
  }
  
  
  
  
  
  MIDIProcessorRechannelizer::MIDIProcessorRechannelizer()
  {
    for( int i=0; i<16; ++i )
    {
      rechan_map[i] = i;
    }
    
  }
  
  MIDIProcessorRechannelizer::~MIDIProcessorRechannelizer()
  {
  }
  
  
  void MIDIProcessorRechannelizer::SetAllRechan( int dest_chan )
  {
    for( int i=0; i<16; ++i )
    {
      rechan_map[i] = dest_chan;
    }
  }
  
  bool MIDIProcessorRechannelizer::Process( MIDITimedBigMessage *msg )
  {
    if( msg->IsChannelMsg() )
    {		
      int new_chan = rechan_map[ msg->GetChannel() ];
      
      if( new_chan==-1 )
      {
        // this channel is to be deleted! return false
        return false;
      }
      
      msg->SetChannel( (unsigned char)new_chan );
    }
    
    return true;
  }
  
  
  
  
  
}
