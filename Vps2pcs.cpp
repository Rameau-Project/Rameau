/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                        16.02.93
    Dieter Neubacher        Vers.: 1.1                        28.02.96
    Dieter Neubacher        Vers.: 2.0                        28.01.97
    -------------------------------------------------------------------
    vps2pcs.c
    Wandelt Notenmenge in ihre Primform                               
    
    Vers.: 1.1 excessive Syntax-Check
	Vers.: 2.0 WIN32
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "rameau.h"
#include "version.h"
#include "primtab.h"

#include "Vps.h"

#define INP_EXT     "vps"
#define OUT_EXT     "pcs"
#define PROG_NAME   "vps2pcs"

/*-------------------------------*/
/* ERROR MASSAGE TO ERROR STREAM */
/*-------------------------------*/

void    vps_to_pcs_usage ( void);


class PrimCode
{
public:


int WriteStr( FILE *stream );
int WriteStr( char *str    );

private:

uint  code;

};



class Vps2Pcs
{
public:
    Vps2Pcs();

uint    primcode (uint code);
int     prim_to_str (uint primcode, char *str);
int     code_to_note (uint code, char *note_str);
uint    note_to_code (char *note_str);

FILE    *in_stream;
FILE    *out_stream;
FILE    *err_stream;

int VerboseFlag;
int ReadLineFlag;


private:


};

Vps2Pcs::Vps2Pcs()
{
   in_stream  = NULL;
   out_stream = NULL;
   err_stream = stderr;

   VerboseFlag = 0;
   ReadLineFlag= 0;

}


Vps2Pcs vps2pcs;
Vps vps;

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
vps_to_pcs_usage ()
{

   fprintf (vps2pcs.err_stream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, INP_EXT);
   fprintf (vps2pcs.err_stream, "\n");
   fprintf (vps2pcs.err_stream, "flags : -? this output\n");
   fprintf (vps2pcs.err_stream, "      : -h this output\n");
   fprintf (vps2pcs.err_stream, "      : -v disp program version\n");
   fprintf (vps2pcs.err_stream, "      : -o output to stdout\n");
   fprintf (vps2pcs.err_stream, "      : -i input from stdin\n");
   fprintf (vps2pcs.err_stream, "      : -l Disp Read line numbers\n");
   fprintf (vps2pcs.err_stream, "\n");

   return;
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int
main (int argc, char *argv[])
{

   char    buf[MAX_PATH_LENGTH];
   char    str[MAX_PATH_LENGTH];
   uint    code;
   int     i;
   long    LineReadCounter = 0L;

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
        vps_to_pcs_usage ();
        return (0);
        break;
     case 'v':
        rameau_version ( vps2pcs.err_stream );
        return (0);
        break;
     case 'i':
        vps2pcs.in_stream = stdin;
        break;
     case 'o':
        vps2pcs.out_stream = stdout;
        break;
     case 'e':
        {
           if ((vps2pcs.err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
           {
          vps2pcs.err_stream = stderr;
          fprintf (vps2pcs.err_stream, "can't open ramenau log file : %s\n", RAMEAU_ERROR_FILE);
           }
        }
        break;
     case 'l':
        vps2pcs.ReadLineFlag = 1;
        break;


     default:

        vps_to_pcs_usage ();
        return (0);
        break;
     }
      }
      else
     /* input/output file */
      {

     /* first parameter is input file */
     if (vps2pcs.in_stream == NULL)
     {
        strcpy (buf, argv[i]);
#ifdef AUTO_EXTENION
        strcat (buf, ".");
        strcat (buf, INP_EXT);
#endif /*  */
        vps2pcs.in_stream = fopen (buf, "r");
        if (vps2pcs.in_stream == NULL)
        {
           fprintf (vps2pcs.err_stream, "!!! File %s open error !!!\n", buf);
           return (1);
        }
        if (vps2pcs.out_stream == NULL)
        {
           strcpy (buf, argv[i]);
#ifdef AUTO_EXTENION
           strcat (buf, ".");
           strcat (buf, OUT_EXT);
#endif /*  */                 
           remove( buf );
           vps2pcs.out_stream = fopen (buf, "w");
           if (vps2pcs.out_stream == NULL)
           {
              fprintf (vps2pcs.err_stream, "!!! File %s open error !!!\n", buf);
              return (2);
           } 
           SetFileInfoLine( vps2pcs.out_stream, buf );
        }
     }
     else
     {
        vps_to_pcs_usage ();
        return (3);
     }
      }
   }
   /*-----------------------------------------------
   if no in/out file specified use stdin/stdout
   -------------------------------------------------
   */

   if (vps2pcs.in_stream == NULL)
   {
      vps2pcs.in_stream = stdin;
   }
   if (vps2pcs.out_stream == NULL)
   {
      vps2pcs.out_stream = stdout;
   }

   /*----------------------------------------------------------
   main loop to bulid pcs primform from vps
   ------------------------------------------------------------*/

   vps.SetInStream ( vps2pcs.in_stream );
   vps.SetOutStream( vps2pcs.out_stream );
   vps.SetErrStream( vps2pcs.err_stream );
   vps.SetMsgStream( vps2pcs.out_stream );
   vps.TxtOn();
   vps.MsgOff();

   while( vps.ReadVpsLine() != -1 )
   {
      if( vps2pcs.ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      code = vps.GetPrimCode();
      //code = vps.GetCode();
      //code = vps2pcs.primcode( code );
      vps2pcs.prim_to_str (code, str);
      fprintf (vps2pcs.out_stream, "%s\n", str);
   }


   if (vps2pcs.in_stream != stdin)
      fclose (vps2pcs.in_stream);

   if (vps2pcs.out_stream != stdout)
      fclose (vps2pcs.out_stream);

   if (vps2pcs.err_stream != stderr)
      fclose (vps2pcs.err_stream);
   return (0);
}


/*---------------------------------------------------------------------
    convert code to note string
    note_str is a buffer for return sting
    size must greater 30 characters
    example :
    char buf[31];
    code_to_node( code, buf );
-----------------------------------------------------------------------
*/
int
Vps2Pcs::code_to_note (uint code, char *note_str)
{

   int     i;
   char    buf[10];
   *note_str = 0;       /* set first position */

   for (i = 0; i < 12; i++)
   {
      if (code >= 0x8000)   /* is hight bit on  */
      {
     if (i < 10)
     {
        buf[0] = (char) i + '0';
        buf[1] = ',';
        buf[2] = 0;
     }
     else
     {
        buf[0] = '1';
        buf[1] = (char) (i - 10) + '0';
        buf[2] = ',';
        buf[3] = 0;
     }
     strcat (note_str, buf);
      }
      code <<= 1;
   }
   return (0);
}
/*---------------------------------------------------------------------
    convert note string to code
    note_str : 0,3,7
    example :
    char buf[20];
    code_to_node( code, buf );
-----------------------------------------------------------------------
*/
uint
Vps2Pcs::note_to_code (char *note_str)
{

   uint    code = 0;
   char    ch;

   while ((ch = *note_str++) != 0)
   {
      if (!isdigit (ch))
     continue;      /* ch is no digit */
      else
      {
     if (isdigit (*note_str))
     {
        /* double number */
        SET_BIT (code, 10 * (int) (ch - '0') + (int) (*note_str - '0'));
        note_str++;
     }
     else
     {
        /* singel number */
        SET_BIT (code, ch - '0');
     }
      }
   }

   return (code);

}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
uint
Vps2Pcs::primcode (uint code)
{
   int     i, j, length = 32;
   uint    prim, test = 0;
   ulong   trans;

   trans = prim = 0xFFF0 & code;/* left 12 bit */
   trans <<= 12;
   trans |= (ulong) prim;
   trans <<= 4;

   for (i = 0; i < 12; i++)
   {
      if (trans & 0x80000000L)
      {
     test = (uint) (trans >> 16);
     test &= 0xFFF0;
#ifdef DEBUG
     {
        char    str[35];
        code_to_note (code, str);
        printf ("\n %s", str);
        code_to_note (test, str);
        printf (" %s", str);
     }

#endif /*  */
     /* test length */
     for (j = 11; j >= 0; j--)
     {
        if (test & (0x8000 >> j))
           break;
     }
     if (j < length)
     {
        prim = test;
        length = j;
     }
     else if (test > prim && j <= length)
     {
        prim = test;
        length = j;
     }
#ifdef DEBUG
     {
        char    str[50];

        code_to_note (prim, str);

        printf ("\n %x %2d %s", prim, length, str);
        printf ("\ttrans : %8lx", trans);
     }

#endif /*  */
      }
      trans <<= 1;      /* shift one pos to left */
   }

   /* mirror note-code */

#ifdef DEBUG
   printf ("\n mirror note-code ");

#endif /*  */
   for (i = 0, test = 0; i < 12; i++)
   {
      if (code & 0x8000)    /* test first bit */
      {
     SET_BIT (test, 11 - i);
      }
      code <<= 1;
   }

   trans = test;
   trans <<= 12;
   trans |= (ulong) test;
   trans <<= 4;

   for (i = 0; i < 12; i++)
   {
      if (trans & 0x80000000L)
      {
     test = (uint) (trans >> 16);
     test &= 0xFFF0;
#ifdef DEBUG
     {
        char    str[50];
        code_to_note (test, str);
        printf ("\n %s", str);
     }
#endif /*  */
     /* test length */
     for (j = 11; j >= 0; j--)
     {
        if (test & (0x8000 >> j))
           break;
     }
     if (j < length)
     {
        prim = test;
        length = j;
     }
     else if (test > prim && j <= length)
     {
        prim = test;
        length = j;
     }
#ifdef DEBUG
     {

        char    str[50];

        code_to_note (prim, str);
        printf ("\n %x %2d %s", prim, length, str);
        printf ("\ttrans : %8lx", trans);
     }
#endif /*  */
      }
      trans <<= 1;      /* shift one pos to left */
   }

   return (prim);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int
Vps2Pcs::prim_to_str (uint primcode, char *str)
{
   int     index = 0;

   /* search primform */
   while ((primtab[index].primcode != primcode)
      && (primtab[index].primcode != 0)
      )
   {
      index++;
   }
   strcpy (str, primtab[index].prim_str);

   if( str[0] == 'i' )
   {
     if (vps2pcs.err_stream != NULL)
     {
    fprintf (vps2pcs.err_stream, "vps2pcs invalid PrimCode at bar %d\n", vps.GetBar());
     }
     return -1;
   }
   else
   {
     return  0;
   }
}
