/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                        26.06.96
							Vers.: 2.0						  02.02.97
    -------------------------------------------------------------------
    gs.cpp

Version 2.0  WIN32
-----------------------------------------------------------------------
*/                                       
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  
#include <direct.h>
#include <process.h>

#include "debug.h"                      
#include "rameau.h" 
#include "global.h"
#include "system.h"
#include "gs.h"

                      
gs GS;
 
 
 
gs::gs()  // Konstruktur
{

    // Read Gs-parameter-File        
    
    ExecStr = new char[80];
    ExecViewStr = new char[80];
    VideoStr = new char[80];
    PrinterStr = new char[80];
    GifStr = new char[80];             
    PcxStr = new char[80];
    TiffStr = new char[80];
    InitStr = new char[80];
    InitViewStr = new char[80];
    PrinterInitStr = new char[80];
    EndStr  = new char[80];
    PathStr = new char[MAX_PATH_LENGTH];
    PathViewStr = new char[MAX_PATH_LENGTH];
    GraphicPrinterStr =  new char[80];
    GraphicScreenStr =  new char[80];
    
    strcpy(ExecStr,"gs386 ");
    strcpy(ExecViewStr,"gsview ");
    strcpy(VideoStr,"-sDEVICE=vga ");
    strcpy(PrinterStr,"-sDEVICE=vga ");
    strcpy(GifStr,"-sDEVICE=gif8 ");
    strcpy(PcxStr,"-sDEVICE=tiffg3 ");
    strcpy(TiffStr,"-sDEVICE=pcx16 ");
    strcpy(InitStr,"-q ");
    strcpy(InitViewStr,"");
    strcpy(PrinterInitStr," -dNOPAUSE -dDISKFONTS -q -r150 ");
    strcpy(EndStr," -c quit");    
    strcpy(PathStr,"\\gs");           
    strcpy(GraphicPrinterStr, "-g841x595 " );
    strcpy(GraphicScreenStr, "-g1100x750 " );
    
    if( ReadParamFile() != 0 )
    {
        printf( "Error in GS-ParamFile\n" );
        getchar();
    }    
    GraphicFormat = GS_GRAPHIC_SCREEN;
    
    #ifdef GS_DEBUG                        
       DispParam();
    #endif 
} 

gs::~gs()
{
}

int gs::ReadParamFile()
{
FILE *GsParamFile;
char Buffer[256], *p,*pStart, *pEnd, *Entry;
int len;

    if( (GsParamFile = fopen( GS_PARAM_FILE_NAME, "r" )) != NULL )
    {  
       while( ReadConfigLine(GsParamFile, Buffer) > 0 )
       {  
          Entry = NULL;
          
          if( (p = strchr( Buffer, ' ' )) == NULL )
          {
             return -1;
          }
          else
          {
             *p = 0;             
          }
          if( strcmp( "GS_EXEC_STR", Buffer ) == 0 ) Entry = ExecStr;
          else
          if( strcmp( "GS_VIEW_EXEC_STR", Buffer ) == 0 ) Entry = ExecViewStr;
          else
          if( strcmp( "GS_VIDEO_STR", Buffer ) == 0 ) Entry = VideoStr;
          else
          if( strcmp( "GS_PRINTER_STR", Buffer ) == 0 ) Entry = PrinterStr;
          else
          if( strcmp( "GS_GIF_STR", Buffer ) == 0 ) Entry = GifStr;
          else
          if( strcmp( "GS_PCX_STR", Buffer ) == 0 ) Entry = PcxStr;
          else
          if( strcmp( "GS_TIFF_STR", Buffer ) == 0 ) Entry = TiffStr;
          else
          if( strcmp( "GS_INIT_STR", Buffer ) == 0 ) Entry = InitStr;
          else
          if( strcmp( "GS_INIT_VIEW_STR", Buffer ) == 0 ) Entry = InitViewStr;
          else
          if( strcmp( "GS_PRINTER_INIT_STR", Buffer ) == 0 ) Entry = PrinterInitStr;
          else
          if( strcmp( "GS_END_STR", Buffer ) == 0 ) Entry = EndStr;
          else
          if( strcmp( "GS_PATH_STR", Buffer ) == 0 ) Entry = PathStr;
          else
          if( strcmp( "GS_VIEW_PATH_STR", Buffer ) == 0 ) Entry = PathViewStr;
          else
          if( strcmp( "GS_GRAPHIC_PRINTER_STR", Buffer ) == 0 ) Entry = GraphicPrinterStr;
          else
          if( strcmp( "GS_GRAPHIC_SCREEN_STR", Buffer ) == 0 ) Entry = GraphicScreenStr;
          else
          { 
             return -2;
          }
          
          // Copy Gs-Param-String
          
          if( Entry != NULL ) // find a legal parameater             
          {
             if( (pStart = strchr( p+1, '"' )) != NULL )
             {  
                pStart++;
                if( (pEnd = strchr( pStart, '"' )) == NULL )
                {
                   return -3;
                }
                else            
                {
                   *pEnd = 0;
                   len = strlen( pStart );
                   if( len >= 80 ) return -4;;
                   strcpy( Entry, pStart );
                }   
             }
          }
       }
       fclose( GsParamFile ); 
    }
    return 0;  
}
int gs::ReadConfigLine( FILE *GsParamFile, char *Buffer )
{
    int i=0, ch;
    
       // get parameter from file
       
       // read line from input file
       
       while( i < 256 )
       {
          switch( ch  = fgetc( GsParamFile ) )
          {
             case EOF : return -1;
                        break;
             case '\n': Buffer[i]=0;
                        return i;
                        break;
             case '\r': 
                        break;
             default  :
                        Buffer[i++] = ch; 
                        break;
          }              
       }                                        
       return -2;
} 
void gs::GsToScreen( char *FileName )
{   
    Init();
    Video();
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,FileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    End();   
    
    CallGs();    
}
void gs::GsToGsView( char *FileName )
{   
	ExecView();
	strcat( Str, " " );
    InitView();
#ifdef WIN32
	strcat( Str, "\"" );
#endif
	strcat(Str,FileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    
    ToGsViewDir();
	printf(Str); getchar();
#ifdef WIN32
	char *args[4];

	args[0] = "gsview32";
	args[1] = FileName;
	
	// _spawnlp( _P_WAIT, args[0], args[0], args[1], NULL );
	_spawnlp( _P_NOWAIT, args[0], args[0], args[1], NULL );

#else
    DoSystemCall( Str );
#endif
    
    // Back To Rameau
    ToRameauDir();
}
void gs::GsToGif(char *InFileName, char *OutFileName )
{
    // build GS-String
    Init();      
    switch( GraphicFormat )
    {
       case GS_GRAPHIC_SCREEN : GraphicScreen();
                                break;
       case GS_GRAPHIC_PRINTER: GraphicPrinter();
                                break;
    }                            
    Gif();
    strcat(Str," -sOutputFile=" );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,OutFileName );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str," " );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,InFileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    End();
    
    CallGs();    
}
void gs::GsToTiff(char *InFileName, char *OutFileName )
{
    // build GS-String
    Init();      
    switch( GraphicFormat )
    {
       case GS_GRAPHIC_SCREEN : GraphicScreen();
                                break;
       case GS_GRAPHIC_PRINTER: GraphicPrinter();
                                break;
    }                            
    Tiff();
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str," -sOutputFile=" );
    strcat(Str,OutFileName );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str," " );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,InFileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    End();
        
    CallGs();    
}
void gs::GsToPcx(char *InFileName, char *OutFileName )
{
    // build GS-String
    Init();      
    switch( GraphicFormat )
    {
       case GS_GRAPHIC_SCREEN : GraphicScreen();
                                break;
       case GS_GRAPHIC_PRINTER: GraphicPrinter();
                                break;
    }                            
    Pcx();
    strcat(Str," -sOutputFile=" );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,OutFileName );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str," " );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str,InFileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    End();
        
    CallGs();    
}
void gs::GsToPrinter(char *InFileName, char *OutFileName )
{
    Init();                              
    PrinterInit();
    Printer();
    strcat(Str," -sOutputFile=" );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str, OutFileName );
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str, " ");
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    strcat(Str, InFileName);
#ifdef WIN32
	strcat( Str, "\"" );
#endif
    End();
    #ifdef GS_DEBUG_PRINT
    printf( "\n%s", Str );
    getchar();
    GsToScreen( InFileName );
    #else
    CallGs();
    #endif    
}                                       

int gs::CallGs()
{ 
FILE *stream;                   

    #ifdef GS_DEBUG_CALL
       printf( "\n%s", Str );
       getchar();
       return 0;
    #endif

    // Write GS-String to file
    
    ToGsDir();
    if( (stream=fopen( "rameau.par","w")) == NULL )
    {
        // Back To Rameau
        ToRameauDir();
        return -1;
    }     
    fprintf( stream,"%s", Str );
    fclose( stream );
    Exec();
    strcat(Str, " @Rameau.par >Rameau.Nul" );
    DoSystemCall( Str );                  
    
#ifndef WIN32
    if( !DebugFlag )
    {
       remove( "Rameau.par" );
       remove( "Rameau.Nul" );
    }
#endif
    // Back To Rameau
    ToRameauDir();
    
    return 0;
}                       
int gs::SetPrinterName( char *str )
{   
int len;
    
    
    delete PrinterStr;
    len = strlen(str);
    // "-sDEVICE=" len + \0 = 10
    if( (PrinterStr = new char[len+10]) == NULL )
    {
        return 1;
    }
    else    
    {
       strcpy( PrinterStr, "-sDEVICE=" );
       strcat( PrinterStr, str );
    }   
    return 0;
}
int gs::ToGsDir()
{
    return chdir( PathStr );
}
int gs::ToGsViewDir()
{
    return chdir( PathViewStr );
}

#ifdef GS_DEBUG                        
void  gs::DispParam()
{   
    printf( "GS-Parameter :\n\n");
    printf( "ExecStr %s\n",ExecStr );
    printf( "ViewExecStr %s\n",ExecViewStr );
    printf( "VideoStr %s\n",VideoStr );
    printf( "PrinterStr %s\n",PrinterStr );
    printf( "GifStr %s\n",GifStr );
    printf( "PcxStr %s\n",PcxStr );
    printf( "TiffStr %s\n",TiffStr );
    printf( "InitStr %s\n",InitStr );
    printf( "InitViewStr %s\n",InitViewStr );
    printf( "PrinterInitStr %s\n",PrinterInitStr );
    printf( "EndStr %s\n",EndStr );
    printf( "PathStr %s\n",PathStr );
    printf( "PathViewStr %s\n",PathViewStr );
    printf( "GraphicPrinterStr %s\n",GraphicPrinterStr );
    printf( "GraphicScreenStr %s\n",GraphicScreenStr );
    
    getchar();
}                        
#endif
