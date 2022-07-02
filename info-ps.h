/*------------------------------------------------------------------------------
        Dieter Neubacher                                                09.07.96
								Vers.: 2.0                              28.01.97
        ------------------------------------------------------------------------
        info-ps.h
        
        Print Rameau Info Page

		Vers.: 2.0    WIN32, Ghostview
---------------------------------------------------------------------------------
*/
                 
#ifndef _INFO_PS_H_                 
#define _INFO_PS_H_
                 


enum PaperType
{
	Letter,		 //612x792
    Tabloid,	 //792x1224
    Ledger,		 //1224x792
	Legal,		 //612x1008
	Statement,	 //396x612
	Executive,	 //540x720
	A3,		     //842x1190
	A4,		     //595x842
	A5,		     //420x595
	B4,		     //729x1032
	B5,		     //516x729
	Envelope,	 //???x???
	Folio,		 //612x936
	Quarto,		 //610x780
}; 

enum PaperOrientation { Portrait, Landscape };

#define POINTS_PER_CM 28.346456
#define PS_FONT_SIZE      14
#define PS_INFO_PRINTER_FONT_SIZE 14
#define PS_INFO_SCREEN_FONT_SIZE  24

#define PsTextYPos 300


#define AnalysisNo            0
#define Date                  1
#define Title                 2
#define Composer              3
#define Opus                  4
#define Year                  5
#define Editor                6
#define Publisher             7
#define Movement              8
#define Key                   9
#define TimeSignature        10
#define BarNodsfrom          11
#define BarNodsto            12
#define OffbeatValue         13
#define MinimalRhythmValue   14
#define InputBy              15

#define INFO_FIELD_COUNT     16

typedef struct
{
   int  x;
   int  y;
   int  input_size;
   char name[80];
   char text[80];

}PS_INFO_FIELD;


extern PS_INFO_FIELD PsInfoTxt[];


/*---------------------------*/
/* function definitions      */
/*---------------------------*/

int disp_error_message( char *str );
int GlobalPs( FILE *stream );
int GetInfo( FILE *InfoStream);

int PsInfoPage(FILE * stream, FILE * info_stream);

/*----------------------------*/
/* Postscript Parameter       */
/*----------------------------*/

#define GS_FONT_NAME        "Helvetica"
#define GS_ITALIC_FONT_NAME "Times-Italic"

#define PRINTER_SCALE_MODE  1
#define SCREEN_SCALE_MODE   2
#define GRAPHIC_SCALE_MODE  3

class scaling
{
public  :
       
       scaling();
       void Set(int SizeX,int SizeY,int TransX,int TransY,double ScaleX,double ScaleY );
       void InitScaling( char *FileName );
              
double ScaleX(){return scaleX;};       
double ScaleY(){return scaleY;};       
int    TransX(){return transX;};
int    TransY(){return transY;};
int    SizeX(){return sizeX;};
int    SizeY(){return sizeY;};
       
void   SetScaleX(double val ){ scaleX = val;};       
void   SetScaleY(double val){ scaleY=val;};       
void   SetTransX(int val){ transX=val;};
void   SetTransY(int val){ transY=val;};
void   SetSizeX(int val){ sizeX=val;};
void   SetSizeY(int val){ sizeY=val;};


       
private :

double scaleX,scaleY;
int    transX,transY;
int    sizeX,sizeY;                        
};    
    
class pspagedata
{
public :

      pspagedata();
      pspagedata( enum PaperType type );
      pspagedata( enum PaperType type, enum PaperOrientation mode );
      void SetPaperType( enum PaperType type, enum PaperOrientation mode );
      void Set( int xsize, int ysize );
      void SetMergin( int size );
      void ScalingMode( int mode ); 
      void GetScalingParam( char *Name );   //Filename whithout ext.
int   GetScalingMode( void ) { return scalingMode; }
	  void NewPage(FILE *stream );
int   PrintFrame(FILE * stream, int mergin );

      
int   Top() {return Ymax; };
int   Bot() {return Ymin; };
int   LMergin() {return Xmin; };
int   RMergin() {return Xmax; };
int   XSize() {return Xmax-Xmin; };
int   YSize() {return Ymax-Ymin; };
int   TotalSizeX(){return PageXSize;};
int   TotalSizeY(){return PageYSize;};
int   PsDSCHeader( FILE *stream );
enum  PaperOrientation GetOrientation() { return Orientation; };                                 


scaling Page;

private :

      void SetOrientation( enum PaperOrientation mode );
      void SetPaperType( enum PaperType type );

int  PageNumber;
int  Mergin;        
int  PageXSize, PageYSize;
int  Xmin;
int  Xmax;
int  Ymin;
int  Ymax;                    
enum PaperType Type;
enum PaperOrientation Orientation;

//---- Parameter ( from Programflags or Configfile )

int scalingMode;                                 
scaling Printer, Screen, Graphic;

};          

extern pspagedata PsPageData;
                
#endif // __INFO_PS_H__ 