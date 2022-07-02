/*---------------------------------------------------------------------------
    Dieter Neubacher		   Vers. 1.0		    04.01.94
    -------------------------------------------------------------------------
    getInfo.c		  Read Rameau Info Files
-----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <ctype.h>

#include "rameau.h"
#include "GetInfo.h"

#ifdef __MS_DOS__
#include <conio.h>
#include <graph.h>
#include <direct.h>
#else /*  */
#define getch(a) getchar()
#endif /*  */

#define ESC    ('\x1b')
#define RET    ('\r')

/*---------------------------*/
/* global variables	     */
/*---------------------------*/



/*---------------------------*/
/* init PS info fields	     */
/*---------------------------*/

PS_INFO_FIELD PsInfoTxt[] =
{
 /* x, y, input size */
   {
      0, 9, 6, "Analysis No", ""
   }
   ,
   {
      44, 9, 8, "Date", ""
   }
   ,
   {
      0, 8, 58, "Title", ""
   }
   ,
   {
      0, 7, 55, "Composer", ""
   }
   ,
   {
      0, 6, 44, "Opus", ""
   }
   ,
   {
      44, 6, 8, "Year", ""
   }
   ,
   {
      0, 5, 21, "Publisher", ""
   }
   ,
   {
      44, 5, 24, "Editor", ""
   }
   ,
   {
      0, 4, 55, "Movement", ""
   }
   ,
   {
      0, 3, 8, "Key", ""
   }
   ,
   {
      44, 3, 5, "Time Signature", ""
   }
   ,
   {
      0, 2, 4, "Bar Nos from", ""
   }
   ,
   {
      23, 2, 4, "to", ""
   }
   ,
   {
      44, 2, 5, "Offbeat Value", ""
   }
   ,
   {
      0, 1, 5, "Minimal Rhythm Value", ""
   }
   ,
   {
      0, 0, 55, "Input By", ""
   }
};


/*--------------------------------------------------------------------------
    read rameau info
----------------------------------------------------------------------------
*/

#define MAX_ENTRY_LENGTH 40

int
GetInfo (FILE * stream)
{
   int     line = 1, end_loop = 0, str_pos;
   char    buffer[TMP_BUFFER_LENGTH];
   char    str[TMP_BUFFER_LENGTH], entry[MAX_ENTRY_LENGTH];

   if (stream == NULL)
   {
      return -1;		/* no input file */
   }

   /* move to first file position */
   fseek (stream, 0L, SEEK_SET);

   do
   {				/* read info entry and info string */

      if (fgets (buffer, TMP_BUFFER_LENGTH, stream) == NULL)
      {
	 end_loop = 1;
	 break;			/* edn of while loop */
      }
      str_pos = strlen (buffer);/* clear new line character */
      switch (buffer[str_pos])
      {
      case '\n':
      case '\r':
	 buffer[str_pos - 1] = 0;

      }
      if (buffer[0] != '#')
      {
	 strcpy (entry, "INVALID");
      }
      else
      {
	 str_pos = 1;
	 while (str_pos < MAX_ENTRY_LENGTH - 1 && buffer[str_pos] != '#')
	 {
	    str_pos++;
	 }
	 str_pos++;
	 if (str_pos < MAX_ENTRY_LENGTH - 1)
	 {
	    strncpy (entry, buffer, str_pos);
	    entry[str_pos] = 0;
	    strncpy (str, buffer + str_pos + 1, TMP_BUFFER_LENGTH - 1);
	    str[TMP_BUFFER_LENGTH - 1] = 0;
	 }
	 else
	    strcpy (entry, "INVALID");
      }

      /* set info fields */
      if (0 == strcmp (entry, "#AnalysisNo#"))
      {
	 strncpy (PsInfoTxt[AnalysisNo].text, str, PsInfoTxt[AnalysisNo].input_size);
      }
      else if (0 == strcmp (entry, "#Date#"))
      {
	 strncpy (PsInfoTxt[Date].text, str, PsInfoTxt[Date].input_size);
      }
      else if (0 == strcmp (entry, "#Title#"))
      {
	 strncpy (PsInfoTxt[Title].text, str, PsInfoTxt[Title].input_size);
      }
      else if (0 == strcmp (entry, "#Composer#"))
      {
	 strncpy (PsInfoTxt[Composer].text, str, PsInfoTxt[Composer].input_size);
      }
      else if (0 == strcmp (entry, "#Opus#"))
      {
	 strncpy (PsInfoTxt[Opus].text, str, PsInfoTxt[Opus].input_size);
      }
      else if (0 == strcmp (entry, "#Year#"))
      {
	 strncpy (PsInfoTxt[Year].text, str, PsInfoTxt[Year].input_size);
      }
      else if (0 == strcmp (entry, "#Editor#"))
      {
	 strncpy (PsInfoTxt[Editor].text, str, PsInfoTxt[Editor].input_size);
      }
      else if (0 == strcmp (entry, "#Publisher#"))
      {
	 strncpy (PsInfoTxt[Publisher].text, str, PsInfoTxt[Publisher].input_size);
      }
      else if (0 == strcmp (entry, "#Movement#"))
      {
	 strncpy (PsInfoTxt[Movement].text, str, PsInfoTxt[Movement].input_size);
      }
      else if (0 == strcmp (entry, "#Key#"))
      {
	 strncpy (PsInfoTxt[Key].text, str, PsInfoTxt[Key].input_size);
      }
      else if (0 == strcmp (entry, "#TimeSignature#"))
      {
	 strncpy (PsInfoTxt[TimeSignature].text, str, PsInfoTxt[TimeSignature].input_size);
      }
      else if (0 == strcmp (entry, "#BarNodsfrom#"))
      {
	 strncpy (PsInfoTxt[BarNodsfrom].text, str, PsInfoTxt[BarNodsfrom].input_size);
      }
      else if (0 == strcmp (entry, "#BarNodsto#"))
      {
	 strncpy (PsInfoTxt[BarNodsto].text, str, PsInfoTxt[BarNodsto].input_size);
      }
      else if (0 == strcmp (entry, "#OffbeatValue#"))
      {
	 strncpy (PsInfoTxt[OffbeatValue].text, str, PsInfoTxt[OffbeatValue].input_size);
      }
      else if (0 == strcmp (entry, "#MinimalRhythmValue#"))
      {
	 strncpy (PsInfoTxt[MinimalRhythmValue].text, str, PsInfoTxt[MinimalRhythmValue].input_size);
      }
      else if (0 == strcmp (entry, "#InputBy#"))
      {
	 strncpy (PsInfoTxt[InputBy].text, str, PsInfoTxt[InputBy].input_size);
      }
      else
      {
	 sprintf (buffer, "!!! Parameter error in info file line #%d!!!", line);
	 disp_error_message (buffer);
      }

      line++;
   }
   while (!end_loop);

   return (0);
}


/*----------------------------------------------------------------------
    disp no error massage
------------------------------------------------------------------------
*/
int
disp_error_message (char *str)
{
   return 0;
}
