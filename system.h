#ifndef _RAMEAU_SYSTEM_H_
#define _RAMEAU_SYSTEM_H_
           
#include "rameau.h"
                             
#ifdef __MS_DOS__
#include <stdio.h>
#include <dos.h>
#else
//#error system.h only for MS-DOS : FileInfo
#endif                               


//define DEBUG_SYSTEM_CALL

/*---------------------------*/
/* keyboard input            */
/* uppercase == lowercase    */
/*---------------------------*/

#define UPPER_LOWER_CASE

#ifdef UPPER_LOWER_CASE
#define LOW_CHAR( a )  ( ( ((a)>='A') && ((a)<='Z') ) ? ((a)|0x20) : (a) )
#else /* UPPER_LOWER_CASE */
#define LOW_CHAR( a )   (a)
#endif /* UPPER_LOWER_CASE */




long    FileInfo (FILE * stream, struct _find_t *find); /* Prototypes */
char   *timestr (unsigned d, char *buf);
char   *datestr (unsigned d, char *buf);
void    WaitTime (long diff_time);
int     DoSystemCall (char *);;
int     WaitTime( long diff_time, int key );
void    WaitTime (long diff_time);
int     GetKey (void);
int     ToRameauDir( void );
int     ToRameauBinDir( void );
int     ToWorkDir( void );
void    ReadOnly( char *Filename );
void    ReadWrite( char *Filename );     
int     TestFile( char *filename );

#ifdef WIN32

#else
int     SetRameauVideomode();
int     RestoreVideomode();
#endif

#endif // _RAMEAU_SYSTEM_H_