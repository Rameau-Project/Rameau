/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 22-06-94
                             Vers.: 1.10                           15.07.96 
    -----------------------------------------------------------------------
    mvpEntry.h
    -----------------------------------------------------------------------
*/


#if !defined( _MVPENTRY_H_ )

#define _MVPENTRY_H_

#include <stdlib.h>
#include <stdio.h>

#define MVP_MAX_CHANNEL 16
#define MVP_MAX_ENTRIES 10
#define MVP_IO_BUF_SIZE 1024

#define MVP_ENTYRY_STR_SIZE 9

#define MVP_NO_COLOR   0x00
#define MVP_RED        0x01
#define MVP_BLUE       0x02
#define MVP_GREEN      0x03

#define MVP_CHAR_NO_COLOR   ' '
#define MVP_CHAR_RED        'r'
#define MVP_CHAR_BLUE       'b'
#define MVP_CHAR_GREEN      'g'

#define MVP_NONE   0x00
#define MVP_BREAK  0x01
#define MVP_OKTAVE 0x02
#define MVP_QUARTE 0x04
#define MVP_QUINTE 0x08

#define MVP_ERROR  0xFF

#define MVP_CHAR_NONE       ' '
#define MVP_CHAR_BREAK      'B'
#define MVP_CHAR_OKTAVE     'O'
#define MVP_CHAR_QUARTE     'q'
#define MVP_CHAR_QUINTE     'Q'

#define MVP_CHAR_ERROR      'e'

//    Channel notes are set to -1 if not used
//
//
//
//



class MvpEntry
{
public:

      MvpEntry() { status=0; OColor=0; QColor=0; ch1Note=ch2Note=-1; };
      MvpEntry( int Status, int ch1Note, int ch2Note, int Ocolor, int Qcolor );

      // Copy constructor

      MvpEntry( const MvpEntry &other )
      {
         l = other.l;
      };

      // Assignment operator

      MvpEntry &operator=( const MvpEntry &other )
      {
         l = other.l;
         return *this;
      };

void  Clear() { status=0; OColor=0; QColor=0; ch1Note=ch2Note=-1; };
void  SetError() { status=0xFF; OColor=0; QColor=0; ch1Note=ch2Note=-1; };

int   GetStatus() { return (int) status; };
int   GetOColor() { return (int) OColor; };
int   GetQColor() { return (int) QColor; };
int   GetCh1Note() { return (int) ch1Note; };
int   GetCh2Note() { return (int) ch2Note; };
int   GetCh1ModNote() { return (int) ch1Note % 12; };
int   GetCh2ModNote() { return (int) ch2Note % 12; };
int   GetModDiff()
      {
         if( ch1Note < 0 || ch2Note < 0 )
           return -1;
         else
         if( ch1Note < ch2Note )
           return ( ch2Note - ch1Note ) % 12;
         else
           return ( ch1Note - ch2Note ) % 12;
      };                                   
void  SetStatus( int Status ) { this->status=Status; };
void  SetOColor( int OColor) { this->OColor=OColor; };
void  SetQColor( int QColor) { this->QColor=QColor; };
void  SetCh1Note( int Ch1Note)
      {
         if( Ch1Note > 127  || Ch1Note < 0 )
            ch1Note=-1;
         else
            ch1Note=Ch1Note;
      };
void  SetCh2Note( int Ch2Note )
      {
         if( Ch2Note > 127  || Ch2Note < 0 )
            ch2Note=-1;
         else
            ch2Note=Ch2Note;
      };
void  Set( int Status, int ch1Note, int ch2Note, int Ocolor, int Qcolor );

int      StrToEntry( char *str );
int      Write( FILE *stream );


private:

   union
   {
      long l;
      struct
      {
         unsigned char status;
         char       ch1Note;
         char       ch2Note;
         union
         {
         unsigned char color;
         struct
         {
           unsigned char OColor : 2;   /* bit 0..1 */
           unsigned char QColor : 2;   /* bit 2..3 */
         };
         };
      };
   };

int NoteToStr( char *str, int note );
int StrToNote( char *str );

};


#endif // MVP_ENTRY
