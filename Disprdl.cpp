/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.0                     Wuhu, 22-06-94
	                         Vers.: 2.0							  29-01-97

  Version 2.0 : WIN32
  -------------------------------------------------------------------------
*/

#include <string.h>

#include "rameau.h"
#include "rdl.h"
#include "global.h"
#include "version.h"

#define PROG_NAME "dispRdl"
#define INP_FILE  "rdlFile"

#define MAX_CHANNEL 16

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
disprdl_usage ()
{
   fprintf (stderr, "usage : %s [flags] [%s]\n", PROG_NAME, INP_FILE);
   fprintf (stderr, "\n");
   fprintf (stderr, "flags : -? this output\n");
   fprintf (stderr, "      : -h this output\n");
   fprintf (stderr, "      : -ch= channel order\n");
   fprintf (stderr, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
   int     i, *UseChannels = NULL;
   FILE   *in_stream  = NULL;
   FILE   *out_stream = NULL;
   FILE   *err_stream = stderr;
   Rdl    rdl;

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
	    disprdl_usage ();
	    return (0);
	    break;
	 case 'i':
	    in_stream = stdin;
	    break;
	 case 'v':
	    rameau_version (err_stream);
	    return (0);
	    break;
	 case 'c':
		    if (argv[i][2] == 'h' && argv[i][3] == '=')
		    {
		       if( (UseChannels = GetChParam( argv[i] + 4, MAX_CHANNEL )) == NULL )
		       {
			  disprdl_usage ();
			  return (0);
		       }
		    }
		    else
		    {
		       disprdl_usage ();
		       return (0);
		    }
		    break;

	 default:
	    disprdl_usage ();
	    return (0);
	    break;
	 }
      }
      else
      /* first parameter is input file */
      {
	 char    buf[_MAX_PATH];

	 strcpy (buf, argv[i]);

	 rdl.OpenInFile(buf);

	 if( UseChannels == NULL )
	 {
	    rdl.Disp();                // no channel order
	 }
	 else
	 {
	    rdl.Disp( UseChannels );   // new channel order
	 }
	 return 0;
      }
   }

   return 0;
}
