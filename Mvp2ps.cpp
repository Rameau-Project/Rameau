/*-------------------------------------------------------------------
     Dieter Neubacher     Vers.: 1.0                    10.07.93
	                      Vers.: 2.0                    28.01.97  
     -------------------------------------------------------------
     Mvp2ps.cpp

	 Vers.: 2.0    WIN32, Ghostview
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "global.h"
#include "rameau.h"
#include "version.h"
#include "MvpPs.h"
#include "MvpEntry.h"
#include "mvp.h"
#include "info-ps.h"


#define MAX_CHANNEL 16


/*------------------*/
/* local functions  */
/*------------------*/

void mvp_to_mps_usage (void);
int  main (int argc, char *argv[]);


/*------------------*/
/* global variables */
/*------------------*/

char PROG_NAME[] = "mvp2ps";

int TmpInt;

struct
{
FILE *ErrStream;
FILE *InStream;
FILE *OutStream;
FILE *InfoStream;

int  *UseChannels;

char InFileName[MAX_PATH_LENGTH];
char OutFileName[MAX_PATH_LENGTH];

int  VoiceCount;
int  ColsPerPage;

int    VerboseFlag;
int    ReadLineFlag;
int    ColorOutputFlag;
} Mvp2Ps =
{
  stderr,
  stdin,
  stdout,
  NULL,

  NULL,

  "",
  "",

  0,
  22,

  0, // Falgs
  0,
  0
};

Mvp      mvp;
MvpPs    mps;
MvpEntry mvpEntry;

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void mvp_to_mps_usage ()
{
char str[10];

	str[0] = 0;
	SetRameauExt( str, MVP );

	fprintf (Mvp2Ps.ErrStream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, str);
	fprintf (Mvp2Ps.ErrStream, "\n");
	fprintf (Mvp2Ps.ErrStream, "flags : -?   this output\n");
	fprintf (Mvp2Ps.ErrStream, "      : -h   this output\n");
	fprintf (Mvp2Ps.ErrStream, "      : -v   disp program version\n");
	fprintf (Mvp2Ps.ErrStream, "      : -l   Disp Read line numbers\n");
	fprintf (Mvp2Ps.ErrStream, "      : -ch= chanels\n");
	fprintf (Mvp2Ps.ErrStream, "      : -c=  cols per page\n");
	fprintf (Mvp2Ps.ErrStream, "      : -C=  color output\n");
	fprintf (Mvp2Ps.ErrStream, "      : -X=  Page X Size\n");
	fprintf (Mvp2Ps.ErrStream, "      : -Y=  Page Y Size\n");
	fprintf (Mvp2Ps.ErrStream, "      : -xo=n x offset\n");
	fprintf (Mvp2Ps.ErrStream, "      : -xs=n x scale\n");
	fprintf (Mvp2Ps.ErrStream, "      : -yo=n y offset\n");
	fprintf (Mvp2Ps.ErrStream, "      : -ys=n y scale\n");
    fprintf (Mvp2Ps.ErrStream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
int     i;
long    time_stamp;
Mvp     mvp;
long    LineReadCounter = 0L;
enum PaperType PageFormat;
enum PaperOrientation PageOrientation;
 
	// Set Default Parameter

	PageOrientation = Landscape;   
	PageFormat      = A5;
    PsPageData.SetPaperType( PageFormat, PageOrientation );
   
   
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
            mvp_to_mps_usage ();
            return (0);
            break;
         case 'v':
            rameau_version (Mvp2Ps.ErrStream);
            return (0);
            break;
         case 'A':
                    if (argv[i][2] == '3' ) PageFormat = A3;
                    if (argv[i][2] == '4' ) PageFormat = A4;
                    if (argv[i][2] == '5' ) PageFormat = A5;
            break;
         case 'L':
                    PageOrientation = Landscape;
            break;
         case 'P':
                    PageOrientation = Portrait;
            break;

         case 'e':
            {
               if ((Mvp2Ps.ErrStream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
               {
                  Mvp2Ps.ErrStream = stderr;
                  fprintf (Mvp2Ps.ErrStream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
               }
            }
            break;
         case 'C':
            Mvp2Ps.ColorOutputFlag = 1;
            break;
         case 'c':
                    if (argv[i][2] == 'h' && argv[i][3] == '=')
                    {
                       if( (Mvp2Ps.UseChannels = GetChParam( argv[i] + 4, MAX_CHANNEL )) == NULL )
                       {
                          mvp_to_mps_usage ();
                          return (0);
                       }
                    }
                    else
                    if (argv[i][2] == '=' )
                    {
                       if( sscanf( argv[i] + 3, "%d", &(Mvp2Ps.ColsPerPage)) != 1 )
                       {
                          mvp_to_mps_usage ();
                          return (0);
                       }
                    }
                    else
                    {
                       mvp_to_mps_usage ();
                       return (0);
                    }
                    break;

         case 'X':
                    if (argv[i][2] == '=')
                    {
                       if( sscanf( argv[i]+3, "%d", &TmpInt) != 1 )
                       {
                          mvp_to_mps_usage ();
                          return (0);
                       }
					   PsPageData.Page.SetSizeX( TmpInt );
                    }
                    else
                    {
                       mvp_to_mps_usage ();
                       return (0);
                    }
                    break;
         case 'Y':
                    if (argv[i][2] == '=')
                    {
                       if( sscanf( argv[i]+3, "%d", &TmpInt) != 1 )
                       {
                          mvp_to_mps_usage ();
                          return (0);
                       }
					   PsPageData.Page.SetSizeY( TmpInt );
                    }
                    else
                    {
                       mvp_to_mps_usage ();
                       return (0);
                    }
                    break;

         case 'x':
            if (argv[i][2] == 'o' && argv[i][3] == '=')
            {
               PsPageData.Page.SetTransX(atoi (argv[i] + 4));
            }
            else if (argv[i][2] == 's' && argv[i][3] == '=')
            {
               PsPageData.Page.SetScaleX((double)(atoi (argv[i] + 4)) / 100.0);
            }
            else
               mvp_to_mps_usage ();
            break;

         case 'y':
            if (argv[i][2] == 'o' && argv[i][3] == '=')
            {
               PsPageData.Page.SetTransY(atoi (argv[i] + 4));
            }
            else if (argv[i][2] == 's' && argv[i][3] == '=')
            {
               PsPageData.Page.SetScaleY((double)(atoi (argv[i] + 4)) / 100.0);
            }
            else
               mvp_to_mps_usage ();
            break;
         case 'l':
                    Mvp2Ps.ReadLineFlag = 1;
                    break;
         default:
            mvp_to_mps_usage ();
            return (0);
            break;
         }
      }
      else
         /* input/output file */
      {
         char    buf[_MAX_PATH];

         /* first parameter is input file */

         if( Mvp2Ps.InFileName[0] == 0 )
         {
            strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
            strcat (buf, ".");
			SetRameauExt( buf, MVP );
#endif /*  */
            strcpy( Mvp2Ps.InFileName, buf );

            strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
            strcat (buf, ".");
			SetRameauExt( buf, MPS );
#endif /*  */
            strcpy ( Mvp2Ps.OutFileName, buf );
            
            strcpy (buf, argv[i]);
            strcat (buf, ".inf");
            
            Mvp2Ps.InfoStream = fopen(buf, "r");

            if(Mvp2Ps.InfoStream == NULL)
            {
               fprintf (Mvp2Ps.ErrStream, "!!! File %s open error !!!\n", buf);
            }

         }
         else
         {
            mvp_to_mps_usage ();
            return (3);
         }
      }
   }

   if( Mvp2Ps.InFileName[0] == 0 )
   {
      mvp_to_mps_usage ();
      return (4);
   }

   /*----------------------------------------------------------
     main loop
   ------------------------------------------------------------
   */


   if( Mvp2Ps.VerboseFlag ) Mvp2Ps.ReadLineFlag = 0;


   Mvp2Ps.InStream  = mvp.OpenInFile ( Mvp2Ps.InFileName );
   Mvp2Ps.OutStream = mps.OpenOutFile( Mvp2Ps.OutFileName );
   mvp.SetOutStream( NULL );
   mvp.SetMsgStream( NULL );
   mvp.SetErrStream( Mvp2Ps.ErrStream );
   mps.SetErrStream( Mvp2Ps.ErrStream );
   mps.SetMsgStream( NULL );
   
   if( Mvp2Ps.InStream == NULL || Mvp2Ps.OutStream == NULL )
   {
      exit( 5 );
   }

   // set channel flags

   for( i=0, Mvp2Ps.VoiceCount = 0; i<MAX_CHANNEL; i++ )
   {
      if( Mvp2Ps.UseChannels == NULL )
      {
         mvp.SetChFlag(i);
         // fprintf( stderr, "use channel %d\n",i );
      }
      else
      if( Mvp2Ps.UseChannels[i] != -1 )
      {
         mvp.SetChFlag( Mvp2Ps.UseChannels[i] );
         Mvp2Ps.VoiceCount++;
         // fprintf( stderr, "use channel %d\n",UseChannels[i] );
      }
   }

   // Ps output

   PsPageData.SetPaperType( PageFormat, PageOrientation );
   mps.SetPageSize( PsPageData.TotalSizeX(), PsPageData.TotalSizeY() );
   mps.SetVoicesPerBlock( Mvp2Ps.VoiceCount );
   
   mvp.MsgOff();
   mvp.TxtOff();

   mps.PsOutputBegin( Mvp2Ps.ColorOutputFlag, Mvp2Ps.InfoStream );


   while ((time_stamp = mvp.ReadMvpLine ()) != -1L)
   {
        // fprintf( stderr, "read mvp line % 8ld\n", time_stamp );
 
        if( Mvp2Ps.ReadLineFlag )
        {
           fprintf( stdout, "\r%8ld", LineReadCounter++ );
        }
        if( Mvp2Ps.VerboseFlag )
        {
           fprintf( stdout, "\rLine: %8ld Time: %8ld Bar: %8d Mvp2Mps ", LineReadCounter++, time_stamp, mvp.GetBar() );
        }
        mps.SetBar( mvp.GetBar() );
        mps.NewTime();
        for( i=0; i < mps.GetLinesPerBlock(); i++ )
        {
           mvpEntry = mvp.GetEntry( i, 0 );

           if( mvpEntry.GetStatus() != MVP_ERROR )
           {
              mps.SetMvpEntry( i, &mvpEntry );
           }

           // mvpEntry.Write( stderr );
           // fprintf( stderr, "time %ld line %d\n", time_stamp, i );
        }
        mps.NextPos();
   }

   mps.PsOutputEnd();
   return 0;



   /*----------------------------------------------------------
   main loop end
   ------------------------------------------------------------
   */
   if (Mvp2Ps.InStream != stdin)
      fclose (Mvp2Ps.InStream);
   if (Mvp2Ps.OutStream != stdout)
      fclose (Mvp2Ps.OutStream);
   if (Mvp2Ps.ErrStream != stderr)
      fclose (Mvp2Ps.ErrStream);
   if (Mvp2Ps.InfoStream != stderr)
      fclose (Mvp2Ps.InfoStream);

   return (0);
}
