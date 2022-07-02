/*-------------------------------------------------------------------
     Dieter Neubacher     Vers.: 1.0            13.02.93
                                 1.1    Wuhu    31.03.94
                                 1.2    Wuhu    24.09.94
                                 1.4    Wuhu    18.07.96
                                 2.0            28.01.97
     -------------------------------------------------------------
     pcs2map.c

        vers.:  1.1  postscript scale, info text
                1.2  no ASCII outout
                1.4  Scalierungen, Info-Page, Farbausgabe, ...
                2.0  WIN32, Ghostview
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <malloc.h>

#include "rameau.h"
#include "tonflag.h"
#include "global.h"
#include "version.h"
#include "pcs.h"
#include "info-ps.h"


#define PCS_DATA_SIZE 100       /* max data block for pcs values while PS error */


FILE* err_stream = stderr;

int    VerboseFlag = 0;
int    ReadLineFlag = 0;


/*-------------------------------------------------------------
  group  elements
    1       1
    2       6
    3      12
    4      29
    5      38
    6      50
---------------------------------------------------------------
*/


/*-----------------------
   FLAGS
-------------------------
*/
int     disp_flag = 0;          /* display primform */
int     ColorOutputFlag = 0;         /* color output */


/*-----------------------
   DEFINITONS AND MACROS
-------------------------
*/

#define MAX_LEVEL 7             /* max level for pcs input (matrix dimension) */

#define BOX_FULL  1
#define BOX_SHMAL 2
#define BOX_CROSS 3

/*-----------------------
   GLOBAL VARIABLES
-------------------------
*/

Pcs pcs;

// char INP_EXT[]   =  ".pcs";
// char OUT_EXT[]   =  ".map";
char PROG_NAME[] = "pcs2seq";
char InputFileName[ 1024 ];

int     MapXSize, MapYSize;
int     HistoYSize;
double  RasterXScale;
double  RasterYScale;
double  PsLineScale;
double  AxisTextScale;
int     RasterTextX = 20;
int     RasterTextY = 20;
int     RasterScaleSize = 4;          // Axis-Scale-Size use for 3.4.5.6 constant size

/* matrix for pcs[n] to pcs[n+1] */

#if MAX_LEVEL == 7
unsigned int pcs_matrix[1 + 6 + 12 + 29 + 38 + 50 + 38][1 + 6 + 12 + 29 + 38 + 50 + 38];

unsigned long histo_count[1 + 6 + 12 + 29 + 38 + 50 + 38];

int     group_index[] =
{ 0, 0, 1, 7, 19, 48, 86, 136, 186, 186 + 38 };   /* start index for group */

#elif MAX_LEVEL == 6
unsigned int pcs_matrix[1 + 6 + 12 + 29 + 38 + 50][1 + 6 + 12 + 29 + 38 + 50];

unsigned long histo_count[1 + 6 + 12 + 29 + 38 + 50];

int     group_index[] =
{ 0, 0, 1, 7, 19, 48, 86, 136, 136 + 50 };        /* start index for group */

#else /*  */
long int pcs_matrix[1 + 6 + 12 + 29 + 38][1 + 6 + 12 + 29 + 38];

unsigned long histo_count[1 + 6 + 12 + 29 + 38];

int     group_index[] =
{ 0, 0, 1, 7, 19, 48, 86, 124 };  /* start index for group */

#endif /*  */


/*--------------------
   FUNCTION DEFINITON
----------------------
*/


int     DrawSeq(FILE* stream, int max_group);
void    VpsToSeqUsage(void);


/*-------------------------------------------------------------
---------------------------------------------------------------
*/

/*
#pragma optimize( "", off )
*/

/*-------------------------------------------------------------
---------------------------------------------------------------
*/
int DrawPcs(FILE* stream, int max_group)
{
    int     i, j, count = 0;

    for (i = 0; i < group_index[max_group]; i++)
    {
        for (j = 0; j < group_index[max_group]; j++)
        {
            if (pcs_matrix[i][j] > 0)
            {

                if (!(count % PCS_DATA_SIZE))
                {
                    /* write pcs data block to PS file */
                    fprintf(stream, "/PcsData%-d\n{\n", count / PCS_DATA_SIZE);
                }
                /* fprintf( stream, "%d %d %d\n", i,j,count );
                                          */
                if (ton_flag[i] == 'D' && ton_flag[j] == 'D')
                {
                    fprintf(stream, "%d %d %d\n", i, j, BOX_FULL);
                }
                else if (ton_flag[i] == 'C' && ton_flag[j] == 'C')
                {
                    fprintf(stream, "%d %d %d\n", i, j, BOX_CROSS);
                }
                else
                {
                    fprintf(stream, "%d %d %d\n", i, j, BOX_SHMAL);
                }

                count++;
                if (!(count % PCS_DATA_SIZE))
                {
                    /* write end of pcs data block to PS file */
                    fprintf(stream, "0 1 %d\n {\n", PCS_DATA_SIZE - 1);
                    fprintf(stream, "  pop\n"
                        "  dup 1 eq { pop box1 1 }if\n"
                        "  dup 2 eq { pop box2 2 }if\n"
                        "  dup 3 eq { pop box3 3 }if\n"
                        "  pop\n"
                    );

                    fprintf(stream, " } for %c print PCS elements\n", '%');

                    fprintf(stream, "} def %c PcsData%-d\n", '%', count / PCS_DATA_SIZE - 1);
                }
            }

        }
    }

    if (count % PCS_DATA_SIZE)
    {
        fprintf(stream, "0 1 %d\n {\n", (count % PCS_DATA_SIZE) - 1);
        fprintf(stream, "  pop\n"
            "  dup 1 eq { pop box1 1 }if\n"
            "  dup 2 eq { pop box2 2 }if\n"
            "  dup 3 eq { pop box3 3 }if\n"
            "  pop\n"
        );
        fprintf(stream, " } for %c print PCS elements\n", '%');

        fprintf(stream, "} def %c PcsData%-d\n", '%', count / PCS_DATA_SIZE);
    }
    fprintf(stream, "/DrawPcs\n{\n");
    fprintf(stream, "  LW1\n");   /* set line width */

    for (i = 0; i < count; i += PCS_DATA_SIZE)
    {
        fprintf(stream, "  PcsData%-d\n", i / PCS_DATA_SIZE);
    }
    fprintf(stream, "} def %c DrawPcs\n", '%');

    return (0);
}
/*-------------------------------------------------------------
---------------------------------------------------------------
*/
int DrawSeq(FILE* stream, int max_group)
{
    int     i, j, count = 0;

    for (j = 0; j < group_index[max_group]; j++)
    {
        for (i = 0; i < group_index[max_group]; i++)
        {
            // fprintf(stream, "%d,%d\n", i,j );
            if (pcs_matrix[i][j] > 0)
            {
                fprintf(stream, "%c", '1' );
//                fprintf(stdout, "%c", '1');
            }
            else
            {
                fprintf(stream, "%c", '0' );
//                fprintf(stdout, "%c", '0');
            }
        }
        fprintf(stream, "%c", ' ');
    }
    return (0);
}
//-------------------------------------------------------------------

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void VpsToSeqUsage()
{
    char str[10];

    str[0] = 0;
    SetRameauExt(str, PCS);

    fprintf(err_stream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, str);
    fprintf(err_stream, "\n");
    fprintf(err_stream, "flags : -? this output\n");
    fprintf(err_stream, "        -h this output\n");
    fprintf(err_stream, "        -v disp program version\n");
    fprintf(err_stream, "        -d display pcs\n");
    fprintf(err_stream, "        -o output to stdout\n");
    fprintf(err_stream, "        -i input from stdin\n");
    fprintf(err_stream, "        -l Disp Read line numbers\n");
    fprintf(err_stream, "        -3 diagramm in 3 levels\n");
    fprintf(err_stream, "        -4 diagramm in 4 levels\n");
    fprintf(err_stream, "        -5 diagramm in 5 levels\n");
#if MAX_LEVEL > 5
    fprintf(err_stream, "        -6 diagramm in 6 levels\n");
#endif /*  */
#if MAX_LEVEL > 6
    fprintf(err_stream, "        -7 diagramm in 7 levels\n");
#endif /*  */
    fprintf(err_stream, "\n");

    return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    int     group, number, last_index = -1;
    char    buf[_MAX_PATH];
    int     i, disp_level = 3;
    FILE* in_stream = NULL, * out_stream = NULL, * info_stream = NULL;
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
                VpsToSeqUsage();
                return (0);
                break;
            case 'v':
                rameau_version(err_stream);
                return (0);
                break;
            case 'e':
            {
                if ((err_stream = fopen(RAMEAU_ERROR_FILE, "at")) == NULL)
                {
                    err_stream = stderr;
                    fprintf(stderr, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
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

            case '3':
                disp_level = 3;
                break;
            case '4':
                disp_level = 4;
                break;
            case '5':
                disp_level = 5;
                break;
#if MAX_LEVEL > 5
            case '6':
                disp_level = 6;
                break;
#endif /*  */
#if MAX_LEVEL > 6
            case '7':
                disp_level = 7;
                break;
#endif /*  */
            default:
                VpsToSeqUsage();
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
                strcpy(buf, argv[i]);
                strcpy(InputFileName, argv[i]);
                strcat(buf, ".pcs");
                in_stream = fopen(buf, "r");

                if (in_stream == NULL)
                {
                    fprintf(err_stream, "!!! File %s open error !!!\n", buf);
                    return (1);
                }

                strcpy(buf, argv[i]);
                strcat(buf, ".inf");
                info_stream = fopen(buf, "r");

                if (info_stream == NULL)
                {
                    fprintf(err_stream, "!!! File %s open error !!!\n", buf);
                }

                if (out_stream == NULL)
                {
                    strcpy(buf, argv[i]);
                    strcat(buf, ".");
                    // SetRameauExt(buf, MAP);
                    strcat(buf, "seq");
                    out_stream = fopen(buf, "w");
                    if (out_stream == NULL)
                    {
                        fprintf(err_stream, "!!! File %s open error !!!\n", buf);
                        return (2);
                    }
                }
            }
            else
            {
                VpsToSeqUsage();
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

    /*
    fprintf(stdout, "stdout: %c", '1');
    fprintf(stderr, "stderr: %c", '1');
    fprintf(out_stream, "out stream: %c", '1');
    */

    /*----------------------------------------------------------
    main loop to bulid pcs primform from vps
    ------------------------------------------------------------
    */

 //   PsPageData.SetPaperType(PageFormat, PageOrientation);

    /* clear pcs matix */

    memset(pcs_matrix, 0, sizeof(pcs_matrix));

    pcs.TxtOff();
    pcs.MsgOff();
    pcs.SetInStream(in_stream);
    pcs.SetErrStream(err_stream);
    pcs.SetOutStream(NULL);
    pcs.SetMsgStream(NULL);


    while (pcs.ReadPcsLine() != -1)
    {
        group = pcs.GetGroup();
        number = pcs.GetNumber();

        if (disp_flag)
        {
            printf("\n%d%d", group, number);
        }
        if (ReadLineFlag)
        {
            fprintf(stdout, "\r%8ld", LineReadCounter++);
        }
        if (group > MAX_LEVEL)
        {
            fprintf(err_stream, "Invalid PCS %d-%d\n", group, number);
            last_index = -1;
        }
        if (last_index != -1)
        {
            /* increment info in pcs_matix */
            pcs_matrix[last_index][group_index[group] + number - 1]++;

            if (disp_flag)
            {
                printf(" group %3d number %3d count %5d", group, number, pcs_matrix[last_index][group_index[group] + number - 1]);
            }
        }

        if (group > MAX_LEVEL)
        {
            last_index = -1;
        }
        else
        {
            last_index = group_index[group] + number - 1;
        }
    }

    if (in_stream != stdin)
        fclose(in_stream);
    /*
    fprintf(stdout, "stdout: %c", '1');
    fprintf(stderr, "stderr: %c", '1');
    fprintf(out_stream, "out stream: %c", '1');
    */
    fprintf(out_stream, "<%s: %d>", InputFileName, disp_level );
    DrawSeq(out_stream, disp_level +1);
    fprintf(out_stream, "\n" );



    /* put info from file .inf in fomular */

    if (out_stream != stdout)
        fclose(out_stream);
    if (err_stream != stderr)
        fclose(err_stream);
    if (info_stream != NULL)
        fclose(info_stream);

    return (0);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
