/*---------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.1                 WuHu 01.07.94  
                             Vers,: 1.4                      21.07.96
							 Vers.: 2.0                      25.01.97
    -------------------------------------------------------------------
    Version.cpp

  Vers.: 2.0	anpassungen für WIN32
-----------------------------------------------------------------------
*/

#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>

#include "version.h"

/*---------------------------------------------------------------------
     R  A  M  E  A  U   Music Analysis Software
-----------------------------------------------------------------------
*/
void rameau_version (FILE * stream)
{
   fprintf (stream, "\nRAMEAU Music Analysis System Version 2.0\n");
   fprintf (stream, "\n(c) 1994-97 by ARION Verlag, Muenchen\n\n");
   return;
}
/*--------------------------------------------------------------------------
    test file   print errormassige if file not found
----------------------------------------------------------------------------
*/
int SetFileInfoLine( FILE *stream, char *FileName )
{
struct _stat buf;

   /* Get data associated with file: */
   if( _stat( FileName, &buf ) != 0 )
   {
      return (-1);
   }
   else
   {  
      if( !(buf.st_mode & _S_IWRITE) )
      {
         return (-3);
      }            
   }      
      
   // Set File information for Printing Header
#ifdef WIN32
   fprintf( stream, "* File : %s Time : %s", FileName, ctime( &buf.st_mtime ) );
#else
   fprintf( stream, "* File : %s Time : %s", FileName, ctime( &buf.st_atime ) );
#endif
   return 0;
}
