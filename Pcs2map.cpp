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


FILE *err_stream = stderr;

int    VerboseFlag  = 0;
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
char PROG_NAME[] =  "pcs2map";

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
{0, 0, 1, 7, 19, 48, 86, 136, 186, 186 + 38};   /* start index for group */

#elif MAX_LEVEL == 6
unsigned int pcs_matrix[1 + 6 + 12 + 29 + 38 + 50][1 + 6 + 12 + 29 + 38 + 50];

unsigned long histo_count[1 + 6 + 12 + 29 + 38 + 50];

int     group_index[] =
{0, 0, 1, 7, 19, 48, 86, 136, 136 + 50};        /* start index for group */

#else /*  */
long int pcs_matrix[1 + 6 + 12 + 29 + 38][1 + 6 + 12 + 29 + 38];

unsigned long histo_count[1 + 6 + 12 + 29 + 38];

int     group_index[] =
{0, 0, 1, 7, 19, 48, 86, 124};  /* start index for group */

#endif /*  */


/*--------------------
   FUNCTION DEFINITON
----------------------
*/


int     FormularHeader(FILE * stream, int max_group);
int     AxisText(FILE * stream, double x, double y, char *str);
int     DrawRaster (FILE * stream, int max_group);
int     DrawHisto(FILE * stream, int max_group);
int     DrawHistoText(FILE * stream, double max );
int     DrawAxisText(FILE * stream, int max_group);
int     formular_line (FILE * stream, int x_min, int y_min, int x_max, int y_max);
int     formular_box (FILE * stream, int x, int y, int style);
int     formular_end (FILE * stream);
int     DrawFormular(FILE * stream, FILE * info_stream, int max_group);
int     DrawPcs(FILE * stream, int max_group);
void    VpsToDiaUsage (void);


/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/

int FormularHeader(FILE * stream, int max_group)
{
   // set Header and global data

   GlobalPs( stream ); 
    
   if (ColorOutputFlag)
   {
#define FULL_BOX_MODE
#ifdef  FULL_BOX_MODE
            fprintf (stream, "%% ------------------------\n"
                     "/box1\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 0 1 setrgbcolor\n"
                     " 0.1 0.9 rmoveto\n"
                     " 0.8 0 rlineto\n"
                     " 0 -0.8 rlineto\n"
                     " -0.8 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box2\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 1 0 setrgbcolor\n"
                     " 0.1 0.9 rmoveto\n"
                     " 0.8 0 rlineto\n"
                     " 0 -0.8 rlineto\n"
                     " -0.8 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box3\n"
                     "{\n"
                     " newpath moveto\n"
                     " 1 0 0 setrgbcolor\n"
                     " 0.1 0.9 rmoveto\n"
                     " 0.8 0 rlineto\n"
                     " 0 -0.8 rlineto\n"
                     " -0.8 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
           );
#else
   fprintf (stream, "%% ------------------------\n"
                     "/box1\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 0 1 setrgbcolor\n"
                     " 0.25 0.75 rmoveto\n"
                     " 0.5 0 rlineto\n"
                     " 0 -0.5 rlineto\n"
                     " -0.5 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box2\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 1 0 setrgbcolor\n"
                     " 0.25 0.75 rmoveto\n"
                     " 0.5 0 rlineto\n"
                     " 0 -0.5 rlineto\n"
                     " -0.5 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box3\n"
                     "{\n"
                     " newpath moveto\n"
                     " 1 0 0 setrgbcolor\n"
                     " 0.25 0.75 rmoveto\n"
                     " 0.5 0 rlineto\n"
                     " 0 -0.5 rlineto\n"
                     " -0.5 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
               );

#endif
   }
   else
   {
            fprintf (stream, "%% --- define procedures ---\n"
                     "%% --- stack: x y => ---\n"
                     "/box1\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 0 0 setrgbcolor\n"
                     " 1 0 rlineto\n"
                     " 0 1 rlineto\n"
                     " -1 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box2\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 0 0 setrgbcolor\n"
                     " 0.25 0.75 rmoveto\n"
                     " 0.5 0 rlineto\n"
                     " 0 -0.5 rlineto\n"
                     " -0.5 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
                     "/box3\n"
                     "{\n"
                     " newpath moveto\n"
                     " 0 0 0 setrgbcolor\n"
                     " 0.33 0.66 rmoveto\n"
                     " 0.33 0 rlineto\n"
                     " 0 -0.33 rlineto\n"
                     " -0.33 0 rlineto\n"
                     " closepath fill\n"
                     "} def\n"
               );
   }

   /* set line width for Raster */
   /* normal line width */
   fprintf (stream, "/LW1\n{\n %lf setlinewidth\n}def\n", 0.5 * PsLineScale);

   /* double      line width */
   fprintf (stream, "/LW2\n{\n %lf setlinewidth\n}def\n", 1.0 * PsLineScale);
   fprintf (stream, "/Line { newpath moveto lineto closepath stroke } def\n");

   /* print raster line for diagramm                     */
   /* stack : i, min, max  --> min,i,max,i,i, min,i,max
            3 -1 roll  --> min max i
            dup   --> min max i i
            3 1 roll   --> min i max i
            4 copy        --> min i max i min i max i
            4 -1 roll  --> min i max i i min i max
                     x  y1 x2 y2
                       */
   fprintf (stream, "/MinMaxLine\n"
                    "{\n"
                    "     3 -1 roll dup 3 1 roll 4 copy 4 -1 roll Line Line\n"
                    "} def %% MinMaxLine\n"
           );

   /* print box */
   /* param stack : fillGray xmin ymin xmax ymax */
   fprintf (stream, "/Box\n"
                    "{\n"
                    " newpath\n"
                    " 0 setgray\n"
                    " 2 copy moveto\n"    /* Xmax Ymax */
                    " 3 copy pop exch lineto\n"   /* Ymin Xmax */
                    " 4 copy\n"
                    " pop pop lineto\n"   /* Ymin Xmin */
                    " exch pop exch pop lineto\n" /* Xmin Ymax */
                    " closepath\n"
                    " fill\n"
                    " stroke\n"
                    "}def %% Box\n"
           );
   /* print frame */
   /* param stack : xmin ymin xmax ymax */
   fprintf (stream, "/Frame\n"
                    "{\n"
                    " newpath\n"
                    " 0 setgray\n"
                    " 2 copy moveto\n"    /* Xmax Ymax */
                    " 3 copy pop exch lineto\n"   /* Ymin Xmax */
                    " 4 copy\n"
                    " pop pop lineto\n"   /* Ymin Xmin */
                    " exch pop exch pop lineto\n" /* Xmin Ymax */
                    " closepath\n"
                    " stroke\n"
                    "}def %% Frame\n"
            );

   return (0);
}

/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/

int AxisText(FILE * stream, double x, double y, char *str)
{
double  dx, dy;
double ScaleSize = 1.0 / RasterXScale;

         dx = (x < 0) ? x * ScaleSize : x;
         dy = (y < 0) ? y * ScaleSize : y;

         fprintf (stream, "%lf %lf moveto\n"
                  "(%s) show\n",
                  dx, dy, str
                 );
   return (0);
}


/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/

int formular_line (FILE * stream, int x_min, int y_min, int x_max, int y_max)
{
   fprintf (stream, "%d %d %d %d Line\n",
                     x_min, y_min, x_max, y_max);
   return (0);
}

/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/
int formular_box (FILE * stream, int x, int y, int mode)
{
float   fx = (float) x, fy = (float) y;

         switch (mode)
         {
         case BOX_FULL:

            fprintf (stream, "%d %d box1\n", x, y);
            break;

         case BOX_SHMAL:
            fprintf (stream, "%d %d box2\n", x, y);
            break;

         case BOX_CROSS:
            fprintf (stream, "%d %d box3\n", x, y);
            break;
         }
         
   return (0);
}

/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/
int formular_end (FILE * stream)
{
	fprintf (stream, "gsave\n");
	PsPageData.NewPage( stream );

    fprintf (stream, "%%--- Info Page ---\n"
                  "gsave\n"
                  " %d %d translate\n"     /* 1 */
                  " %lf %lf scale\n"       /* 2 */
                  " gsave\n"
	         // "  PageFrame\n"
                  "  InfoText\n"
                  "  showpage\n"
                  " grestore\n"
                  "grestore\n",
          /* 1 */ PsPageData.Page.TransX(), PsPageData.Page.TransY(),
          /* 2 */ PsPageData.Page.ScaleX(), PsPageData.Page.ScaleY()
			  );		  


	fprintf (stream, "grestore\n");
	fprintf (stream, "gsave\n");
	PsPageData.NewPage( stream );
	fprintf (stream, "%%--- Histo Page ---\n"
                  " gsave\n"
               // "   PageFrame\n"
                  "   %d %d translate\n"   /* 3 */
                  "   %lf %lf scale\n"     /* 4 */
                  "   DrawPcs\n"
                  "   DrawRaster\n"
                  "   AxisText\n"
                  " grestore\n"
                  " gsave\n"
                  "  %d %d translate\n"    /* 5 */
                  "  %lf %lf scale\n"
                  "  DrawHisto\n"
                  " grestore\n"
                  " gsave\n"
                  "  HistoText\n"
                  " grestore\n"
                  "grestore\n"
                  "/#copies 1 def\n"
                  "showpage\n",
          /* 3 */ PsPageData.LMergin()+RasterTextX,PsPageData.Top()- MapXSize+RasterTextX,     /* translate Raster to the Left mergin and Top*/
          /* 4 */ RasterXScale, RasterYScale,   /* DrawRaster */             /* scale Raster */
          /* 5 */ PsPageData.LMergin()+RasterTextX,PsPageData.Bot(),                 /* translate Hisogramm to the Left mergin and Bop*/
                  RasterXScale, 1.0             /* DrawHisto  */             
            );
   fprintf (stream, "grestore\n");
	
   return (0);
}


/*-------------------------------------------------------------
---------------------------------------------------------------
*/

/*
#pragma optimize( "", off )
*/

int DrawRaster(FILE * stream, int max_group)
{
int    group = 1, num = 0, text_line = 0;
double ScaleSize = 1.0 / RasterXScale;

   /* draw output raster */
   
   fprintf (stream, "/DrawRaster\n{\n");
   fprintf (stream, " 0 setgray\n");

   /* for loop for raster output */
   fprintf (stream, "%%--- draw raster lines ----\n");
   for (group = max_group; group > 1; group--)
   {
            fprintf (stream, "LW2 %d %lf %lf MinMaxLine\n", group_index[group], -2 * ScaleSize, (double) group_index[max_group] + 2 * ScaleSize);
            fprintf (stream, "%d %d %d\n{\n", group_index[group - 1], 1, group_index[group]);
            fprintf (stream, " dup %d sub 5 mod 0 eq\n", group_index[group - 1]);
            fprintf (stream, " { LW2 %lf %lf MinMaxLine}\n", -ScaleSize, (double) group_index[max_group] + 1 * ScaleSize);
            fprintf (stream, " { LW1 0 %d MinMaxLine} ifelse\n", group_index[max_group]);
            fprintf (stream, "} for\n");
   }
   fprintf (stream, "} def %% DrawRaster\n");

   return (0);
}

/*-------------------------------------------------------------

---------------------------------------------------------------
*/
int DrawAxisText(FILE * stream, int max_group)
{
int        group = 1;
int        i;
const int    XPos = - RasterTextX;
const int    YPos = RasterTextY;


         fprintf (stream, "/AxisText\n{\n");
         fprintf (stream, " 0 setgray\n");

         /* print axis text */
         fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",
                  GS_FONT_NAME,   /* see Info-ps.h */
                  AxisTextScale
            );

         for (i = 0, group = 1; i <= group_index[max_group]; i++)
         {
            if (i == group_index[group])
            {
               if (i != group_index[max_group])
               {
                  char    str[2];

                  str[0] = (char) (group + '0');
                  str[1] = 0;

                  if( group == 1 )
                  {
                     AxisText(stream, XPos, -YPos, str);
                  }
                  else
                  {
                     AxisText(stream, XPos, i, str);
                     AxisText(stream, i, -YPos, str);
                  }
               }
               group++;
            }
         }

         // Use Normal font for "pcs" text

         fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",            
                  GS_FONT_NAME,
                  AxisTextScale
            );

         // y axis
         
         AxisText( stream, XPos,group_index[max_group]-1.0*AxisTextScale, "p" );         
         AxisText( stream, XPos,group_index[max_group]-1.9*AxisTextScale, "c" );         
         AxisText( stream, XPos,group_index[max_group]-2.8*AxisTextScale, "s" );         
         // Use  font (small) for indices
         fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",
                  GS_ITALIC_FONT_NAME,
                  AxisTextScale * 0.66
            );
         AxisText( stream, XPos,group_index[max_group]-3.6*AxisTextScale, "n" );         

         
         // x axis

         fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",            
                  GS_FONT_NAME,
                  AxisTextScale
            );
         AxisText( stream, group_index[max_group]- (40.0 / RasterYScale),-YPos,"pcs" );
         // Use Standart font (small) for indices
         fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",
                  GS_ITALIC_FONT_NAME,
                  AxisTextScale * 0.66
            );
         AxisText( stream, group_index[max_group]- (15.0 / RasterYScale),-YPos,"n+1" );
         
         fprintf (stream, "} def %c AxisText\n", '%');
         
   return (0);
}

/*-------------------------------------------------------------
---------------------------------------------------------------
*/
int DrawHisto(FILE * stream, int max_group)
{
int     group = 1, num = 0, text_line = 0;
int     i, j;
unsigned long max_count, sum;
double ScaleSize = 1.0 / RasterXScale;

   /* draw Histogram output raster */
   
   fprintf (stream, "/DrawHisto\n{\n");

   /* for loop for raster output */
   fprintf (stream, "%%--- draw histo raster lines ----\n");

   for (group = max_group; group > 1; group--)
   {
            fprintf (stream, "%d %lf %d %lf LW2 Line\n", group_index[group], -2.0 * ScaleSize, group_index[group], HistoYSize + 2 * ScaleSize);
            fprintf (stream, "%d %d %d\n{\n", group_index[group - 1], 1, group_index[group]);
            fprintf (stream, " dup %d sub 5 mod 0 eq\n", group_index[group - 1]);
            fprintf (stream, " { dup %lf exch %lf LW2 Line}\n", -1.0 * ScaleSize, HistoYSize + 1 * ScaleSize);
            fprintf (stream, " { dup %d exch %d LW1 Line} ifelse\n", 0, HistoYSize);
            fprintf (stream, "} for\n");
   }
   fprintf (stream, "%%--- draw frame ----\n");
   fprintf (stream, "0 0 %d %d Frame\n", group_index[max_group], HistoYSize);
   fprintf (stream, "%%--- draw data ----\n");
   
   max_count = sum = 0L;
   for (i = 0; i < group_index[max_group]; i++)
   {
            histo_count[i] = 0L;
            for (j = 0; j < group_index[max_group]; j++)
            {
               histo_count[i] += pcs_matrix[j][i];
            }
            sum += histo_count[i];
            if (max_count < histo_count[i])
                max_count = histo_count[i];
   }
   if( max_count != 0 )
         for (i = 0; i < group_index[max_group]; i++)
         {
            fprintf (stream, " %d %d %d %d Box\n", i, 0, i + 1, (int) (HistoYSize * histo_count[i] / max_count));
         }


   fprintf (stream, "} def %% DrawHisto\n");

   if( max_count != 0 )
     DrawHistoText( stream,  (double)max_count * 100.0 / (double)sum );
   else
     DrawHistoText( stream, 0.0 );

   return (0);
}
//-------------------------------------------------------------------
int DrawHistoText(FILE * stream, double max )
{
   /* draw Histogram output raster */
   
   fprintf (stream, "/HistoText\n{\n");
   fprintf (stream, "%%--- draw histo text ----\n");


   //-----------------------------
   // Draw Histo scale Text
   //-----------------------------


   // Use Standart font for indices

   fprintf (stream, "/%s findfont\n%lf scalefont\nsetfont\n",
                     GS_FONT_NAME,
                    (double)PS_FONT_SIZE
           );

   if( max >= 99.5 )
   {
            fprintf (stream, "%lf %lf moveto\n"
                             "(100) show\n",
                             (double)PsPageData.LMergin(), (double) HistoYSize - PS_FONT_SIZE +  PsPageData.Bot()
                    );
   }
   else
   if( max >= 10.0 )
   {
            fprintf (stream, "%lf %lf moveto\n"
                             "(%2.0lf) show\n",
                             (double)PsPageData.LMergin(), (double) HistoYSize - PS_FONT_SIZE  +  PsPageData.Bot(), max
                    );
   }
   else
   {
            fprintf (stream, "%lf %lf moveto\n"
                             "(%2.1lf) show\n",
                             (double)PsPageData.LMergin(), (double) HistoYSize - PS_FONT_SIZE  +  PsPageData.Bot(), max
                    );
   }
   fprintf (stream, "%lf %lf moveto\n"
                    "(%c) show\n",
                     (double)PsPageData.LMergin(), (double) HistoYSize +  PsPageData.Bot() - 2.0 * PS_FONT_SIZE, '%'
           );

   fprintf (stream, "%lf %lf moveto\n"
                    "(0) show\n",
                     (double)PsPageData.LMergin(), (double) PsPageData.Bot()
           );
   fprintf (stream, "} def %% HistoText\n");


   return (0);
}
/*-------------------------------------------------------------
---------------------------------------------------------------
*/

int DrawFormular (FILE * stream, FILE * info_stream, int max_group)
{
   max_group++;
   
   /*---------------------------------------------------------------------------------
        init global variables
   -----------------------------------------------------------------------------------
   */ 
   if( PsPageData.GetOrientation() == Landscape )
   {
       MapYSize = PsPageData.YSize();
       MapXSize = PsPageData.YSize() - PsPageData.YSize()/4;
   }
   else
   {
      MapXSize = PsPageData.XSize();
      MapYSize = PsPageData.YSize();
   }

   RasterXScale = (double) (MapXSize - RasterTextX - RasterScaleSize) / ((double) group_index[max_group]);
   RasterYScale = RasterXScale;
   HistoYSize   = MapYSize - MapXSize - 15;
   PsLineScale = 1.0 / RasterXScale;
   AxisTextScale = 14 / RasterXScale;

   FormularHeader(stream, max_group);
   PsInfoPage(stream, info_stream);
   DrawRaster(stream, max_group);
   DrawAxisText(stream, max_group);
   DrawPcs(stream, max_group);
   DrawHisto(stream, max_group);
   formular_end (stream);

   return (0);
}
/*-------------------------------------------------------------
---------------------------------------------------------------
*/
int DrawPcs(FILE * stream, int max_group)
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
                     fprintf (stream, "/PcsData%-d\n{\n", count / PCS_DATA_SIZE);
                  }
                  /* fprintf( stream, "%d %d %d\n", i,j,count );
                                            */
                  if (ton_flag[i] == 'D' && ton_flag[j] == 'D')
                  {
                     fprintf (stream, "%d %d %d\n", i, j, BOX_FULL);
                  }
                  else if (ton_flag[i] == 'C' && ton_flag[j] == 'C')
                  {
                     fprintf (stream, "%d %d %d\n", i, j, BOX_CROSS);
                  }
                  else
                  {
                     fprintf (stream, "%d %d %d\n", i, j, BOX_SHMAL);
                  }

                  count++;
                  if (!(count % PCS_DATA_SIZE))
                  {
                     /* write end of pcs data block to PS file */
                     fprintf (stream, "0 1 %d\n {\n", PCS_DATA_SIZE - 1);
                     fprintf (stream, "  pop\n"
                              "  dup 1 eq { pop box1 1 }if\n"
                              "  dup 2 eq { pop box2 2 }if\n"
                              "  dup 3 eq { pop box3 3 }if\n"
                              "  pop\n"
                        );

                     fprintf (stream, " } for %c print PCS elements\n", '%');

                     fprintf (stream, "} def %c PcsData%-d\n", '%', count / PCS_DATA_SIZE - 1);
                  }
               }

            }
         }

         if (count % PCS_DATA_SIZE)
         {
            fprintf (stream, "0 1 %d\n {\n", (count % PCS_DATA_SIZE) - 1);
            fprintf (stream, "  pop\n"
                     "  dup 1 eq { pop box1 1 }if\n"
                     "  dup 2 eq { pop box2 2 }if\n"
                     "  dup 3 eq { pop box3 3 }if\n"
                     "  pop\n"
               );
            fprintf (stream, " } for %c print PCS elements\n", '%');

            fprintf (stream, "} def %c PcsData%-d\n", '%', count / PCS_DATA_SIZE);
         }
         fprintf (stream, "/DrawPcs\n{\n");
         fprintf (stream, "  LW1\n");   /* set line width */

         for (i = 0; i < count; i += PCS_DATA_SIZE)
         {
            fprintf (stream, "  PcsData%-d\n", i / PCS_DATA_SIZE);
         }
         fprintf (stream, "} def %c DrawPcs\n", '%');
         
   return (0);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void VpsToDiaUsage ()
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
   fprintf (err_stream, "        -3 diagramm in 3 levels\n");
   fprintf (err_stream, "        -4 diagramm in 4 levels\n");
   fprintf (err_stream, "        -5 diagramm in 5 levels\n");
#if MAX_LEVEL > 5
   fprintf (err_stream, "        -6 diagramm in 6 levels\n");
#endif /*  */
#if MAX_LEVEL > 6
   fprintf (err_stream, "        -7 diagramm in 7 levels\n");
#endif /*  */
   fprintf (err_stream, "        -xo=n x offset\n");
   fprintf (err_stream, "        -xs=n x scale\n");
   fprintf (err_stream, "        -yo=n y offset\n");
   fprintf (err_stream, "        -ys=n y scale\n");
   fprintf (err_stream, "        -C color output\n");
   fprintf (err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
   int     group, number, last_index = -1;
   char    buf[_MAX_PATH];
   int     i, disp_level = 3;
   FILE   *in_stream = NULL, *out_stream = NULL, *info_stream = NULL;
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
            VpsToDiaUsage ();
            return (0);
            break;
         case 'v':
            rameau_version ( err_stream );
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
         case 'C':
            ColorOutputFlag = 1;
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
               VpsToDiaUsage ();
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
               VpsToDiaUsage ();
            break;

         default:
            VpsToDiaUsage ();
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
               SetRameauExt(buf, MAP );
               out_stream = fopen (buf, "wb");
               if (out_stream == NULL)
               {
                  fprintf (err_stream, "!!! File %s open error !!!\n", buf);
                  return (2);
               }
            }
         }
         else
         {
            VpsToDiaUsage ();
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

   PsPageData.SetPaperType( PageFormat, PageOrientation );

   /* clear pcs matix */

   memset (pcs_matrix, 0, sizeof (pcs_matrix));

   pcs.TxtOff();
   pcs.MsgOff();
   pcs.SetInStream( in_stream );
   pcs.SetErrStream( err_stream );
   pcs.SetOutStream( NULL );
   pcs.SetMsgStream( NULL );


   while ( pcs.ReadPcsLine() != -1 )
   {
      group = pcs.GetGroup();
      number= pcs.GetNumber();

      if (disp_flag)
      {
             printf ("\n%d%d", group, number );
      }
      if( ReadLineFlag )
      {
             fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if (group > MAX_LEVEL)
      {
             fprintf (err_stream, "Invalid PCS %d-%d\n", group, number);
             last_index = -1;
      }
      if (last_index != -1)
      {
         /* increment info in pcs_matix */
         pcs_matrix[last_index][group_index[group] + number - 1]++;

         if (disp_flag)
         {
            printf (" group %3d number %3d count %5d", group, number, pcs_matrix[last_index][group_index[group] + number - 1]);
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
      fclose (in_stream);

   DrawFormular(out_stream, info_stream, disp_level);

   /* put info from file .inf in fomular */

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
