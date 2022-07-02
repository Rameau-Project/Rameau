/*---------------------------------------------------------------------
Mittlere ton hoehe  AverageToneValue
-----------------------------------------------------------------------
    rdl2atv.cpp
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "rameau.h"
#include "global.h"
#include "version.h"
#include "rdl.h"
#include "atv.h"
#include "ChNotes.h"


#define PROG_NAME   "rdl2avt"

#define AVR_OUT_FILE_WRITE_ERROR -1

void  RdlToAvtUsage (void);

/*------------------*/
/* global variables */
/*------------------*/

ChNotes Notes;

FILE   *in_stream = NULL, *out_stream = NULL, *err_stream= stderr;
int    flag_all_notes = 0;  /* print all notes */
int    VerboseFlag  = 0;
int    ReadLineFlag = 0;
//----------------------------------
//  error handler for avt
//----------------------------------

void Error(int errNum )
{
   switch( errNum )
   {
     case AVR_OUT_FILE_WRITE_ERROR :
          fprintf( err_stream, "Exit ATV while OutFile write error %d!\n", errNum );
          exit( errNum );
          break;

     default :
          fprintf( err_stream, "Exit AVT while error %d!\n", errNum );
          exit( errNum );
          break;
   }
}
/*------------------------------------
--------------------------------------
*/

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
RdlToAtvUsage ()
{
char str[10];

   SetRameauExt( str, RDL );
   
   fprintf (err_stream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, str);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags : -? this output\n");
   fprintf (err_stream, "      : -h this output\n");
   fprintf (err_stream, "      : -v disp program version\n");
   fprintf (err_stream, "      : -o output to stdout\n");
   fprintf (err_stream, "      : -i input from stdin\n");
   fprintf (err_stream, "      : -a print all nodes\n");
   fprintf (err_stream, "      : -V verbose\n");
   fprintf (err_stream, "      : -l Disp Read line numbers\n");
   fprintf (err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
   int     i;
   int     channel_nodes[16];
   long    time_stamp, last_time = -1;

   Rdl     rdl;
   Atv	   atv;
   
   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
     switch (argv[i][1])
     {
     case '?':
     case 'h':
     case 'H':
        RdlToAtvUsage ();
        return (0);
        break;
     case 'v':
        rameau_version (err_stream);
        return (0);
        break;
     case 'e':
        {
           if ((err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
           {
              err_stream = stderr;
              Notes.SetErrStream( stderr );
              fprintf (err_stream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
           }
           Notes.SetErrStream( err_stream );
        }
        break;
     case 'i':
        in_stream = stdin;
        break;
     case 'o':
        out_stream = stdout;
        break;
     case 'a':
        flag_all_notes = 1;
        break;
     case 'V':
        VerboseFlag = 1;
        break;

     case 'l':
        ReadLineFlag = 1;
        break;

     default:
        RdlToAtvUsage ();
        return (0);
        break;
     }
      }
      else

      {  
      /*-------------------*/
      /* input/output file */
      /*-------------------*/
      char    buf[MAX_PATH_LENGTH];
      /*-------------------------------*/
      /* first parameter is input file */
      /*-------------------------------*/
      if (in_stream == NULL)
      {
        strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
        strcat (buf, ".");
		SetRameauExt( buf, RDL );
#endif /*  */

        if ((in_stream = rdl.OpenInFile (buf)) == NULL )
        {
           return (1);
        }

     /*--------------------------------*/
     /* Second parameter is input file */
     /*--------------------------------*/
        if (out_stream == NULL)
        {
           strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
           strcat (buf, ".");
		   SetRameauExt( buf, ATV);
#endif /*  */       
           remove( buf );
           out_stream = fopen (buf, "w");

           if (out_stream == NULL)
           {
              fprintf (err_stream, "!!! File %s open error !!!\n", buf);
              return (2);
           }
           SetFileInfoLine( out_stream, buf );
        }
     }
     else
     {
        RdlToAtvUsage ();
        return (3);
     }
      }
   }

   /*-----------------------------------------------
   if no in/out file specified use stdin/stdout
   -------------------------------------------------*/
   if (in_stream == NULL)
   {
      in_stream = stdin;
   }

   if (out_stream == NULL)
   {
      out_stream = stdout;
   }


   /*----------------------------------------------------------
   main loop
   ------------------------------------------------------------*/
   {
   long LineReadCounter = 0L;

   if( VerboseFlag ) ReadLineFlag = 0;

   rdl.SetOutStream( NULL );
   rdl.SetErrStream( err_stream );
   rdl.SetMsgStream( out_stream );
   rdl.MsgOn();
   rdl.TxtOn();

   atv.SetInStream( NULL );
   atv.SetOutStream( out_stream );
   atv.SetErrStream( err_stream );
   atv.SetMsgStream( out_stream );
   atv.MsgOff();
   atv.TxtOff();

   while ((time_stamp = rdl.Read ( channel_nodes)) != -1L)
   {                 
   
      if( ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if( VerboseFlag )
      {
         fprintf( stdout, "\rTime: %8ld Bar: %8d ", time_stamp, rdl.GetBar() );
      }
      
	  atv.SetBar( rdl.GetBar() );
      if (last_time < time_stamp)
      {

		 atv.WriteAtvLine( time_stamp, Notes );
         atv.NewTime();
         last_time = time_stamp;
      }
      Notes.SetChNotes( channel_nodes );
   }
   atv.WriteAtvLine( last_time, Notes );
   }
   /*----------------------------------------------------------
   main loop end
   ------------------------------------------------------------*/

   if (in_stream != stdin)
      fclose (in_stream);
   if (out_stream != stdout)
      fclose (out_stream);
   if (err_stream != stderr)
      fclose (err_stream);

   return (0);
}

