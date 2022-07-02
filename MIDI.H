/*---------------------------------------------------------------------
    Dieter Neubacher	      Vers.: 1.0            Wuhu,      22.02.95
    -------------------------------------------------------------------
    midi.h                                                 
-----------------------------------------------------------------------
*/

#if !defined( _MIDI_H_ )

#define _MIDI_H_

#include <stdlib.h>
#include <stdio.h>


#define INP_EXT     "mid"
#define OUT_EXT     "rdl"
#define BAR_EXT     "bar" 
        
#define MAX_BAR_NUMBER 32000        
        
#define SYS_EXCL_F0   0xF0
#define SYS_EXCL_F7   0xF7

#define EO_COMMEN  0xF7

#define CLOCK	   0xF8
#define START      0xFA
#define CONTINUE   0xFB
#define STOP       0xFC
#define ACT_SEN    0xFE
#define RESET	   0xFF
#define META_EVENT 0xFF


/*---------------------------*/
/*			                 */
/*---------------------------*/

#define NOTE_OFF      0x80
#define NOTE_ON       0x90
#define CHANNEL	      0x0F
#define ALL_NOTES_OFF 0xB0
#define MIDI_EVENT    0x80
#define NONE	      0x00         

/*-----------------------------*/
/* midi meta events	           */
/*-----------------------------*/

#define ME_SEQUENCE_NUMBER    0x00
#define ME_TEXT_EVENT	      0x01
#define ME_COPYRIGTH	      0x02
#define ME_TRACK_NAME	      0x03
#define ME_INSTRUMENT_NAME    0x04
#define ME_LYRIC	          0x05
#define ME_MARKER	          0x06
#define ME_CUE_POINT	      0x07
#define ME_CHANNEL_PREFIX     0x20
#define ME_END_OF_TRACK       0x2F
#define ME_SET_TEMPO	      0x51
#define ME_SMPTE_OFFSET       0x54
#define ME_TIME_SIGNATURE     0x58
#define ME_KEY_SIGNATUR       0x59
#define ME_SEQUENCER_SPECIFIC 0x7F

class BarInfo
{
public:
      
      BarInfo();
      ~BarInfo() { if( InfoStream != NULL ){fclose( InfoStream ); InfoStream=NULL;} }
int   GetBarNum() {return CallCounter-1;}      
void  SetInfoStream(FILE *inFile);
void  SetTicks( int TicksPerQuaterNote ) {TicksPerNote = (long)TicksPerQuaterNote * 4L;}
void  SetOffBeat( int OffBeatZ, int OffBeatN );
void  SetSpeed( int BarTimeZ, int BarTimeN );
void  Debug( FILE *OutSteam );      
long  NextBarTime();

private:
     
      int NextLine();
      
FILE   *InfoStream;
int     CallCounter;
long    Time, DeltaTime;
long    TicksPerNote;
int     InputLineCount;
int     Bar,Z,N;
         
         
protected:

};



#endif /* _MIDI_H_ */