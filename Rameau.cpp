/*-------------------------------------------------------------------
     Dieter Neubacher     Vers.: 1.0            25.03.93
                                 1.1            24.09.94
								 2.0            27.01.97 
     -------------------------------------------------------------
    rameau.c    Shell for programs of rameau music analysis

  Vers.: 2.0    WIN32
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <errno.h>

#include "rameau.h"
#include "version.h"
#include "global.h"
#include "chnotes.h"           // MAX_CHANNEL
#include "info-ps.h"

#define PROG_NAME "rameau"


/* #define DEBUG_PROG_FLAGS
   #define DEBUG_MEM_INFO
*/
/*--------------------*/
/* destination output */
/*--------------------*/


int     dest_output = 0;

/*---------------------------------------------------------------------
    function declerations
-----------------------------------------------------------------------
*/


void    rameau_usage (void);
int     SystemCall(char *);
int     massage (FILE * stream, char *str, int flag);
int     clear_massage (int flag);


/*-----------------------*/
/* GLOBAL FLAGs AND DATA */
/*-----------------------*/

int     menu_flag = 1;
int     disp_flag = 0;

int     x_offset = 0, y_offset = 0;
int     x_scale  = 100, y_scale = 100;

int     disp_level = 3;
int     notes_from = 0;
int     notes_to = 9999;
int     bar_time_n = 4;
int     bar_time_z = 4;
int     off_beat_n = 4;         /* off beat time */
int     off_beat_z = 0;
int     voices     = 4;
int     ColorPrinterFlag = 0;    // 0 black/white  1 Color 

char    in_file_name[MAX_PATH_LENGTH] = "";

FILE    *err_stream = stderr;

int     *UseChannels;

enum PaperType PageFormat = A4;
enum PaperOrientation PageOrientation = Portrait;

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int 
main (int argc, char *argv[])
{
   int     i, UsePath;
   char    com_str[MAX_COMMAND_LENGTH], file_extension[10];

   /*----------------------------------------------------------
     programm flags setting und validation
     ------------------------------------------------------------
     */
   in_file_name[0] = 0;
   DebugFlag = 0;
   
   for (i = 1; i < argc; i++)
   {
#ifdef DEBUG
      printf (">>>%s<<< argv[%d]", argv[i], i);
#endif /*  */
      if (argv[i][0] == '-')    /* program flags */
      {

     switch (argv[i][1])
     {
        /* FDA FLA */
     case 'F':
        if (strcmp ("-FDA", argv[i]) == 0)
        {
           dest_output = FDA;
        }
        else if (strcmp ("-FLA", argv[i]) == 0)
        {
           dest_output = FLA;
        }
        else
           rameau_usage ();
        break;
        /* MAP MVP MPS*/
     case 'M':
        if (strcmp ("-MAP", argv[i]) == 0)
        {
           dest_output = MAP;
        }
        else
        if (strcmp ("-MVP", argv[i]) == 0)
        {
           dest_output = MVP;
        }
        else
        if (strcmp ("-MPS", argv[i]) == 0)
        {
           dest_output = MPS;
        }
        else
           rameau_usage ();
        break;

		/* ICV */
     case 'I':
        if (strcmp ("-ICV", argv[i]) == 0)
        {
           dest_output = ICV;
        }
        else
           rameau_usage ();
        break;

        /* PCS PMX */
     case 'P':
        if (strcmp ("-PCS", argv[i]) == 0)
        {
           dest_output = PCS;
        }
        else if (strcmp ("-PMX", argv[i]) == 0)
        {
           dest_output = PMX;
        }
        else
           rameau_usage ();
        break;

        /* VPS      */
     case 'V':
        if (strcmp ("-VPS", argv[i]) == 0)
        {
           dest_output = VPS;
        }
        else
           rameau_usage ();
        break;
        /* RDL RDA RLA */
     case 'R':
        if (strcmp ("-RDL", argv[i]) == 0)
        {
           dest_output = RDL;
        }
        else if (strcmp ("-RDA", argv[i]) == 0)
        {
           dest_output = RDA;
        }
        else if (strcmp ("-RLA", argv[i]) == 0)
        {
           dest_output = RLA;
        }
        else
           rameau_usage ();
        break;

     case '?':
     case 'h':
     case 'H':
        rameau_usage ();
        return (0);
        break;
     case 'C':
               ColorPrinterFlag  = 1;   // Color Printer
               break;
     case 'c':
           if (argv[i][2] == 'h' && argv[i][3] == '=')
           {
          if( ( UseChannels = GetChParam( argv[i] + 4, MAX_CHANNEL )) == NULL )
          {
             rameau_usage ();
             return (0);
          }
          voices = 0;
          for( int i=0; i < MAX_CHANNEL; i++ )
          {
             if( UseChannels[i] > voices )
             {
             voices = UseChannels[i];
             }
          }
           }
           else
           {
          rameau_usage ();
          return (1);
           }
           break;

     case 'v':
        rameau_version ( err_stream );
        return (0);
        break;
     case 'd':
        disp_flag = 1;
        break;
     case 'D':
        DebugFlag = 1;
        break;
     case '3':
        disp_level = 3;
        break;
     case '4':
        disp_level = 4;
        break;
     case '5':
        disp_level = 5;
        break;
     case '6':
        disp_level = 6;
        break;
     case '7':
        disp_level = 7;
        break;
     case 'A':
        if (strcmp ("-ATV", argv[i]) == 0)
        {
           dest_output = ATV;
        }
        else
               if (argv[i][2] == '3' ) PageFormat = A3;
               if (argv[i][2] == '4' ) PageFormat = A4;
               if (argv[i][2] == '5' ) PageFormat = A5;
        break;
     case 'L':
               PageOrientation = Landscape;
         break;

     case 's':              /* time signature */
        {
           if (argv[i][2] != '=')
           {
          rameau_usage ();
          return (1);
           }
           sscanf (argv[i] + 3, "%d/%d", &bar_time_z, &bar_time_n);

        }
        break;

     case 'b':              /* off beat time signature */
        {
           if (argv[i][2] != '=')
           {
          rameau_usage ();
          return (1);
           }
           sscanf (argv[i] + 3, "%d/%d", &off_beat_z, &off_beat_n);

        }
        break;

     case 'f':              /* notes from */

        if (argv[i][2] != '=')
        {
           rameau_usage ();
           return (1);
        }
        sscanf (argv[i] + 3, "%d", &notes_from);
        break;

     case 't':              /* notes to */

        if (argv[i][2] != '=')
        {
           rameau_usage ();
           return (1);
        }
        sscanf (argv[i] + 3, "%d", &notes_to);

        break;

     case 'x':
        if (argv[i][2] == 'o' && argv[i][3] == '=')
        {
           x_offset += atoi (argv[i] + 4);
        }
        else if (argv[i][2] == 's' && argv[i][3] == '=')
        {
           x_scale = atoi (argv[i] + 4);
        }
        else
           rameau_usage();
        break;

     case 'y':
        if (argv[i][2] == 'o' && argv[i][3] == '=')
        {
           y_offset += atoi (argv[i] + 4);
        }
        else if (argv[i][2] == 's' && argv[i][3] == '=')
        {
           y_scale = atoi (argv[i] + 4);
        }
        else
           rameau_usage();
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
     default:
        rameau_usage ();
        return (0);
        break;
     }
      }
      else
     /* input file */
      {
     if (in_file_name[0] == 0)
     {
        strcpy (in_file_name, argv[i]);
#ifdef DEBUG
        printf ("\n input file %s", in_file_name);
#endif /*  */
     }
     else
     {
        rameau_usage ();
        return (0);
     }
      }
   }

   UsePath = dest_output & 0xF0;

   /*-----------------------------------*/
   /* execute files                      */
   /*-----------------------------------*/

   /* get input file externsion */
   strlwr (in_file_name);       /* convert to lowercase */

   if (in_file_name[strlen (in_file_name) - 4] == '.')
   {
      strcpy (file_extension, in_file_name + strlen (in_file_name) - 3);
      in_file_name[strlen (in_file_name) - 4] = 0;
   }
   else
   {
      fprintf (err_stream, "invalid input file extension\n");
      return (0);
   }

   /* build rdl file */

   if (strcmp (file_extension, "mid") == 0 && (UsePath == 0x10 || UsePath == 0x20 || UsePath == 0x30 || UsePath == 0x40) )
   {
      massage (stdout, "     >>> build rdl file", menu_flag);
      sprintf (com_str, "mid2rdl %s-e -l -b=%d/%d -s=%d/%d -f=%d -t=%d \"%s\"",
           (DebugFlag == 0) ? "" : "-D ",
           off_beat_z,
           off_beat_n,
           bar_time_z,
           bar_time_n,
           notes_from,
           notes_to,
           in_file_name
     );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "mid2rdl execution error\n");
         return (1);
      }
      else
      {
     if (dest_output == RDL)
        return (0);
     else
        strcpy (file_extension, "rdl");
      }
   }
//--------
// PATH 10
//--------

   /* build vps file */

   if (strcmp (file_extension, "rdl") == 0 && (UsePath == 0x10  || UsePath == 0x40))
   {
      massage (stdout, "     >>> build vps file", menu_flag);
      sprintf (com_str, "rdl2vps -e -l \"%s\"", in_file_name );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "vps2pcs execution error\n");
         return (1);
      }
      else
      {
     if (dest_output == VPS)
        return (0);
     else
        strcpy (file_extension, "vps");
      }
   }

   /* build pcs file */
   if (strcmp (file_extension, "vps") == 0 && (UsePath == 0x10 || UsePath == 0x40) )
   {
      massage (stdout, "     >>> build pcs file", menu_flag);
      sprintf (com_str, "vps2pcs -e -l \"%s\"", in_file_name );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "vps2pcs execution error\n");
         return (1);
      }
      else
      {
     if (dest_output == PCS)
        return (0);
     else
        strcpy (file_extension, "pcs");
      }
   }

   /* build map file */

   if (strcmp (file_extension, "pcs") == 0 && UsePath == 0x10 )
   {
    char pf[10];
	
      switch( PageFormat )
	  {
		case A3: strcpy( pf, "-A3 " ); break;
		case A4: strcpy( pf, "-A4 " ); break;
		case A5: strcpy( pf, "-A5 " ); break;
		default :strcpy( pf, " " ); break;
	  }
	  
      massage (stdout, "     >>> build map file", menu_flag);
      sprintf (com_str, "pcs2map -e -l -C -%d -%c %s\"%s\"", 
		       disp_level,
			   (( PageOrientation == Landscape ) ? 'L' : 'P'),
			   pf,
			   in_file_name 
			  );
      
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "pcs2map execution error\n");
         return (1);
      }
      else
      {
         if (dest_output == MAP)
            return (0);
         else
            strcpy (file_extension, "map");
      }
   }

//--------
// PATH 20
//--------

   /* build MVP file */

   if (strcmp (file_extension, "rdl") == 0 && UsePath == 0x20 )
   {
      massage (stdout, "     >>> build mvp file", menu_flag);
      sprintf (com_str, "rdl2mvp %s-e -l -ch=0..%d \"%s\"",
           (DebugFlag == 0) ? "" : "-D ",
           voices,
           in_file_name
     );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "vps2mvp execution error\n");
         return (1);
      }
      else
      {
         if (dest_output == MVP)
             return (0);
         else
             strcpy (file_extension, "mvp");
      }
   }

   /* build MPS file */

   if (strcmp (file_extension, "mvp") == 0 && UsePath == 0x20 )
   {
    char pf[10];
	
      switch( PageFormat )
	  {
		case A3: strcpy( pf, "-A3 " ); break;
		case A4: strcpy( pf, "-A4 " ); break;
		case A5: strcpy( pf, "-A5 " ); break;
		default :strcpy( pf, " " ); break;
	  }
	  
      massage (stdout, "     >>> build mps file", menu_flag);
      sprintf (com_str, "mvp2ps -e -l -C -ch=0..%d -%c %s\"%s\"", 
		       voices, 
			   (( PageOrientation == Landscape ) ? 'L' : 'P'),
			   pf,
			   in_file_name 
			  );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "mvp2ps execution error\n");
         return (1);
      }
      else
      {
         if (dest_output == MPS)
            return (0);
         else
            strcpy (file_extension, "mps");
      }
   }


//--------
// PATH 30
//--------

   /* build atv file */

   if (strcmp (file_extension, "rdl") == 0 && UsePath == 0x30 )
   {
      massage (stdout, "     >>> build atv file", menu_flag);
      sprintf (com_str, "rdl2atv -e -l \"%s\"", in_file_name );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "vps2atv execution error\n");
         return (1);
      }
      else
      {
     if (dest_output == ATV)
        return (0);
     else
        strcpy (file_extension, "atv");
      }
   }
//--------
// PATH 40
//--------

   /* build icv file */

   if (strcmp (file_extension, "pcs") == 0 && UsePath == 0x40 )
   {
      massage (stdout, "     >>> build icv file", menu_flag);
      sprintf (com_str, "pcs2icv -e -l \"%s\"", in_file_name );
      if (SystemCall(com_str))
      {
         fprintf (err_stream, "vps2icv execution error\n");
         return (1);
      }
      else
      {
     if (dest_output == ICV)
        return (0);
     else
        strcpy (file_extension, "icv");
      }
   }

   
//--------
// Error
//--------

   {
   char str[10];

      SetRameauExt ( str, dest_output );

      strupr( str );
      strupr( file_extension );

      fprintf (err_stream, "Can't build %s file from %s input file\n",
               str, file_extension
          );

   }

   if (err_stream != stderr)
      fclose (err_stream);

   return (1);

}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void rameau_usage ()
{
   fprintf (err_stream, "usage : %s [flags] dest input_file \n", PROG_NAME);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags :                      dest :      \n");
   fprintf (err_stream, "\n");
   fprintf (err_stream, "-? this output               -MAP        \n");
   fprintf (err_stream, "-h this output               -PCS        \n");
   fprintf (err_stream, "-v disp program version      -VPS        \n");
   fprintf (err_stream, "-d display pcs               -RDL        \n");
   fprintf (err_stream, "-3 diagramm in 3 levels      -PMX        \n");
   fprintf (err_stream, "-4 diagramm in 4 levels      -FDA        \n");
   fprintf (err_stream, "-5 diagramm in 5 levels      -FLA        \n");
   fprintf (err_stream, "-b=x/y off beat Time         -RDA        \n");
   fprintf (err_stream, "-s=x/y Time Signature        -RLA        \n");
   fprintf (err_stream, "-f=nnn Nots form             -MVP        \n");
   fprintf (err_stream, "-t=nnn Nots to               -MPS        \n");
   fprintf (err_stream, "-xo=n x offset                           \n");
   fprintf (err_stream, "-xs=n x scale   default 100              \n");
   fprintf (err_stream, "-yo=n x offset                           \n");
   fprintf (err_stream, "-ys=n x scale   default 100              \n");
   fprintf (err_stream, "-C Color printer                         \n");
   fprintf (err_stream, "\n");

   return;
}
/*---------------------------------------------------------------------

-----------------------------------------------------------------------
*/
int SystemCall (char *str)
{
#ifdef DEBUG_PROG_FLAGS
   fprintf ( stderr, "\n%s", str);
#endif /*  */

   // write info to error stream

   fprintf (err_stream, "%s\n", str);
   fflush( err_stream );
   
   if (system (str) != 0)
   {  
      if( errno == E2BIG )       
      { 
         fprintf (err_stream, "!!! argument list exceeds 128 bytes, or environment information exceeds 32K. !!!\n", str);
      }
      if( errno == ENOENT )
      { 
         fprintf (err_stream, "!!! The command interpreter cannot be found. !!!\n", str);
      }
      if( errno == ENOEXEC )
      { 
         fprintf (err_stream, "!!! The command-interpreter file has an invalid format and is not executable.!!!\n", str);
      }  
      if( errno == ENOMEM )
      { 
         fprintf (err_stream, "!!! Not enough memory is available to execute the command. !!!\n", str);
      }                      
      else
      { 
         fprintf (err_stream, "!!! Error while execute %s !!!\n", str);
      }                      
      return 1;
   }
   else
   {  
      return (0);
   }
}
/*---------------------------------------------------------------------
   display working massage
-----------------------------------------------------------------------
*/


int massage (FILE * stream, char *str, int flag)
{
   if (flag == 0)
   {
      fprintf (stream, "%s\n", str);
   }
   else
   {
#ifdef WIN32
	   // fprintf( stdout, "\x1b[%d;%dH", 1, 25 );
	   printf( str );
#else
      _settextposition(25,1);
      _outtext (str);
#endif
   }
   return 0;
}

int clear_massage (int flag)
{

   if (flag == 1)
   {
      char    buffer[80];

      memset (buffer, ' ', 79);
      buffer[79] = 0;
#ifdef WIN32
	   // fprintf( stdout, "\x1b[%d;%dH", 1, 25 );
       printf(buffer);
	   // fprintf( stdout, "\x1b[%d;%dH", 0, 0 );
#else
      _settextposition(25,1);
      _outtext (buffer);
      _settextposition(0,0);
#endif
   }
   return 0;
}

