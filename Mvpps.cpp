/*-------------------------------------------------------------------
     Dieter Neubacher     Vers.: 1.0                    10.07.93
	                      Vers.: 2.0                    28.01.97
     -------------------------------------------------------------

	 Vers.: 2.0     WIN32, Ghostview
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "MvpPs.h"
#include "MvpEntry.h"  
#include "Info-ps.h"

/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/

MvpPs::MvpPs()
{
   init();
}

MvpPs::MvpPs( int nVoices )
{
int i, nLines;

   Voices        = nVoices;
   nLines        = (nVoices) * (nVoices-1) / 2;
   LinesPerBlock = nLines;
   BlocksPerPage = (LinesPerPage+1) / (LinesPerBlock+1);
   mvpEntries = new  MvpEntry[ nLines ];

   for( i=0; i<nLines; i++ )
   {
      mvpEntries[i].Clear();
   }
}

void MvpPs::init()
{
   mvpEntries = NULL;

   OutStream = stdout;
   ErrStream = stderr;
   MsgStream = stdout;

   PsLineScale = 1.0;

   LeftMargin   = 3 * POINTS_PER_CM;
   RightMargin  = 2 * POINTS_PER_CM;
   ScaleTextPos = 2 * POINTS_PER_CM;
   TopMargin    = 2 * POINTS_PER_CM;
   BottenMargin = 2 * POINTS_PER_CM;


   LineSpaceSize = 45;
   ColSpaceSize  = 25;
   MarginSpace   = 10;

   LinesPerBlock = 0;
   Voices        = 0;
   
   fontSize   = 12;
   noteOffset = 2;
   oBoxHigh   = 3 * fontSize + noteOffset;
   oBoxWidth  = fontSize + 3 * noteOffset;
   qBoxHigh   = 2 * fontSize + noteOffset;
   qBoxWidth  = fontSize + 3 * noteOffset;
   bBoxHigh   = fontSize / 2;
   bBoxWidth  = fontSize;

   oLineOffset = (int)(2.5 * fontSize);
   qLineOffset = (int)(1.5 * fontSize);

   pointerHigh  = 4;
   pointerWidth = 6;

   // Output informations

   ActPos   = 0;
   ActBlock = 0;
   ActPage  = 0;
/***
   strcpy( NoteCode[ 0] , "H " );
   strcpy( NoteCode[ 1] , "C " );
   strcpy( NoteCode[ 2] , "C+" );
   strcpy( NoteCode[ 3] , "D " );
   strcpy( NoteCode[ 4] , "D+" );
   strcpy( NoteCode[ 5] , "E " );
   strcpy( NoteCode[ 6] , "F " );
   strcpy( NoteCode[ 7] , "F+" );
   strcpy( NoteCode[ 8] , "G " );
   strcpy( NoteCode[ 9] , "G+" );
   strcpy( NoteCode[10] , "A " );
   strcpy( NoteCode[11] , "A+" );
***/
   strcpy( NoteCode[ 0] , "h " );
   strcpy( NoteCode[ 1] , "c " );
   strcpy( NoteCode[ 2] , "c+" );
   strcpy( NoteCode[ 3] , "d " );
   strcpy( NoteCode[ 4] , "d+" );
   strcpy( NoteCode[ 5] , "e " );
   strcpy( NoteCode[ 6] , "f " );
   strcpy( NoteCode[ 7] , "f+" );
   strcpy( NoteCode[ 8] , "g " );
   strcpy( NoteCode[ 9] , "g+" );
   strcpy( NoteCode[10] , "a " );
   strcpy( NoteCode[11] , "a+" );

   Bar = LastBar = 0;
}


MvpPs::~MvpPs()
{
   delete mvpEntries;
}


FILE * MvpPs::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "wb");
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MvpPs output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * MvpPs::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w");
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MvpPs error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * MvpPs::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MvpPs massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}


/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/
int   MvpPs::SetMvpEntry( int line, MvpEntry *entry )
{

   if( line < 0 || line > LinesPerBlock-1 )
   {
      return -1;
   }

   // if new line write start lines

   if( ActPos   == 0 )
   {
   switch( mvpEntries[ line ].GetOColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d OLB\n",GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d OLB\n",GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d OLB\n",GetLine(line) );
                         break;
   }
   switch( mvpEntries[ line ].GetQColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d QLB\n",GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d QLB\n",GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d QLB\n",GetLine(line) );
                         break;
   }
   }

   // set color lines for Oktave and Quinte

   if( ActPos   < ColsPerPage-1 )
   {
   switch( entry->GetOColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d %d OLM\n",ActPos,GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d %d OLM\n",ActPos,GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d %d OLM\n",ActPos,GetLine(line) );
                         break;
   }
   switch( entry->GetQColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d %d QLM\n",ActPos,GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d %d QLM\n",ActPos,GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d %d QLM\n",ActPos,GetLine(line) );
                         break;
   }
   }
   else   // if new line write end lines
   if( ActPos   == ColsPerPage-1 )
   {
   switch( entry->GetOColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d %d OLE\n",ActPos,GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d %d OLE\n",ActPos,GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d %d OLE\n",ActPos,GetLine(line) );
                         break;
   }
   switch( entry->GetQColor() )
   {
      case  MVP_RED :
                         fprintf (OutStream, "RED %d %d QLE\n",ActPos,GetLine(line) );
                         break;
      case  MVP_GREEN :
                         fprintf (OutStream, "GREEN %d %d QLE\n",ActPos,GetLine(line) );
                         break;
      case  MVP_BLUE :
                         fprintf (OutStream, "BLUE %d %d QLE\n",ActPos,GetLine(line) );
                         break;
   }
   }
   // set Box info to Ps file   
   
   #define MAXNOTE(a,b) ((a>b)?a:b)
   #define MINNOTE(a,b) ((a<b)?a:b)

   switch( entry->GetStatus() )
   {
      case MVP_OKTAVE:

         fprintf (OutStream, "(%s) (%s) %d %d OBox\n",
                 NoteCode[ entry->GetCh1ModNote() ],NoteCode[ entry->GetCh2ModNote() ],
                 ActPos,GetLine( line )
         );
         break;

      case MVP_QUINTE:

         fprintf (OutStream, "(%s) (%s) %d %d QBox\n",
                 NoteCode[ MINNOTE(entry->GetCh1Note(),entry->GetCh2ModNote()) % 12 ],
                 NoteCode[ MAXNOTE(entry->GetCh1Note(),entry->GetCh2ModNote()) % 12 ],
                 // old 1.4 --- NoteCode[ entry->GetCh1ModNote() ],NoteCode[ entry->GetCh2ModNote() ],
                 ActPos,GetLine( line )
         );
         break;

      case MVP_BREAK:

         fprintf (OutStream, "%d %d BBox\n",
                 ActPos,GetLine( line )
         );
         break;

      default:

         return -2;
   }

   mvpEntries[line] = *entry;

   return 0;
}

void   MvpPs::NewTime( void )
{
    NewBar();
}

void MvpPs::NewBar( void )
{

   if( LastBar < Bar )
   {
      fprintf (OutStream, "0 setgray LW1 %d POS 3 sub %d ROW moveto %d POS 3 sub %d ROW 5 sub lineto stroke\n"
                          "%d POS 5 sub %d ROW 15 sub moveto (%d) show\n",
                          ActPos,GetLine( -1 ), ActPos,GetLine( LinesPerBlock-1 ),
                          ActPos,GetLine( LinesPerBlock-1 ),(int)Bar
              );

      LastBar = Bar;
   }
}

int   MvpPs::NextPos()
{
   if( ActPos < ColsPerPage-1 )
   {
      ++ActPos;
      return 0;
   }
   else
   if( ActBlock < BlocksPerPage-1 )
   {
      ActPos = 0;
      ++ActBlock;
      return 0;
   }
   else
   {
      NextPage();
      ActPos   = 0;
      ActBlock = 0;
      return 0;
   }
   return 0;
}

int   MvpPs::FirstPage()
{     
      // Print MVP-data
      ActPage = 2;
      InitPage();
      return 0;
}

int   MvpPs::NextPage()
{
      ++ActPage;
      fprintf (OutStream, " grestore\n"
                          "showpage\n"
              );

      InitPage();
      return 0;
}

int   MvpPs::InitPage( void )
{
int block, line;
int i,j;
int LineData[MAX_CHANNEL * MAX_CHANNEL][2], ldIndex;


      /* init line information data */

      ldIndex = 0;

      for( i=1; i <= Voices; i++ )
      {
        for( j=i+1; j <= Voices; j++ )
        {
          LineData[ldIndex][0] = i;
          LineData[ldIndex][1] = j;

          ldIndex++;
        }
      }
	  
	  PsPageData.NewPage( OutStream );

      fprintf (OutStream, "gsave\n"
                          "InitPage\n"
                          "Text\n"
              );
              
      // fprintf (OutStream, "%d %d %d %d Frame\n", 0, 0, PageXSize, PageYSize );

      for( block = 0; block < BlocksPerPage; block++ )
      {
        for( line = 0; line < LinesPerBlock; line++ )
        {
           fprintf (OutStream, "%d %d ROW moveto (%1d/%1d) show\n", ScaleTextPos, block * (LinesPerBlock+1)+line,LineData[line][0] ,LineData[line][1] );
        }
      }
      return 0;
}

int   MvpPs::GetLine( int line )
{
      return (LinesPerBlock+1) * ActBlock + line;
}

/*-------------------------------------------------------------------
        PsHeader( int ColorMode )
        ColorMode     0 = BW 1 = Color   
---------------------------------------------------------------------
*/
int MvpPs::PsHeader( int ColorMode )
{
		 GlobalPs( OutStream );

         fprintf (OutStream, "%c --- define procedures ---\n",'%' );
            
         /* define Color params */
         
         if( ColorMode == 1 )
         {
            fprintf (OutStream, "/RED\n{\n 1 0 0 setrgbcolor\n 0.5 setlinewidth\n}def\n");
            fprintf (OutStream, "/BLUE\n{\n 0 0 1 setrgbcolor\n 0.5 setlinewidth\n}def\n");
            fprintf (OutStream, "/GREEN\n{\n 0 1 0 setrgbcolor\n 0.5 setlinewidth\n}def\n");
         }
         else
         {
            fprintf (OutStream, "/RED\n{\n 0.00 setgray\n 0.33 setlinewidth\n}def\n");
            fprintf (OutStream, "/BLUE\n{\n 0.33 setgray\n 0.75 setlinewidth\n}def\n");
            fprintf (OutStream, "/GREEN\n{\n 0.66 setgray\n 1.50 setlinewidth\n}def\n");
         }
         fprintf (OutStream, "/ROW\n {\n %d mul neg %d add\n}def\n",LineSpaceSize, SizeY  - TopMargin - LineSpaceSize);
         fprintf (OutStream, "/POS\n {\n %d mul %d add\n}def\n", ColSpaceSize,LeftMargin+MarginSpace);


         /* set line width for Raster */
         /* normal line width */
         fprintf (OutStream, "/LW1\n{\n %lf setlinewidth\n}def\n", 1.0 * PsLineScale);

         /* double      line width */
         fprintf (OutStream, "/LW2\n{\n %lf setlinewidth\n}def\n", 0.5 * PsLineScale);
         fprintf (OutStream, "/Line { newpath moveto lineto closepath stroke } def\n");

         /* print oktave box */

         /* param stack :  note1 note1 pos line */
         fprintf (OutStream, "/OBox\n"
                  "{\n"
                  " newpath\n"
                  " LW2\n"
                  " ROW exch POS exch\n" /* calculat position */
                  " moveto\n"
                  " currentpoint\n"
                  " 0 %d rlineto\n"     /* draw box */
                  " %d 0 rlineto\n"
                  " 0 -%d rlineto\n"
                  " -%d 0  rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  1 setgray\n"
                  "  fill\n"
                  " grestore\n"
                  " 0.5 setlinewidth\n"
                  " 0 setgray\n"
                  " stroke\n"
                  " moveto\n"           /* home position */
                  " %d %d  rmoveto\n"   /* note1 position */
                  " currentpoint\n"
                  " 3 index show\n"     /* show first note */
                  " moveto\n"           /* note1 position */
                  " 0 %d rmoveto\n"     /* note2 position */
                  " show\n"             /* show second note */
                  " pop\n"
                  " stroke\n"
                  "}def %c OBox\n",
                  oBoxHigh,oBoxWidth,oBoxHigh,oBoxWidth,
                  noteOffset,noteOffset,
                  2 * fontSize,'%'

            );

         /* print quinten box */

         /* param stack :  note1 note1 pos line */
         fprintf (OutStream, "/QBox\n"
                  "{\n"
                  " newpath\n"
                  " LW2\n"
                  " ROW exch POS exch\n" /* calculat position */
                  " moveto\n"
                  " currentpoint\n"
                  " 0 %d rlineto\n"     /* draw box */
                  " %d 0 rlineto\n"
                  " 0 -%d rlineto\n"
                  " -%d 0  rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  1 setgray\n"
                  "  fill\n"
                  " grestore\n"
                  " 0.5 setlinewidth\n"
                  " 0 setgray\n"
                  " stroke\n"
                  " moveto\n"           /* home position */
                  " %d %d  rmoveto\n"   /* note1 position */
                  " currentpoint\n"
                  " 3 index show\n"     /* show first note */
                  " moveto\n"           /* note1 position */
                  " 0 %d rmoveto\n"     /* note2 position */
                  " show\n"             /* show second note */
                  " pop\n"
                  " stroke\n"
                  "}def %c QBox\n",
                  qBoxHigh,qBoxWidth,qBoxHigh,qBoxWidth,
                  noteOffset,noteOffset,
                  fontSize,'%'

            );

         /* print break box */

         /* param stack :  pos line */
         fprintf (OutStream, "/BBox\n"
                  "{\n"
                  " newpath\n"
                  " LW2\n"
                  " ROW exch POS exch\n" /* calculat position */
                  " moveto\n"
                  " 0 %d rlineto\n"     /* draw box */
                  " %d 0 rlineto\n"
                  " 0 -%d rlineto\n"
                  " -%d 0  rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  1 setgray\n"
                  "  fill\n"
                  " grestore\n"
                  " 0.5 setlinewidth\n"
                  " 0 setgray\n"
                  " stroke\n"
                  "}def %c BBox\n",
                  bBoxHigh,bBoxWidth,bBoxHigh,bBoxWidth,'%'

            );

         /* Draw base line  psStack : Line */

         fprintf (OutStream, "/BLine\n"
                  "{\n"
                  " newpath\n"
                  " ROW dup %d exch moveto\n"
                  " %d exch lineto\n"
                  " closepath\n"
                  " stroke\n"
                  "}def %c BLine\n",
                  LeftMargin,
                  SizeX - RightMargin,
                  '%'
            );

         /* Init new page */

         fprintf (OutStream, "/InitPage\n"
                  "{\n"
                  " newpath\n"
                  " 0.5 setlinewidth\n"
                  " 0 setgray\n"
                  " 0 1 %d \n"  /* FOR loop */
                  " {\n"
                  "   dup %d mod %d eq {pop} { BLine } ifelse\n"
                  " }\n"
                  " for\n"      /* End of FOR loop */
                  " closepath\n"
                  " stroke\n"
                  "}def %c InitPage\n",
                  (BlocksPerPage) * (LinesPerBlock+1) -1,
                  LinesPerBlock+1,LinesPerBlock,
                  '%'
            );

         /* print frame */
         /* param stack : xmin ymin xmax ymax */
         fprintf (OutStream, "/Frame\n"
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
                  "}def %c Frame\n", '%'
            );


         /* print oktaven line */

         /* param stack : row */
         fprintf (OutStream, "/OLB\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"          /* y-position */
                  " %d exch moveto\n"
                  " %d 0 rlineto\n"        /* first pos */
                  " stroke\n"
                  "}def %c OLB\n",
                  oLineOffset,
                  LeftMargin,MarginSpace,
                  '%'
            );

         /* param stack : Pos row */
         fprintf (OutStream, "/OLM\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS exch moveto\n"
                  " %d 0 rlineto\n"     /* end position */
                  " stroke\n"
                  "}def %c OLM\n",
                  oLineOffset,
                  ColSpaceSize,
                  '%'
            );

         /* param stack : Pos row */
         fprintf (OutStream, "/OLE\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS 1 index moveto\n"
                  " %d exch lineto\n"   /* end position */
                  " stroke\n"
                  "}def %c OLE\n",
                  oLineOffset,
                  SizeX - RightMargin,
                  '%'
            );
         /* param stack : Pos row */
         fprintf (OutStream, "/OL\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch\n"                /* Pos        */
                  " dup 0 le\n"
                  " { pop %d exch moveto %d 0 rlineto}\n"     /* first pos */
                  " { POS exch moveto %d 0 rlineto} ifelse\n" /* end position */
                  " stroke\n"
                  "}def %c OL\n",
                  oLineOffset,
                  LeftMargin,MarginSpace,
                  ColSpaceSize,
                  '%'
            );

         /* param stack : fromPos toPos row */
         fprintf (OutStream, "/OLine\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS 1 index\n"
                  " moveto\n"
                  " -%d %d rlineto\n"   /* draw right Pointer */
                  " 0 -%d rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  fill\n"
                  " grestore\n"
                  " exch POS %d add exch\n"
                  " lineto\n"           /* end of first box position */
                  " currentpoint\n"
                  " stroke\n"
                  " newpath\n"
                  " moveto\n"
                  " %d %d rlineto\n"    /* draw left Pointer */
                  " 0 -%d rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  fill\n"
                  " grestore\n"
                  " stroke\n"
                  "}def %c OLine\n",
                  oLineOffset,
                  pointerWidth,pointerHigh / 2,
                  pointerHigh,
                  oBoxWidth,
                  pointerWidth,pointerHigh / 2,
                  pointerHigh,
                  '%'
            );

         /* print quinten line */

         /* param stack : row */
         fprintf (OutStream, "/QLB\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"          /* y-position */
                  " %d exch moveto\n"
                  " %d 0 rlineto\n"        /* first pos */
                  " stroke\n"
                  "}def %c QLB\n",
                  qLineOffset,
                  LeftMargin,MarginSpace,
                  '%'
            );

         /* param stack : Pos row */
         fprintf (OutStream, "/QLM\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS exch moveto\n"
                  " %d 0 rlineto\n"     /* end position */
                  " stroke\n"
                  "}def %c QLM\n",
                  qLineOffset,
                  ColSpaceSize,
                  '%'
            );

         /* param stack : Pos row */
         fprintf (OutStream, "/QLE\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS 1 index moveto\n"
                  " %d exch lineto\n"   /* end position */
                  " stroke\n"
                  "}def %c QLE\n",
                  qLineOffset,
                  SizeX - RightMargin,
                  '%'
            );



         /* param stack : fromPos toPos row */
         fprintf (OutStream, "/QLine\n"
                  "{\n"
                  " newpath\n"
                //  " 0.5 setlinewidth\n"
                  " ROW %d add\n"       /* y-position */
                  " exch POS 1 index\n"
                  " moveto\n"
                  " -%d %d rlineto\n"   /* draw right Pointer */
                  " 0 -%d rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  fill\n"
                  " grestore\n"
                  " exch POS %d add exch\n"
                  " lineto\n"           /* end of first box position */
                  " currentpoint\n"
                  " stroke\n"
                  " newpath\n"
                  " moveto\n"
                  " %d %d rlineto\n"    /* draw left Pointer */
                  " 0 -%d rlineto\n"
                  " closepath\n"
                  " gsave\n"
                  "  fill\n"
                  " grestore\n"
                  " stroke\n"
                  "}def %c QLine\n",
                  qLineOffset,
                  pointerWidth,pointerHigh / 2,
                  pointerHigh,
                  qBoxWidth,
                  pointerWidth,pointerHigh / 2,
                  pointerHigh,
                  '%'
            );



         fprintf (OutStream, "/Text\n"
                  "{\n"
                  " /%s findfont\n"
                  " %d scalefont\n"
                  " setfont\n"
                  " 0 0 0 setrgbcolor\n"
                  "}def %c Text\n",
                  GS_FONT_NAME, fontSize ,'%'
            );
   return 0;
}

int MvpPs::PsMainBegin (void)
{
      // Print INFO-Page
      
	  PsPageData.NewPage( OutStream );

      fprintf (OutStream,
		          "%%c--- Info Page ---\n"
                  " gsave\n"
                  " %d %d translate\n"     /* 1 */
                  " %lf %lf scale\n"       /* 2 */
                  "  InfoText\n"
                  " grestore\n"
                  "showpage\n",
          /* 1 */ PsPageData.Page.TransX(), PsPageData.Page.TransY(),
          /* 2 */ PsPageData.Page.ScaleX(), PsPageData.Page.ScaleY()      
               );
                  
   fprintf (OutStream, "%c-------- MVP Output ----------\n"
                       ,'%'
           );
   FirstPage();
   return (0);
}

int MvpPs::PsMainEnd (void)
{
   fprintf (OutStream, "grestore\n"
                       "/#copies 1 def\n"
                       "showpage\n"
           );

   return (0);
}
int MvpPs::PsOutputBegin( int ColorOutputFlag, FILE * InfoStream )
{
   PsHeader( ColorOutputFlag);
   PsInfoPage( OutStream, InfoStream );
   PsMainBegin();

   return 0;
}

int MvpPs::PsOutputEnd( void )
{
   PsMainEnd();

   return 0;
}


/*-------------------------------------------------------------------
---------------------------------------------------------------------
*/

/* Macro to get a random integer within a specified range */
#define getrandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))


int MvpPs::MvpTestData( void )
{
MvpEntry mvpEntry;
int i, line;


      for( i=0; i < 1000; i++ )
      {
        for( line=0; line < LinesPerBlock; line++ )
        {
         mvpEntry.SetCh1Note( getrandom(0,127) );
         mvpEntry.SetCh2Note( getrandom(0,127) );
         mvpEntry.SetOColor( getrandom(1,3) );
         mvpEntry.SetQColor( getrandom(1,3) );
         switch( getrandom(0,2) )
         {
         case 0:
                mvpEntry.SetStatus( MVP_OKTAVE );
                SetMvpEntry( line , &mvpEntry );
                break;
         case 1:
                mvpEntry.SetStatus( MVP_QUINTE );
                SetMvpEntry( line , &mvpEntry );
                break;
         case 2:
                mvpEntry.SetStatus(  MVP_BREAK );
                SetMvpEntry( line , &mvpEntry );
                break;
         default:
                break;
         }
        }

        NextPos();
      }

   return 0;
}
