/*---------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.1                 WuHu 01.07.94
    -------------------------------------------------------------------
    Version.h
-----------------------------------------------------------------------
*/

/*---------------------------------------------------------------------
     R  A  M  E  A  U   Music Analysis Software
-----------------------------------------------------------------------
*/
#ifndef _VERSION_H_

#define _VERSION_H_

#include <stdio.h>

void rameau_version (FILE * stream);
int  SetFileInfoLine( FILE *stream, char *FileName );

#endif /* _VERSION_H_ */
