/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_SMPTE_H
#define _JDKMIDI_SMPTE_H

namespace jdkmidi
{
  
  enum SMPTE_RATE
    {
      SMPTE_RATE_24=0,
      SMPTE_RATE_25,
      SMPTE_RATE_2997,
      SMPTE_RATE_2997DF,
      SMPTE_RATE_30,
      SMPTE_RATE_30DF
    };
  
  
  
  enum SAMPLE_RATE
    {
      SAMPLE_32000=0,
      SAMPLE_44056,
      SAMPLE_44100,
      SAMPLE_47952,
      SAMPLE_48000,
      SAMPLE_48048
    };
  
  
  
  
//
// MDGetSMPTERateFrequency() converts the SMPTE_RATE enum to a double frequency.
//
  
  inline double GetSMPTERateFrequency( SMPTE_RATE r )
    {
      extern const double smpte_smpte_rates[];
      
      return smpte_smpte_rates[(int)r];
    }
  
  
//
// MDGetSMPTERateFrequencyLong() convert the SMPTE_RATE enum to an int
// frequency times 100
//
  
  inline long GetSMPTERateFrequencyLong( SMPTE_RATE r )
    {
      extern const double smpte_smpte_rates_long[];
      
      return (long)smpte_smpte_rates_long[(int)r];
    }
  
  
//
// GetSampleRateFrequency() convert the SAMPLE_RATE enum to a double frequency
//
  
  
  inline 	double 	GetSampleRateFrequency( SAMPLE_RATE r )
    {
      extern const double smpte_sample_rates[];
      
      return smpte_sample_rates[(int)r];
    }
  
  
//
// MDGetSampleRateFrequencyLong() converts the SAMPLE_RATE enum to a long
// frequency times 10
//
  
  
  inline	long	GetSampleRateFrequencyLong( SAMPLE_RATE r )
    {
      // return the sample rate as a long word of the frequency times 10.
      
      extern const long smpte_sample_rates_long[];
      return smpte_sample_rates_long[(int)r];
    }
  
  
  
  
  
  
  class  SMPTE
    {
    public:
      SMPTE(
        SMPTE_RATE smpte_rate=SMPTE_RATE_30,
        SAMPLE_RATE sample_rate=SAMPLE_48000
        );
      
      SMPTE(
        const SMPTE & s
        );
      
      void	SetSMPTERate( SMPTE_RATE r )	{ smpte_rate=r; sample_number_dirty=true;	}
      SMPTE_RATE GetSMPTERate()		{ return smpte_rate;				}
      
      void	SetSampleRate( SAMPLE_RATE r )	{ sample_rate=r; sample_number_dirty=true;	}
      SAMPLE_RATE GetSampleRate()		{ return sample_rate;				}	 
      
      void	SetSampleNumber( ulong n )	{ sample_number=n; SampleToTime();	 	}
      ulong	GetSampleNumber()	{ if( sample_number_dirty ) TimeToSample(); return sample_number; }
      
      void	SetTime( uchar h, uchar m, uchar s, uchar f=0, uchar sf=0 )
        { hours=h; minutes=m; seconds=s; frames=f; sub_frames=sf; sample_number_dirty=true;	}
      
      uchar	GetHours()		{ return hours;		}
      uchar	GetMinutes()		{ return minutes;	}
      uchar	GetSeconds()		{ return seconds;	}
      uchar 	GetFrames()		{ return frames;	}
      uchar	GetSubFrames()		{ return sub_frames;	}
      
      void	SetHours( uchar h )	{ hours=h;   	 sample_number_dirty=true;	}
      void	SetMinutes( uchar m )	{ minutes=m; 	 sample_number_dirty=true;	}
      void	SetSeconds( uchar s )	{ seconds=s; 	 sample_number_dirty=true;	}
      void	SetFrames( uchar f )	{ frames=f;  	 sample_number_dirty=true;	}
      void	SetSubFrames( uchar sf)	{ sub_frames=sf; sample_number_dirty=true;	}
      
      void	AddHours( char h );
      void	AddMinutes( char m );
      void	AddSeconds( char s );
      void	AddFrames( char f );
      void	AddSubFrames( char sf );
      void	AddSamples( long n )	{ sample_number=GetSampleNumber()+n; SampleToTime();	}
      
      void	IncHours()	{ AddHours(1);		}
      void	IncMinutes()	{ AddMinutes(1);	}
      void	IncSeconds()	{ AddSeconds(1);	}
      void	IncFrames()	{ AddFrames(1);		}
      void	IncSubFrames()	{ AddSubFrames(1);	}
      void	IncSamples()	{ AddSamples(1);	}
      
      void	DecHours()	{ AddHours(-1);		}
      void	DecMinutes()	{ AddMinutes(-1);	}
      void	DecSeconds()	{ AddSeconds(-1);	}
      void	DecFrames()	{ AddFrames(-1);	}
      void	DecSubFrames()	{ AddSubFrames(-1);	}
      void	DecSamples()	{ AddSamples(-1);	}
      
      
      
      const SMPTE & operator =  ( const SMPTE & s )	{ Copy(s); return *this;	}
      bool operator == ( SMPTE & s )	{ return Compare(s)==0;		}
      bool operator != ( SMPTE & s )	{ return Compare(s)!=0;		}
      bool operator <  ( SMPTE & s )	{ return Compare(s)<0;		}
      bool operator >  ( SMPTE & s )	{ return Compare(s)>0;		}
      bool operator <= ( SMPTE & s )	{ return Compare(s)<=0;		}
      bool operator >= ( SMPTE & s )	{ return Compare(s)>=0;		}
      
      const SMPTE & operator += ( SMPTE & s )	{ Add( s ); return *this;	}
      const SMPTE & operator -= ( SMPTE & s )	{ Subtract( s ); return *this;	}
      
    protected:
      void	SampleToTime();
      void	TimeToSample();
      
      void	Copy( const SMPTE & s );
      int	Compare( SMPTE & s );
      void	Add( SMPTE & s );
      void	Subtract( SMPTE & s );
      
      long	GetSampleRateLong()
        { return GetSampleRateFrequencyLong( sample_rate );	}
      
      int	GetSMPTERateLong()
        { return GetSMPTERateFrequencyLong( smpte_rate );	}
      
    private:
      SMPTE_RATE 	smpte_rate;
      SAMPLE_RATE	sample_rate;
      ulong		sample_number;
      
      uchar		hours;
      uchar		minutes;
      uchar		seconds;
      uchar		frames;
      uchar		sub_frames;
      uchar		sample_number_dirty;
      
      
      friend	SMPTE operator + ( SMPTE a, SMPTE b );
      friend	SMPTE operator - ( SMPTE a, SMPTE b );
    };
  
  inline SMPTE operator + ( SMPTE a, SMPTE b )
    {
      SMPTE c(a);
      
      c+=b;
      return c;
    }
  
  inline SMPTE operator - ( SMPTE a, SMPTE b )
    {
      SMPTE c(a);
      
      c-=b;
      return c;
    }
  
}

#endif


