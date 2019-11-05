#ifndef _JDKMIDI_MANAGER_H
#define _JDKMIDI_MANAGER_H

#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/driver.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/tick.h"

namespace jdkmidi
{  
  class MIDIManager : public MIDITick 
    {
    public:
      MIDIManager(
        MIDIDriver *drv,
        MIDISequencerGUIEventNotifier *n=0,
        MIDISequencer *seq_=0
        );
      
      virtual ~MIDIManager();
      
      void Reset();
      
      // to set and get the current sequencer
      void SetSeq( MIDISequencer *seq );
      MIDISequencer *GetSeq();
      const MIDISequencer *GetSeq() const;
      
      // to get the driver that we use
      MIDIDriver *GetDriver()
        {
          return driver;
        }
      
      
      // to set and get the system time offset
      void SetTimeOffset( unsigned long off );
      unsigned long GetTimeOffset();
      
      // to set and get the sequencer time offset
      void SetSeqOffset( unsigned long seqoff );
      unsigned long GetSeqOffset();
      
      
      // to manage the playback of the sequencer
      void SeqPlay();
      void SeqStop();
      void SetRepeatPlay(
        bool flag,
        unsigned long start_measure,
        unsigned long end_measure
        );
      
      
      // status request functions
      bool IsSeqPlay() const;
      bool IsSeqStop() const;
      bool IsSeqRepeat() const;
      
      // inherited from MIDITick
      virtual void TimeTick( unsigned long sys_time );
      
    protected:
      
      virtual void TimeTickPlayMode( unsigned long sys_time_ );
      virtual void TimeTickStopMode( unsigned long sys_time_ );
      
      MIDIDriver *driver;
      
      MIDISequencer *sequencer;
      
      unsigned long sys_time_offset;
      unsigned long seq_time_offset;
      
      volatile bool play_mode;
      volatile bool stop_mode;
      volatile bool repeat_play_mode;
      
      long repeat_start_measure;
      long repeat_end_measure;
      
      MIDISequencerGUIEventNotifier *notifier;
    }; 
  
  
}

#endif
