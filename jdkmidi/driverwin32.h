#ifndef _JDKMIDI_DRIVERWIN32_H
#define _JDKMIDI_DRIVERWIN32_H

#include "jdkmidi/driver.h"
#include "jdkmidi/sequencer.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>

namespace jdkmidi
{
  
  class MIDISequencerGUIEventNotifierWin32 :
    public MIDISequencerGUIEventNotifier
    {
    public:
      MIDISequencerGUIEventNotifierWin32(
        HWND w,
        DWORD wmmsg,
        WPARAM wparam_value_=0
        );
      
      virtual ~MIDISequencerGUIEventNotifierWin32();
      
      virtual void Notify( const MIDISequencer *seq, MIDISequencerGUIEvent e );
      virtual bool GetEnable() const;
      virtual void SetEnable( bool f );
      
    private:
      
      HWND dest_window;
      DWORD window_msg;
      WPARAM wparam_value;
      bool en;
    };
  
  
  
  class MIDIDriverWin32 : public MIDIDriver
    {
    public:
      MIDIDriverWin32( int queue_size);
      virtual ~MIDIDriverWin32();
      
      void ResetMIDIOut();
      
      bool StartTimer( int resolution_ms );
      bool OpenMIDIInPort( int id );
      bool OpenMIDIOutPort( int id );
      
      void StopTimer();
      void CloseMIDIInPort();
      void CloseMIDIOutPort();
      
      
      bool HardwareMsgOut( const MIDITimedBigMessage &msg );
      
    protected:
      
      static void CALLBACK win32_timer(
        UINT wTimerID,
        UINT msg,
        DWORD dwUser,
        DWORD dw1,
        DWORD dw2
        );
      
      static void CALLBACK win32_midi_in(
        HMIDIIN hMidiIn,
        UINT wMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2
        );
      
      HMIDIIN in_handle;
      HMIDIOUT out_handle;
      int timer_id;
      int timer_res;
      
      bool in_open;
      bool out_open;
      bool timer_open;
    }; 
  
  
}
#endif

#endif
