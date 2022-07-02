/*-------------------------------------------------------------------------
    Dieter Neubacher	     Vers.: 1.00		     Wuhu, 10-07-94
    -----------------------------------------------------------------------
    MvpEntry.cpp

    -----------------------------------------------------------------------
*/

#include <ctype.h>

#include "MvpEntry.h"

int MvpEntry::StrToEntry( char *str )
{
int ret=0,note;

   // get status information

   switch( *str )
   {
    case  MVP_CHAR_NONE   :
		       SetStatus( MVP_NONE );
		       break;

    case  MVP_CHAR_BREAK  :
		       SetStatus( MVP_BREAK );
		       break;

    case  MVP_CHAR_OKTAVE :
		       SetStatus( MVP_OKTAVE );
		       break;

    case  MVP_CHAR_QUARTE :
		       SetStatus( MVP_QUARTE );
		       break;

    case  MVP_CHAR_QUINTE :
		       SetStatus( MVP_QUINTE );
		       break;

    default	     :
		       ret--;
		       break;
   }

   // get Oktaven color information

   str++;
   switch( *str )
   {

    case MVP_CHAR_NO_COLOR:
		       SetOColor( MVP_NO_COLOR );
		       break;

    case MVP_CHAR_RED:
		       SetOColor( MVP_RED );
		       break;

    case MVP_CHAR_BLUE:
		       SetOColor( MVP_BLUE );
		       break;

    case MVP_CHAR_GREEN:
		       SetOColor( MVP_GREEN);
		       break;

    default	     :
		       ret--;
		       break;
   }

   // get Quinten color information

   str++;

   switch( *str )
   {

    case MVP_CHAR_NO_COLOR:
		       SetQColor( MVP_NO_COLOR);
		       break;

    case MVP_CHAR_RED:
		       SetQColor( MVP_RED );
		       break;

    case MVP_CHAR_BLUE:
		       SetQColor( MVP_BLUE);
		       break;

    case MVP_CHAR_GREEN:
		       SetQColor( MVP_GREEN);
		       break;

    default	     :
		       ret--;
		       break;
   }

   // get channel 1 note information

   str++;
   if( (note = StrToNote(str)) == -1 )
   {
     ret--;
   }
   SetCh1Note( note );

   // get channel 2 note information

   str += 3;
   if( (note = StrToNote(str)) == -1 )
   {
     ret--;
   }
   SetCh2Note( note );

   return ret;
}

/*------------------------------------------------------------------------
--------------------------------------------------------------------------
*/

int MvpEntry::Write( FILE *stream )
{
char buf[15];
int  strPos=0, ret;

   if( stream == NULL )
     return -1;

   ret = 0;

   switch( status )
   {

    case  MVP_NONE   :
		       buf[strPos] = MVP_CHAR_NONE;
		       break;

    case  MVP_BREAK  :
		       buf[strPos] = MVP_CHAR_BREAK;
		       break;

    case  MVP_OKTAVE :
		       buf[strPos] = MVP_CHAR_OKTAVE;
		       break;

    case  MVP_QUARTE :
		       buf[strPos] = MVP_CHAR_QUARTE;
		       break;

    case  MVP_QUINTE :
		       buf[strPos] = MVP_CHAR_QUINTE;
		       break;

    default	     :
		       buf[strPos] = MVP_CHAR_ERROR;
		       ret--;
		       break;
   }
   strPos++;

   switch( OColor )
   {

    case MVP_NO_COLOR:
		       buf[strPos] = MVP_CHAR_NO_COLOR;
		       break;

    case MVP_RED:
		       buf[strPos] = MVP_CHAR_RED;
		       break;

    case MVP_BLUE:
		       buf[strPos] = MVP_CHAR_BLUE;
		       break;

    case MVP_GREEN:
		       buf[strPos] = MVP_CHAR_GREEN;
		       break;

    default:
		       buf[strPos] = MVP_CHAR_ERROR;
		       ret--;
		       break;
   }
   strPos++;


   switch( QColor )
   {

    case MVP_NO_COLOR:
		       buf[strPos] = MVP_CHAR_NO_COLOR;
		       break;

    case MVP_RED:
		       buf[strPos] = MVP_CHAR_RED;
		       break;

    case MVP_BLUE:
		       buf[strPos] = MVP_CHAR_BLUE;
		       break;

    case MVP_GREEN:
		       buf[strPos] = MVP_CHAR_GREEN;
		       break;

    default	     :
		       buf[strPos] = MVP_CHAR_ERROR;
		       ret--;
		       break;
   }
   strPos++;


   strPos += NoteToStr( buf+strPos, ch1Note );
   strPos += NoteToStr( buf+strPos, ch2Note );

   fwrite( buf, 1, strPos, stream );

   return ret;
}

//
// write Note to String ( use 3 Character )
//
//	 return :    3	( written Characters )

int MvpEntry::NoteToStr( char *str, int note )
{
    if( note < 0 || note > 127 )
    {
       *(str++) = ' ';
       *(str++) = ' ';
       *(str++) = ' ';
    }
    else
    {
       if( note >= 100 )     // hunderter
       {
	 *(str++) = '1';
       }
       else
       {
	 *(str++) = ' ';
       }

       if( note >= 10 )      // Zehner
       {
	 *(str++) = '0' + (note%100) / 10;
       }
       else
       {
	 *(str++) = ' ';
       }

       // Einer
       *(str++) = '0' + (note%10);
    }
    return 3;
}

//
// String to note ( use 3 Character )
//
//	 return : note
//		  < 0 indicate an error

int MvpEntry::StrToNote( char *str )
{
int note = 0;

    // first character	blank or 1

    if( *str == ' ' )
    {

    }
    else
    if( *str == '1' )
    {
       note = 100;
    }
    else
    {
       return -1;
    }

    // second character	blank or 1..9

    str++;
    if( *str == ' ' )
    {
      if( note > 0 ) return -1;
    }
    else
    if( isdigit( *str ) )
    {
       note += ((*str)-'0') * 10;
    }
    else
    {
       return -1;
    }

    // 3. character	blank or 1..9

    str++;
    if( *str == ' ' )
    {
      if( note > 0 )
	 return -1;
      else
	note = -1;
    }
    else
    if( isdigit( *str ) )
    {
       note += ((*str)-'0');
    }
    else
    {
       return -1;
    }

    if( note > 128 )
    {
       return -1;
    }
    else
       return note;
}
