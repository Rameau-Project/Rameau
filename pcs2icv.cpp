/*-------------------------------------------------------------------
     Dieter Neubacher     Vers.: 1.0                         16.02.97
---------------------------------------------------------------------
     pcs2icv.c
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "rameau.h"
#include "global.h"
#include "version.h"
#include "pcs.h"
#include "primtab.h"


FILE *err_stream = stderr;

int    VerboseFlag  = 0;
int    ReadLineFlag = 0;


/*-----------------------
   FLAGS
-------------------------
*/
int     disp_flag = 0;               /* display primform */

/*-----------------------
   DEFINITONS AND MACROS
-------------------------
*/
#define MAX_LEVEL	8

/*-----------------------
   GLOBAL VARIABLES
-------------------------
*/

Pcs pcs;

char PROG_NAME[] =  "pcs2icv";

/*--------------------
   FUNCTION DEFINITON
----------------------
*/

void    VpsToIcvUsage (void);


/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void VpsToIcvUsage ()
{
char str[10];

   str[0] = 0;	
   SetRameauExt( str, PCS );
   
   fprintf (err_stream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, str);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags : -? this output\n");
   fprintf (err_stream, "        -h this output\n");
   fprintf (err_stream, "        -v disp program version\n");
   fprintf (err_stream, "        -d display pcs\n");
   fprintf (err_stream, "        -o output to stdout\n");
   fprintf (err_stream, "        -i input from stdin\n");
   fprintf (err_stream, "        -l Disp Read line numbers\n");
   fprintf (err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
   int     group, number;
   char    buf[MAX_PATH_LENGTH];
   int     i, bar = -1;
   FILE    *in_stream = NULL, *out_stream = NULL, *info_stream = NULL;
   long    LineReadCounter = 0L;
   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------
   */
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
         switch (argv[i][1])
         {
         case '?':
         case 'h':
         case 'H':
            VpsToIcvUsage ();
            return (0);
            break;
         case 'v':
            rameau_version ( err_stream );
            return (0);
            break;
       case 'e':
            {
               if ((err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
               {
                  err_stream = stderr;
                  fprintf (stderr, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
               }
            }
            break;
         case 'd':
            disp_flag = 1;
            break;
         case 'i':
            in_stream = stdin;
            break;
         case 'o':
            out_stream = stdout;
            break;
         case 'l':
            ReadLineFlag = 1;
            break;
         default:
            VpsToIcvUsage ();
            return (0);
            break;
         }
      }
      else
         /* input/output file */
      {
         /* first parameter is input file */

         if (in_stream == NULL)
         {
            strcpy (buf, argv[i]);
            strcat (buf, ".pcs");
            in_stream = fopen (buf, "r");

            if (in_stream == NULL)
            {
               fprintf (err_stream, "!!! File %s open error !!!\n", buf);
               return (1);
            }

            strcpy (buf, argv[i]);
            strcat (buf, ".inf");
            info_stream = fopen (buf, "r");

            if (info_stream == NULL)
            {
               fprintf (err_stream, "!!! File %s open error !!!\n", buf);
            }

            if (out_stream == NULL)
            {
               strcpy(buf, argv[i]);
			   strcat(buf, "." );	
               SetRameauExt(buf, ICV );
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
            VpsToIcvUsage ();
            return (3);
         }
      }
   }

   /*-----------------------------------------------
   if no in/out file specified use stdin/stdout
   -------------------------------------------------
   */

   if (in_stream == NULL)
   {
      in_stream = stdin;
   }

   if (out_stream == NULL)
   {
      out_stream = stdout;
   }


   /*----------------------------------------------------------
   main loop to bulid pcs primform from vps
   ------------------------------------------------------------
   */

   pcs.SetInStream( in_stream );
   pcs.SetErrStream( err_stream );
   pcs.SetOutStream( NULL );
   pcs.SetMsgStream( out_stream );
   pcs.TxtOn();
   pcs.MsgOn();
   
   while ( pcs.ReadPcsLine() != -1 )
   {
      group = pcs.GetGroup();
      number= pcs.GetNumber();

	  if (disp_flag)
      {
             fprintf( stdout, "\n%d%d", group, number );
      }
      if( ReadLineFlag )
      {
             fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if (group > MAX_LEVEL)
      {
             fprintf (err_stream, "Invalid PCS %d-%d\n", group, number);
      }
      
	  // get PCS primcode
	  i=0;	
      while( strcmp(primtab[i].prim_str, "invalid") != 0 )
	  {
	   char str[10];
	   int  icv[12], note[12], diff;
	   unsigned int code, test;

		  sprintf( str,"%d-%d", group, number ); 
		  if( strcmp( str, primtab[i].prim_str ) != 0 ) 
		  {
			  i++;
			  continue;
		  }
		  // primcode found
		  code = primtab[i].primcode;

    	  // fprintf( out_stream, "%5s %4x ", str, code );
    	  fprintf( out_stream, "%-5s ", str );
		  test =0x8000;
		  for( i=0; i<12; i++ )
		  {
 			if( code & test ) note[i] = 1; else note[i] = 0;
			// fprintf( out_stream, "%d", note[i] );
			test >>= 1;
		  }
		  // icv 
		  for( i=0; i<12; i++ ) icv[i] = 0;
		  for( diff=1; diff <= 11; diff++ )
		  {
			for( i=0; i+diff <= 11; i++ )
			{
			  if( note[i] == 1 && note[i+diff] == 1 ) icv[diff]++;
			}
		  }
		  fprintf( out_stream,"[%d %d %d %d %d %d]", 
				   icv[1]+icv[11],
				   icv[2]+icv[10],
				   icv[3]+icv[9],
				   icv[4]+icv[8],
				   icv[5]+icv[7],
				   icv[6]
				   );
		  fprintf( out_stream, "\n" );
		  break;
	  }

   }

   if (in_stream != stdin)
      fclose (in_stream);
   if (out_stream != stdout)
      fclose (out_stream);
   if (err_stream != stderr)
      fclose (err_stream);
   if (info_stream != NULL)
      fclose (info_stream);

   return (0);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
