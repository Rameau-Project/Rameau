/*--------------------------------------------------------------------------
Dieter Neubacher      Vers.: 1.0                             23.06.96
Dieter Neubacher      Vers.: 2.0                             01.02.97
------------------------------------------------------------------------
system.cpp

Vesion 2.0    WIN32
----------------------------------------------------------------------------
*/

#include <process.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>   
#include <sys\types.h>
#include <sys\utime.h>
#include <sys\stat.h>
#include <sys\timeb.h>                    
#include <time.h>
#include <dos.h>
#include <io.h>
#include <conio.h>
#include <direct.h>

#include "rameau.h"
#include "system.h"
#include "screen.h"
#include "debug.h"

#ifdef WIN32

#else
struct _videoconfig vc;
#endif



/*-------------------------------------------------------------------------
    change to Directory
---------------------------------------------------------------------------
*/
int ToRameauBinDir()
{
    return chdir("\\Rameau95\\Bin");
}
int ToRameauDir()
{
    return chdir("\\Rameau95");
}
int ToWorkDir()
{
    return chdir("\\Rameau95\\data");
} 

/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/
void    ReadOnly( char *Filename )
{   
    _chmod( Filename, _S_IREAD );
}
void    ReadWrite( char *Filename )
{
    _chmod( Filename, _S_IREAD | _S_IWRITE );
}

/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/
#ifdef WIN32

#else
int     SetRameauVideomode()
{  
int mode = 0xA0;

   _getvideoconfig( &vc );       
   
   return _setvideomoderows( _TEXTC80,25 );
}
int RestoreVideomode()
{
    return _setvideomoderows( vc.mode, vc.numtextrows );
}
#endif
/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/
int DoSystemCall (char *str)
{
char    buffer[MAX_COMMAND_LENGTH];

#ifdef DEBUG_SYSTEM_CALL
   fprintf (stdout, "\n%s", str);
   getch ();
#endif /*  */

   flushall();

   if (system (str) != 0)
   {  
      // sprintf (buffer, "!!! error while proccessing %s !!!", str);
      // perror( buffer );
      // DispErrorMassage (buffer);
      
      if( errno == E2BIG )       
      { 
         sprintf (buffer, "!!! argument list exceeds 128 bytes, or environment information exceeds 32K. !!!", str);
         Screen.DispError(buffer);
      }
      if( errno == ENOENT )
      { 
         sprintf (buffer, "!!! The command interpreter cannot be found. !!!", str);
         Screen.DispError (buffer);
      }
      if( errno == ENOEXEC )
      { 
         sprintf (buffer, "!!! The command-interpreter file has an invalid format and is not executable.!!!", str);
         Screen.DispError(buffer);
      }
      if( errno == ENOMEM )
      { 
         sprintf (buffer, "!!! Not enough memory is available to execute the command. !!!", str);
         Screen.DispError(buffer);
      }
   }
   else
   {  
      return (0);
   }
   return 0;
}
/*--------------------------------------------------------------------------
    get keyboard input
----------------------------------------------------------------------------
*/

int GetKey (void)
{
int     ch, sc;

   Screen.Pos(0,0);
#ifdef __MS_DOS__
   while (kbhit ()) getch (); /* clear keyboard buffer */
      
   do
   {
      if ((ch = getch ()) == 0 || ch == 0xE0 )
      {
        sc = getch ();      /* read scan code */
      }
   }
   while (ch == 0 || ch == 0xE0 );

#else /* __MS_DOS__ */
#error input() function not implementet in this version
#endif /* __MS_DOS__ */
   return (LOW_CHAR (ch));

}
/*-----------------------------------------------------------------------
Displays information about a file.
-------------------------------------------------------------------------
*/
#ifdef WIN32

#else

long FileInfo (FILE * stream, struct _find_t *pfind)
{

   char    timebuf[10], datebuf[10], *pkind;
   datestr (pfind->wr_date, datebuf);
   timestr (pfind->wr_time, timebuf);

   if (pfind->attrib & _A_SUBDIR)
      pkind = "Directory";
   else if (pfind->attrib & _A_VOLID)
      pkind = "Label";
   else
      pkind = "File";

#ifdef DIR_SHOW_FILE_FLAGS
   fprintf (stream, "%-12s   %8ld    %8s   %8s   %-9s    %c   %c   %c   %c\n",
        pfind->name, pfind->size, timebuf, datebuf, pkind,
        (pfind->attrib & _A_RDONLY) ? 'Y' : 'N',
        (pfind->attrib & _A_HIDDEN) ? 'Y' : 'N',
        (pfind->attrib & _A_SYSTEM) ? 'Y' : 'N',
        (pfind->attrib & _A_ARCH)   ? 'Y' : 'N');
#else /*  */
   fprintf (stream, "%-12s   %8ld    %8s   %8s   %c%c%c%c\n",
        pfind->name, pfind->size, timebuf, datebuf,
        (pfind->attrib & _A_RDONLY) ? 'r' : '-',
        (pfind->attrib & _A_HIDDEN) ? 'h' : '-',
        (pfind->attrib & _A_SYSTEM) ? 's' : '-',
        (pfind->attrib & _A_ARCH) ? 'a' : '-');

#endif /*  */
   return pfind->size;

}
#endif // WIN32

/*-------------------------------------------------------------------------
*
* Funktion     :  void waittime( long diff_time );
* Parameter    :  diff_time : wait for diff_time in millisec
*
* Return value :  no error return
--------------------------------------------------------------------------
*/

#ifdef __GNUC__

char   *itoa (int val, char *string, int radix)
{
   switch (radix)
   {
   case 10:
      sprintf (string, "%d", val);
      break;
   case 16:
      sprintf (string, "%x", val);
      break;
   case 8:
      sprintf (string, "%o", val);
      break;
   default:
      strcpy (string, "ERROR");
   }
   return (string);
}

unsigned long dos_timer (void)
{
unsigned long t;
unsigned short l, h;
union REGS r;

   r.h.ah = 0;
   int86 (0x1A, &r, &r);
   h = r.x.cx;
   l = r.x.dx;
   t = h << 16;
   t += (0x0000FFFFL & l);
   return (t);
}

void waittime (long diff_time)
{
   unsigned long wait_time, start_time, act_time;

   start_time = dos_timer ();
   do
   {
      act_time = dos_timer ();
      if (act_time < start_time)
     act_time += start_time;
      wait_time = (act_time - start_time) * (long) (1000.0 / 18.21);
   }
   while (wait_time < diff_time);
}

int getch (void)
{
static int ch, flag = 0;

   if (flag == 0)
   {
      if ((ch = getkey ()) > 255)
      {
     flag = 1;
     ch -= 256;
     return (0);
      }
      else
      {
     return (ch);
      }
   }
   else
   {
      flag = 0;
      return (ch);
   }
}

#else /* __GNUC__ */

int WaitTime( long diff_time, int key )
{
      struct timeb start_time, act_time;
      long    wait_time;
      ftime (&start_time);
      do
      {
         if (kbhit ())
            if(getch () == key)
               return key;
            else
               continue;
         ftime (&act_time);
         wait_time = (act_time.time - start_time.time) * 1000
         + ((long) act_time.millitm - (long) start_time.millitm);
      }while (wait_time < diff_time);
      return 0;

}

void WaitTime (long diff_time)
{
struct timeb start_time, act_time;
long    wait_time;

   ftime (&start_time);

   do
   {
      ftime (&act_time);
      wait_time = (act_time.time - start_time.time) * 1000
      + ((long) act_time.millitm - (long) start_time.millitm);
   }
   while (wait_time < diff_time);
}

#endif /* __GNUC__ */


/* Takes unsigned time in the format:       fedcba9876543210
 * s=2 sec incr, m=0-59, h=23               hhhhhmmmmmmsssss
 * Changes to a 9-byte string (ignore seconds):     hh:mm ?m
 */
char   *timestr (unsigned t, char *buf)
{
int h = (t >> 11) & 0x1f, m = (t >> 5) & 0x3f;

   sprintf (buf, "%2.2d:%02.2d %cm", h % 12, m, h > 11 ? 'p' : 'a');
   return buf;
}
/* Takes unsigned date in the format:           fedcba9876543210
 * d=1-31, m=1-12, y=0-119 (1980-2099)          yyyyyyymmmmddddd
 * Changes to a 9-byte string:              mm/dd/yy
 */
char   *datestr (unsigned d, char *buf)
{
   sprintf (buf, "%2.2d/%02.2d/%02.2d",(d >> 5) & 0x0f, d & 0x1f, (d >> 9) + 80);
   return buf;
}


/*--------------------------------------------------------------------------
    test file   print errormassige if file not found
----------------------------------------------------------------------------
*/
int TestFile( char *filename )
{
char buffer[MAX_COMMAND_LENGTH];
struct _stat buf;

   /* Get data associated with file: */
   if( _stat( filename, &buf ) != 0 )
   {
      sprintf (buffer, "!!! file \"%s\" open error !!!", filename);
      Screen.DispError (buffer);
      return (-1);
   }
   else
   {  
      if( buf.st_mode & _S_IFDIR )
      {
         sprintf (buffer, "!!! file \"%s\" is a directory !!!", filename);
         Screen.DispError (buffer);
         return (-2);
      }
      if( !(buf.st_mode & _S_IWRITE) )
      {
         sprintf (buffer, "!!! file \"%s\" is read only !!!", filename);
         Screen.DispError (buffer);
         return (-3);
      }            
      return 0;
   }      
}
/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/
