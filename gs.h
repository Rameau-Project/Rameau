/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                        26.06.96
    -------------------------------------------------------------------
    gs.h
-----------------------------------------------------------------------
*/

#ifndef _GS_H_
#define _GS_H_

#include <string.h>                            

#include "debug.h"

#define GS_PARAM_FILE_NAME "bin\\rameau.gsp"

#define GS_EXEC_STR			0
#define GS_VIEW_EXEC_STR    13
#define GS_VIDEO_STR		1
#define GS_PRINTER_STR		2
#define GS_GIF_STR			3             
#define GS_PCX_STR			4
#define GS_TIFF_STR			5
#define GS_INIT_STR			6
#define GS_VIEW_INIT_STR	14
#define GS_PRINTER_INIT_STR 7
#define GS_END_STR			8 
#define GS_PATH				9 
#define GS_VIEW_PATH		12 
#define GS_GRAPHIC_PRINTER	10
#define GS_GRAPHIC_SCREEN	11

class gs
{
public:

     gs();
     ~gs();
void GsToScreen( char *FileName );
void GsToGsView( char *FileName );
void GsToPrinter(char *InFileName, char *OutFileName );
void GsToGif(char *InFileName, char *OutFileName );
void GsToPcx(char *InFileName, char *OutFileName );
void GsToTiff(char *InFileName, char *OutFileName );
int  SetPrinterName( char *str );
int  SetGraphicInput( int mode ) { GraphicFormat = mode; return 0; };

private:

char Str[512]; // Command Sting
                    

int     CallGs();
int     ReadParamFile();
int     ReadConfigLine( FILE *GsParamFile, char *Buffer );

void    Exec()   {strcpy(Str,ExecStr);}
void    ExecView(){strcpy(Str,ExecViewStr);}
void    Init()   {strcpy(Str,InitStr);}
void    InitView(){strcat(Str,InitViewStr);}
void    Video()  {strcat(Str,VideoStr);}
void    Printer(){strcat(Str,PrinterStr);}
void    Gif()    {strcat(Str,GifStr);}
void    Pcx()    {strcat(Str,PcxStr);}
void    Tiff()   {strcat(Str,TiffStr);}
void    GraphicPrinter() {strcat(Str,GraphicPrinterStr);}
void    GraphicScreen()  {strcat(Str,GraphicScreenStr);}
void    PrinterInit()    {strcat(Str,PrinterInitStr);}
void    End()    {strcat(Str,EndStr);}

int     ToGsDir(); 
int     ToGsViewDir(); 


char *ExecStr;
char *ExecViewStr;
char *VideoStr;
char *PrinterStr;
char *GifStr;             
char *PcxStr;
char *TiffStr;
char *GraphicPrinterStr;
char *GraphicScreenStr;
char *InitStr;
char *InitViewStr;
char *PrinterInitStr;
char *EndStr;            
char *PathStr;   
char *PathViewStr;   

int  GraphicFormat;
     
#ifdef GS_DEBUG                        
void    DispParam();                        
#endif
};
             
             
extern gs GS;

#endif /* _GS_H_ */