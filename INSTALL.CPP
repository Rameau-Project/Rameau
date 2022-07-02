/*----------------------------------------------------------------------
    Dieter Neubacher		Vers. 1.0		30.12.93
    --------------------------------------------------------------------
    install.c
    install functions for rameau system
------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <process.h>
#include <ctype.h>


#include <direct.h>
#include <dos.h>
#include <bios.h>
#include <conio.h>


#define INP_EXT	     "xxx"
#define OUT_EXT      "yyy"
#define PROG_NAME    "install"
#define GS_FILE_NAME "gs252dos.exe"


void    install_usage (void);
void	install_version (void);




/* #define INSTALL_SOURCE
*/
/* #define INSTALL_EXEC_MENU
*/
#define INSTALL_DATA

/* #define DEBUG_SYSTEM_CALL
*/

FILE   *config;
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
install_version ()
{

   fprintf (stderr, "\n   R  A  M  E  A  U");
   fprintf (stderr, "\n");
   fprintf (stderr, "\nMusic Analysis Software");
   fprintf (stderr, "\n");
   fprintf (stderr, "\n");
   fprintf (stderr, "\n(c) 1994 by ARION Verlag, Muenchen");
   fprintf (stderr, "\n");

   return;
}



/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
install_usage ()
{

   fprintf (stderr, "\nusage : %s [flags] [%s_file_name]", PROG_NAME, INP_EXT);
   fprintf (stderr, "\n");

   fprintf (stderr, "\nflags : -? this output");

   fprintf (stderr, "\n      : -h this output");

   fprintf (stderr, "\n      : -v disp program version");

   fprintf (stderr, "\n");

   return;

}



/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
int
do_system_call (char *str)
{

#ifdef DEBUG_SYSTEM_CALL
   fprintf (stdout, "\n%s", str);

   if (getch () != '\r')
      return (0);

#endif /* DEBUG_SYSTEM_CALL */
   if (system (str) != 0)

   {

      printf ("\n!!! ERROR WHILE PROCESSING %s !!!", str);

      printf ("\n!!! EXIT INSTALL PROCCESS !!!");

      exit (-1);

   }

   return (0);

}




/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int
main (int argc, char *argv[])
{

   int     ch1, ch2;

   int     i, error_flag;

   unsigned install_from_drive, install_to_drive;	/* install rameau drive */

   struct _diskfree_t drvinfo;

   char    path[100];

   char    drive[10];

   char    null_file[100];

   char    buffer[100];


   /*----------------------------------------------------------
     programm flags setting und validation
     ------------------------------------------------------------
                 */
   for (i = 1; i < argc; i++)

   {

      if (argv[i][0] == '-')	/* program flags */

      {

	 switch (argv[i][1])

	 {

	 case '?':

	 case 'h':

	 case 'H':

	    install_usage ();

	    return (0);

	    break;

	 case 'v':
	    install_version ();

	    return (0);

	    break;

	 default:

	    install_usage ();

	    return (0);

	    break;

	 }

      }

   }

   install_from_drive = _getdrive ();

   /* GET AND CHANGE TO RAMEAU DRIVE */
   do

   {

      long    mem_size = 0L;

      error_flag = 0;

      fprintf (stdout, "\nInstall RAMEAU on drive [C,D,E,...] : ");

      fgets (drive, 10, stdin);

      fprintf (stdout, "\n");

      install_to_drive = toupper (drive[0]) - 'A' + 1;

      if (_chdrive (install_to_drive) != 0)

      {

	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE %s\n", drive);

	 error_flag = 1;

      }

      _dos_getdiskfree (install_to_drive, &drvinfo);

      mem_size = (long) drvinfo.avail_clusters * drvinfo.sectors_per_cluster * drvinfo.bytes_per_sector;


      fprintf (stderr, "\nDisk %c space free : \t%ld byte\n",
	       (char) install_to_drive - 1 + 'A', mem_size
	 );

      if (mem_size < 5000000L)

      {

	 fprintf (stderr, "\nInsufficient space on drive %c:", (char) ('A' - 1 + install_to_drive));

	 fprintf (stderr, "\nInstallation of RAMEAU aborted");

	 exit (1);

      }

   }
   while (error_flag);


   /* CREATE RAMEAU DIRECTORIES */
   error_flag = 0;

   printf ("\ncreate rameau directories\n\n");

   strcpy (path, "\\RAMEAU");

   strcpy (buffer, path);

   if (_mkdir (buffer) != 0)

      error_flag = 1;

   else
   {

      sprintf (buffer, "%s\\BIN", path);

      if (_mkdir (buffer) != 0)

	 error_flag = 1;

      else
      {

	 sprintf (buffer, "%s\\HLP", path);

	 if (_mkdir (buffer) != 0)

	    error_flag = 1;

	 else
	 {

	    sprintf (buffer, "%s\\TMP", path);

	    if (_mkdir (buffer) != 0)

	       error_flag = 1;

	    else
	    {

	       sprintf (buffer, "%s\\DATA", path);

	       if (_mkdir (buffer) != 0)

		  error_flag = 1;

	    }

	 }

      }

   }

   if (error_flag != 0)

   {

      fprintf (stderr, "\nRAMEAU INSTALL ERROR : CAN'T CREATE DIRECTORY %s", buffer);

      fprintf (stderr, "\nInstallation aborted");

      exit (1);

   }

   sprintf (path, "%c:\\RAMEAU", (char) ('A' - 1 + install_to_drive));

   sprintf (null_file, "%c:\\RAMEAU\\tmp\\null", (char) ('A' - 1 + install_to_drive));

   /* INSTALL RAMEAU FILES */

   printf ("\nInstalling program files ...\n");

   if (_chdrive (install_from_drive) != 0)

   {

      fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE\n");

      fprintf (stderr, "\nInstallation aborted");

      exit (1);

   }

   sprintf (buffer, "copy menu.exe %s\\rameau.exe > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy rameau.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy mid2rdl.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy rdl2vps.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy vps2pcs.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy pcs2map.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy rdl2mvp.exe %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy mvp2ps.exe  %s\\bin > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy DispRdl.exe %s > %s ", path, null_file);
   do_system_call (buffer);

   sprintf (buffer, "copy rameau.cfg %s > %s ", path, null_file);
   do_system_call (buffer);


   printf ("\nInstalling help files ...\n");

   sprintf (buffer, "copy *.hlp %s\\hlp > %s ", path, null_file);
   do_system_call (buffer);


   /*---------------------*/
   /* install data files  */
   /*---------------------*/
#ifdef INSTALL_DATA
   printf ("\nInstalling data files ...\n");

   sprintf (buffer, "copy *.mid %s\\data > %s ", path, null_file);

   do_system_call (buffer);

   sprintf (buffer, "copy *.inf %s\\data > %s ", path, null_file);

   do_system_call (buffer);

#endif /* INSTALL_DATA */
   /*---------------------*/
   /* install source code */
   /*---------------------*/
#ifdef INSTALL_SOURCE
   printf ("\nInstalling source files ...\n");

   sprintf (buffer, "mkdir %s\\src > %s ", path, null_file);

   do_system_call (buffer);

   sprintf (buffer, "copy *.c %s\\src > %s ", path, null_file);

   do_system_call (buffer);

   sprintf (buffer, "copy *.h %s\\src > %s ", path, null_file);

   do_system_call (buffer);

   sprintf (buffer, "copy *.mak %s\\src > %s ", path, null_file);

   do_system_call (buffer);

   sprintf (buffer, "copy *.bat %s\\src > %s ", path, null_file);

   do_system_call (buffer);

#endif /* INSTALL_SOURCE */
   /*---------------------*/
   /* get User name	    */
   /*---------------------*/
   while (kbhit ())
      fgetc (stdin);

   sprintf (buffer, "copy rameau.cfg %s > %s ", path, null_file);

   do_system_call (buffer);

   {

      char    str[41];
      buffer[80];

      fprintf (stdout, "\nEnter User/Company name (max. 40 characters): \n");

      fgets (str, 40, stdin);

      sprintf (buffer, "%s\\rameau.cfg", path);

      config = fopen (buffer, "a+");

      fprintf (config, "%s", str);

      fprintf (stdout, "\n");

      if (config != NULL)
	 fclose (config);

   }

   /*---------------------*/
   /* install Ghostscript */
   /*---------------------*/

   while (kbhit ())
      fgetc (stdin);

   /* compiler ERROR */
   fprintf (stdout, "\nBuild RAMEAU config file ...\n\nPlease hit any key to continue");

   while (!kbhit ());
   while (kbhit ())
      fgetc (stdin);

   /* compiler ERROR */
   fprintf (stdout, "\nInstall Ghostscript [Y/N]? : ");

   ch1 = fgetc (stdin);

   if (ch1 == 'y' || ch1 == 'Y')

   {

      FILE   *gs_file;

      char    buffer[100];

     do
     {
      /* Installing GS */
      fprintf (stdout, "\nInsert disk 2 in drive %c:",
	       (char) install_from_drive - 1 + 'A');

      fprintf (stdout, "\n\nPlease hit any key to continue");

      /* compiler ERROR */
      while (!kbhit ());
      while (kbhit ())
	 fgetc (stdin);

      if (_chdrive (install_from_drive) != 0)
      {
	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE %s\n", drive);
	 fprintf (stderr, "\nInstallation aborted");

	 exit (1);
      }

     }while( (gs_file = fopen (GS_FILE_NAME, "r") ) == NULL);
     fclose( gs_file );

      /* compiler ERROR */
      fprintf (stdout, "\nInstalling Ghostscript files ...");

      if (_chdrive (install_from_drive) != 0)
      {
	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE %s\n", drive);
	 fprintf (stderr, "\nInstallation aborted");

	 exit (1);
      }


      /* file on disk ? */
      if ((gs_file = fopen (GS_FILE_NAME, "r")) == NULL)

      {

	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : CAN'T OPEN FILE %s\n", GS_FILE_NAME);

	 fprintf (stderr, "\nInstallation aborted");

	 exit (1);

      }

      else
      {

	 fclose (gs_file);

      }

      /* if file on destination disk ? */
      sprintf (buffer, "%c:%s",
	       (char) install_to_drive - 1 + 'A',
	       GS_FILE_NAME
	 );

      if ((gs_file = fopen (buffer, "r")) != NULL)

      {

	 /* compiler ERROR */
	 while (!kbhit ());
	 while (kbhit ())
	    fgetc (stdin);

	 /* compiler ERROR */
	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : OVERWRITE FILE %c:%s [Y/N]? :",
		  (char) install_to_drive - 1 + 'A',
		  GS_FILE_NAME
	    );

	 ch1 = fgetc (stdin);

	 if (ch1 == 'y' || ch1 == 'Y')

	 {

	    fclose (gs_file);

	 }

	 else
	 {

	    fclose (gs_file);

	    fprintf (stderr, "\nInstallation aborted");

	    exit (1);

	 }

      }

      else
      {

	 fclose (gs_file);

      }

      sprintf (buffer, "copy %c:%s %c:%s",
	       (char) install_from_drive - 1 + 'A',
	       GS_FILE_NAME,
	       (char) install_to_drive - 1 + 'A',
	       GS_FILE_NAME
	 );

      do_system_call (buffer);

      if (_chdrive (install_to_drive) != 0)

      {

	 fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE %s\n", drive);

	 fprintf (stderr, "\nInstallation aborted");

	 exit (1);

      }

      sprintf (buffer, "%s -y", GS_FILE_NAME);

      do_system_call (buffer);

      sprintf (buffer, "erase %s > %s", GS_FILE_NAME, null_file);

      do_system_call (buffer);

   }

   /*---------------------*/
   /* install exec menu   */
   /*---------------------*/
#ifdef INSTALL_EXEC_MENU
   if (_chdrive (install_to_drive) != 0)

   {

      fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DRIVE %s\n", drive);

      fprintf (stderr, "\nInstallation aborted");

      exit (1);

   }

   if (_chdir ("RAMEAU") != 0)

   {

      fprintf (stderr, "\nRAMEAU INSTALL ERROR : INVALID DIRECTORY \\RAMEAU\n");

      fprintf (stderr, "\nInstallation aborted");

      exit (1);

   }

   /* set screen mode */
   sprintf (buffer, "mode con: lines=25");

   do_system_call (buffer);


   /*---------------------*/
   /*    exec menu	    */
   /*---------------------*/
   do_system_call ("rameau");

#endif /* INSTALL_EXEC_MENU */
   return (0);

}
