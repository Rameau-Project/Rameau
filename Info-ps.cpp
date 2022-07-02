/*--------------------------------------------------------------------------------
        Dieter Neubacher                    Vers. 1.0                    09.07.96
											Vers. 2.0					 27.01.97
        --------------------------------------------------------------------------
        info-ps.cpp
        
        Print Rameau Info Page

		Vers.: 2.0   WIN32, Ghostview

----------------------------------------------------------------------------------

Comments recognized by ghostview
================================

%!PS-Adobe-<real> [EPSF-<real>]
%%BoundingBox: <int> <int> <int> <int>|(atend)
%%CreationDate: <textline>
%%Orientation: Portrait|Landscape|(atend)
%%Pages: <uint>|(atend)
%%PageOrder: Ascend|Descend|Special|(atend)
%%Title: <textline>
%%DocumentMedia: <text> <real> <real> <real> <text> <text>
%%DocumentPageSizes: <text>
%%EndComments

%%BeginPreview
%%EndPreview

%%BeginDefaults
%%PageBoundingBox: <int> <int> <int> <int>|(atend)
%%PageOrientation: Portrait|Landscape
%%PageMedia: <text>
%%EndDefaults

%%BeginProlog
%%EndProlog

%%BeginSetup
%%PageBoundingBox: <int> <int> <int> <int>|(atend)
%%PageOrientation: Portrait|Landscape
%%PaperSize: <text>
%%EndSetup

%%Page: <text> <uint>
%%PageBoundingBox: <int> <int> <int> <int>|(atend)
%%PageOrientation: Portrait|Landscape
%%PageMedia: <text>
%%PaperSize: <text>

%%Trailer
%%EOF

%%BeginDocument: <text> [<real>[<text>]]
%%EndDocument

%%BeginBinary: <uint>
%%EndBinary

%%BeginData: <uint> [Hex|Binary|ASCII[Bytes|Lines]]
%%EndData


Paper Keywords and paper size in points
=======================================

Letter		 612x792
LetterSmall	 612x792
Tabloid		 792x1224
Ledger		 1224x792
Legal		 612x1008
Statement	 396x612
Executive	 540x720
A3		     842x1190
A4		     595x842
A4Small		 595x842
A5		     420x595
B4		     729x1032
B5		     516x729
Envelope	 ???x???
Folio		 612x936
Quarto		 610x780
10x14		 720x1008

----------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//#include <sys/timeb.h>

#include "rameau.h"
#include "info-ps.h"
#include "debug.h"

#ifdef __MS_DOS__
#include <conio.h>
#ifndef WIN32
#include <graph.h>
#endif
#include <direct.h>
#else /*  */
#define getch(a) getchar()
#endif /*  */

#define ESC    ('\x1b')
#define RET    ('\r')

/*---------------------------*/
/* global variables          */
/*---------------------------*/

pspagedata PsPageData( A3, Landscape ); 

/*---------------------------*/
/* init PS info fields       */
/*---------------------------*/

PS_INFO_FIELD PsInfoTxt[] =
{
 /* x, Line, input size */
   {
      0, 1, 6, "Analysis No", ""
   }
   ,
   {
      0, 2, 8, "Date", ""
   }
   ,
   {
      0, 3, 59, "Title", ""
   }
   ,
   {
      0, 4, 56, "Composer", ""
   }
   ,
   {
      0, 5, 44, "Opus", ""
   }
   ,
   {
      0, 6, 8, "Year", ""
   }
   ,
   {
      0, 7, 21, "Publisher", ""
   }
   ,
   {
      0, 8, 24, "Editor", ""
   }
   ,
   {
      0, 9, 56, "Movement", ""
   }
   ,
   {
      0,10, 8, "Key", ""
   }
   ,
   {
      0,11, 5, "Time Signature", ""
   }
   ,
   {
      0,12, 4, "Bar Nos from", ""
   }
   ,
   {
      20,12, 4, "to", ""
   }
   ,
   {
      0,13, 5, "Offbeat Value", ""
   }
   ,
   {
      0,14, 5, "Minimal Rhythm Value", ""
   }
   ,
   {
      0,15, 56, "Input By", ""
   }
};

/*----------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/           
pspagedata::pspagedata( enum PaperType type, enum PaperOrientation mode )
{
   PageNumber = 1;
   Mergin = (int) 2.0 * (int)POINTS_PER_CM;
   SetOrientation( mode );
   SetPaperType( type );
   Page.Set( PageXSize, PageYSize, 0,0,1.0,1.0);
}

pspagedata::pspagedata( enum PaperType type )
{
   PageNumber = 1;
   Mergin = (int) 2.0 * (int)POINTS_PER_CM;
   SetOrientation( Portrait );
   SetPaperType( type );
   Page.Set( PageXSize,PageYSize, 0,0,1.0,1.0);
}

pspagedata::pspagedata()
{
   PageNumber = 1;
   Mergin = (int) 2.0 * (int)POINTS_PER_CM;
   SetOrientation( Portrait );
   SetPaperType( A4 );
   Page.Set( PageXSize,PageYSize, 0,0,1.0,1.0);
}
pspagedata::Set( int xsize, int ysize )
{
   PageXSize= xsize;
   PageYSize= ysize;
   Xmin = 0 + Mergin;
   Xmax = xsize - Mergin;
   Ymin = 0 + Mergin;
   Ymax = ysize - Mergin; 
   
   return 0;
}
pspagedata::SetMergin( int size )
{
   Mergin = size;
   Xmin = 0 + Mergin;
   Xmax = PageXSize - Mergin;
   Ymin = 0 + Mergin;
   Ymax = PageYSize - Mergin; 
   
   return 0;
} 

pspagedata::SetPaperType( enum PaperType type, enum PaperOrientation mode )
{
	Orientation = mode;
    SetPaperType( type );
	return 0;
}

pspagedata::SetOrientation( enum PaperOrientation mode )
{
	Orientation = mode;

	return 0;
}

pspagedata::SetPaperType( enum PaperType type )
{
int XSize, YSize;

	switch( type )
	{
	case Letter	:	XSize = 612;	YSize = 792;	break;
	case Tabloid:	XSize = 792;	YSize = 1224;	break;
	case Ledger:	XSize = 1224;	YSize = 792; 	break;
    case Legal:		XSize = 612;	YSize = 1008; 	break;
    case Statement:	XSize = 396;	YSize = 612;  	break;
	case Executive:	XSize = 540;	YSize = 720; 	break;
	case A3:		XSize = 842;	YSize = 1190; 	break;
	case A4:		XSize = 595;	YSize = 842; 	break;
	case A5:		XSize = 420;	YSize = 595; 	break;
	case B4:		XSize = 729;	YSize = 1032; 	break;
	case B5:		XSize = 516;	YSize = 729; 	break;
	case Folio:		XSize = 612;	YSize = 936; 	break;
	case Quarto:	XSize = 610;	YSize = 780; 	break;

	default :
    	return 1;
	}
	Type = type;     // set new type

	if( Orientation == Portrait )
	{
       PageXSize= XSize;
       PageYSize= YSize;
	}
	else
	if( Orientation == Landscape )
	{
       PageXSize= YSize;
       PageYSize= XSize;
	}
	else
		return 2;

   
   Xmin = 0 + Mergin;
   Xmax = PageXSize - Mergin;
   Ymin = 0 + Mergin;
   Ymax = PageYSize - Mergin; 
   return 0;
} 

pspagedata::NewPage( FILE *stream )
{
	// Ps, GSView commands for new page

    fprintf( stream, "%%---NEXT-PAGE------------------------------\n" );
    fprintf( stream, "%%%%Page: %d %d\n", PageNumber, PageNumber );
	
	if( Orientation == Landscape )
	{
		fprintf( stream, "%d 0 translate\n", PageYSize );
		fprintf( stream, "90   rotate\n" );
	}

	PageNumber++;

	return PageNumber -1;
}
pspagedata::ScalingMode( int mode )
{
   switch( mode )
   {
     case PRINTER_SCALE_MODE :
          scalingMode = PRINTER_SCALE_MODE;
          Page = Printer;
          break;
     case SCREEN_SCALE_MODE : 
          scalingMode = SCREEN_SCALE_MODE;
          Page = Screen;
          break;
     case GRAPHIC_SCALE_MODE : 
          scalingMode = GRAPHIC_SCALE_MODE;
          Page = Graphic;
          break;             
     default :
          return -1;
          break;
   }
   return 0;         
}           

pspagedata::GetScalingParam( char *Name )   //Filename whithout ext.
{
char Buffer[256];
   
   // Printer 
   strcpy( Buffer, Name );
   strcat( Buffer, ".cfp");
   Printer.InitScaling( Buffer );
   // Screen 
   strcpy( Buffer, Name );
   strcat( Buffer, ".cfs");
   Screen.InitScaling( Buffer );
   // Graphic 
   strcpy( Buffer, Name );
   strcat( Buffer, ".cfg");
   Graphic.InitScaling( Buffer );
     
   return 0;
}
/*----------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/
scaling::scaling()
{
    Set( 200,400,0,0,1.0,1.0);
}
 

scaling::Set(int SizeX,int SizeY,int TransX,int TransY,double ScaleX,double ScaleY )
{  
   #ifdef DEBUG_SCALING
      printf( "\n Set Scaling Parameter %d %d %d %d %lf %lf",SizeX,SizeY,TransX,TransY,ScaleX,ScaleY );
      getchar();
   #endif
   SetSizeX(SizeX);
   SetSizeY(SizeY);    
   SetTransX(TransX);
   SetTransY(TransY);               
   SetScaleX(ScaleX);
   SetScaleY(ScaleY);
   return 0;
}
scaling::InitScaling( char *FileName )
{
FILE    *stream;
int     x,y,tx,ty;
double  sx,sy;
    
    #ifdef DEBUG_SCALING
       printf("\n Read Scaling File %s", FileName );
       getchar();
    #endif
    if( (stream = fopen( FileName, "r" )) == NULL )
    {
       #ifdef DEBUG_SCALING
          printf("\n open error" );
          getchar();
       #endif
       return -1;
    }
    if( fscanf(stream,"%d%d%d%d%lf%lf", &x,&y,&tx,&ty,&sx,&sy ) != 6 )
    {  
       fclose( stream );
       #ifdef DEBUG_SCALING
          printf("\n parameter error" );
          getchar();
       #endif
       return -2;
    }           
    fclose( stream );
    Set(x,y,tx,ty,sx,sy);
    return 0;
}

/*----------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/           
           
int pspagedata::PrintFrame(FILE * stream, int mergin )
{          
int Xmin = 0 + mergin;
int Xmax = PageXSize - mergin;
int Ymin = 0 + mergin;
int Ymax = PageYSize - mergin;

    fprintf( stream,
            "\n"       
            "newpath\n"
            "%d %d moveto\n"
            "%d %d lineto\n"
            "%d %d lineto\n"
            "%d %d lineto\n"
            "closepath\nstroke\n",
            Xmin,Ymin,Xmin,Ymax,Xmax,Ymax,Xmax,Ymin
           );  
    return 0;       
}           
           
           
/*--------------------------------------------------------------------------
    Rameau Copyright
----------------------------------------------------------------------------
*/                    
int pspagedata::PsDSCHeader( FILE *stream )
{
time_t ltime;
char TimeStr[100];
char paper[20];
char mode[20];

	switch( Type )
	{

		case A3 : strcpy( paper, "a3" ); break;
		case A4 : strcpy( paper, "a4" ); break;
		case A5 : strcpy( paper, "a5" ); break;
		case B4 : strcpy( paper, "b4" ); break;
		case B5 : strcpy( paper, "b5" ); break;
    
		default : strcpy( paper, "a4" ); break;
	}

	switch( Orientation )
	{
		case Portrait : strcpy( mode, "Portrait" ); break;
		case Landscape: strcpy( mode, "Landscape" ); break;
		default       : strcpy( mode, "Portrait" ); break;
	}
 

    /* Get UNIX-style time and display as number and string. */
    time( &ltime );
    sprintf( TimeStr, "%s", ctime( &ltime ) );


   // write Header and Copyright 
   // init for Ghostview

   fprintf (stream, "%%!PS-Adobe-2.0\n"
                    "%%%%Creator: RAMEAU v. 2.0 (c) 1994-97 by ARION Verlag\n"
                    "%%%%Title: Rameau File\n"
                    "%%%%CreationDate: %s"
                    "%%%%PageOrder: Ascend\n"
					"%%%%PaperSize: %s\n"
					"%%%%Orientation: %s\n" // Portrait|Landscape
                    "%%%%DocumentPaperSizes: %s\n"
                    "%%%%DocumentMedia: %s %d %d  80 white ( )\n"
                    "%%%%BoundingBox: 0 0 %d %d\n"
				    "%%%%EndComments\n",
					TimeStr,
					paper,
					mode,
					paper,
					paper, PageXSize, PageYSize,
					((Orientation == Portrait ) ? PageXSize : PageYSize),
					((Orientation == Portrait ) ? PageYSize : PageXSize)
					);

   return 0;
}
/*--------------------------------------------------------------------------
    Global PS-Commands
----------------------------------------------------------------------------
*/                    

int GlobalPs( FILE *stream )
{
   // write Header and Copyright 
   // init for Ghostview

	PsPageData.PsDSCHeader( stream );

    fprintf( stream, "%c--------- Global Functions ---------\n", '%' );
    fprintf( stream, "/ch {28.346456 mul } def\n" );
    fprintf( stream, "/PageTop { %d } def\n",PsPageData.Top() );
    fprintf( stream, "/PageBot { %d } def\n",PsPageData.Bot() );
    fprintf( stream, "/PageLeft { %d } def\n",PsPageData.LMergin() );
    fprintf( stream, "/PageRight { %d } def\n",PsPageData.RMergin() );
    
    fprintf( stream,
            "/PageFrame\n{\n"       
            "newpath\n"
            "%d %d moveto\n"
            "%d %d lineto\n"
            "%d %d lineto\n"
            "%d %d lineto\n"
            "closepath\nstroke\n} def\n",
            PsPageData.LMergin(),PsPageData.Bot(),
            PsPageData.LMergin(),PsPageData.Top(),
            PsPageData.RMergin(),PsPageData.Top(),
            PsPageData.RMergin(),PsPageData.Bot()
           );                     
           
    return 0;
}
/*--------------------------------------------------------------------------
    read rameau info
----------------------------------------------------------------------------
*/

#define MAX_ENTRY_LENGTH 40

int GetInfo (FILE * stream)
{
   int     line = 1, end_loop = 0, str_pos;
   char    buffer[TMP_BUFFER_LENGTH];
   char    str[TMP_BUFFER_LENGTH], entry[MAX_ENTRY_LENGTH];

   if (stream == NULL)
   {
      return -1;                /* no input file */
   }

   /* move to first file position */
   fseek (stream, 0L, SEEK_SET);

   do
   {                            /* read info entry and info string */

      if (fgets (buffer, TMP_BUFFER_LENGTH, stream) == NULL)
      {
         end_loop = 1;
         break;                 /* edn of while loop */
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
int disp_error_message (char *str)
{
   return 0;
}
/*-------------------------------------------------------------------
        Print Rameau Info Page
---------------------------------------------------------------------
*/
int PsInfoPage(FILE * stream, FILE * info_stream)
{
int     i = 0;
char    buf[256];
int     FontSize;

    FontSize = ( PsPageData.GetScalingMode() == PRINTER_SCALE_MODE ) ? PS_INFO_PRINTER_FONT_SIZE : PS_INFO_SCREEN_FONT_SIZE;

    fprintf (stream, "/InfoText\n"
                     "{\n"
                     "/%s findfont\n"
                     "%d scalefont\n"
                     "setfont\n"
                     " 0 0 0 setrgbcolor\n",
                     GS_FONT_NAME, 
                     FontSize
            );
    GetInfo(info_stream);

    for (i = 0; i < INFO_FIELD_COUNT; i++)
    {
        strcpy (buf, PsInfoTxt[i].name);
        strcat (buf, ": ");
        strcat (buf, PsInfoTxt[i].text);
        fprintf (stream, "%d %d moveto\n"
                         "(%s) show\n",
                         PsPageData.LMergin() + PsInfoTxt[i].x * FontSize / 2,
                         PsPageData.Top() - PsInfoTxt[i].y * FontSize,
                         buf
                );
   } 
   /*
   PrintA4Frame(stream,50);
   */
   fprintf (stream, "} def %c InfoText\n", '%');
   return (0);
}

