/*---------------------------------------------------------------------------
    Dieter Neubacher 
              
                   Vers. 1.0            06.12.93
                   Vers. 1.1            19.07.94
                   Vers. 1.2            29.09.94 
                   Vers. 1.3            28.01.96
                   Vers. 1.4            22.06.96  
				   Vers. 2.0            01.02.97
    -------------------------------------------------------------------------

    1.1    convertierung nach C++               
    1.3    call batch-files
    1.4    GS-Fuktionen (Screen,Printer,Tiff,Gif,Pcx)
           Neue Edit Funktionen in C++
           Neue Output Funktionen : Screen.cpp  
	2.0    WIN32	   
                  
    menu.c  Rameau menu
-----------------------------------------------------------------------------
*/

#ifdef WIN32
#include <windows.h>
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>

// --------------------------

#include "rameau.h"

#ifdef __MS_DOS__ 
#include <dos.h>
#include <conio.h>
#include <direct.h>
#else /* __MS_DOS__ */
#define getch(a) getchar()
#endif /* __MS_DOS__ */
                        
#include "debug.h"                        
#include "global.h"
#include "screen.h"
#include "system.h"
#include "gs.h"
#include "edi.h"         
#include "version.h"

#define ESC    ('\x1b')
#define RET    ('\r')

// #define DEBUG_SCREEN

/*-----------------*/
/*   Ú   \x218     */
/*   ³   \x179     */
/*   À   \x192     */
/*   ¿   \x191     */
/*   Ù   \x217     */
/*   Ä   \x196     */
/*-----------------*/


/*---------------------------*/
/*                           */
/*---------------------------*/

#define IN_ACTIV 0x00
#define ACTIV    0x01

typedef struct field
{
   int     x;
   int     y;
   int     option_flag;
   int     status_flag;
   int     key;
   char    text[40];
}
FIELD;
/*---------------------------*/
/*                           */
/*---------------------------*/




#define INFO_MAX_EDI 16

#define EDI_DISP         0x01
#define EDI_EDIT         0x02
#define EDI_INIT         0x03
#define EDI_READ_FILE    0x04
#define EDI_WRITE_FILE   0x05
#define EDI_CLOSE        0x06



#define IndexAnalysisNo          0
#define IndexDate                1
#define IndexTitle               2
#define IndexComposer            3
#define IndexOpus                4
#define IndexYear                5
#define IndexEditor              6
#define IndexPublisher           7
#define IndexMovement            8
#define IndexKey                 9
#define IndexTimeSignature      10
#define IndexBarNodsfrom        11
#define IndexBarNodsto          12
#define IndexOffbeatValue       13
#define IndexMinimalRhythmValue 14
#define IndexInputBy            15

#define InfoMaxIndex            15
                                         
char InfoName[InfoMaxIndex+1][25] =
{                                         
   "#AnalysisNo#",
   "#Date#",
   "#Title#",
   "#Composer#",
   "#Opus#",
   "#Year#",
   "#Editor#",
   "#Publisher#",
   "#Movement#",
   "#Key#",
   "#TimeSignature#",
   "#BarNodsfrom#",
   "#BarNodsto#",
   "#OffbeatValue#",
   "#MinimalRhythmValue#",
   "#InputBy#"
};
                                         
/*---------------------------*/
/* global variables          */
/*---------------------------*/


REMEAU_FLAGS input_flags;
REMEAU_FLAGS rameau_flags;

char    rameau_command[MAX_COMMAND_LENGTH];

#ifdef WIN32
#define MAX_NAME_LENGTH MAX_PATH_LENGTH
#else
#define MAX_NAME_LENGTH 8
#endif
edi    RameauPath(MAX_PATH_LENGTH, 1, 1, 1, 1, 1);
edi    InputName(MAX_NAME_LENGTH, 1, 31, 31 + 24, 3, 3);
edi    WorkPath(MAX_PATH_LENGTH, 1, 26, 68, 8, 8);
edi    ExternCommand(MAX_COMMAND_LENGTH, 1, 24, 76, 6, 6);
edi    PcsCode(MAX_COMMAND_LENGTH, 1, 24, 66, 9, 9);


edi    *Info[INFO_MAX_EDI]; 


char    user_company[80];

/*---------------------------*/
/* extern Editor             */
/*---------------------------*/
                               
int   EditorFlag = 0;
char *EditorName;

/*---------------------------*/
/* function definitions      */
/*---------------------------*/

#define INPUT_MODE_RET       0
#define INPUT_MODE_NO_RET    1

int menu_input(FIELD entry[], int entry_count, int mode);
int menu_disp_flags (FIELD entry[], int entry_count, int flag);
int menu_disp(char *menu[], int lines);
int DispError (char *str);
int ExecRameau(void);
int BuildName( void );
int BuildWorkPath( void );
int BuildNamePath (void);
int write_info (char *filename);
int read_info (char *filename);
int edit_info (int mode);
int read_config (void);
int write_config (void);
int PrintFile ( char *FileName );
int show_dir (void);
int DispAsciiFile (char *filename);
int DispFile();
int ConvertGraphic();
int not_implemented (void);
void RameauExit( int err );
int BuildInfoTempFile( char *InFileName, char *TmpFileName, int SpaceFlag );


/*---------------------------*/
/* Rameau Work FileName      */
/*---------------------------*/

class rwfn
{   
public :

    rwfn();
    ~rwfn();
    
void Ext( char *ext ); 
void Ext( int   ext ); 
char *FileName();

private :    

char *p;
};

rwfn::rwfn() 
{
    p = new char[MAX_COMMAND_LENGTH];
}
rwfn::~rwfn() 
{
    delete p;
}                                     

void rwfn::Ext( char *ext ) 
{   
int len;
    
    BuildNamePath();
    
    WorkPath.GetStr(p, MAX_COMMAND_LENGTH );
    len = strlen( p );
    InputName.GetStr(p + len,MAX_COMMAND_LENGTH - len );
    strcat(p, "." );
    strcat(p, ext );
}
void rwfn::Ext( int ext ) 
{   
int len, i, flag;
char Buffer[256];

    BuildNamePath(); 
    
    WorkPath.GetStr(p, MAX_COMMAND_LENGTH );
    len = strlen( p );
    
    switch( ext )
    {
       case GRAPHIC_GIF :    // this are multiple files
       case GRAPHIC_TIFF:
       case GRAPHIC_PCX :
                 
                 // insert the "%d" (PageNumber) definition
                 InputName.GetStr(Buffer,MAX_COMMAND_LENGTH - len );
                 
                 // overread leading blanks
                 flag = 0;
                 Buffer[6] = 0;
                 for( i=0; i<6; i++ )
                 {  
                    switch( Buffer[i] )
                    {  
                    
                       case 0: 
                               break;
                       case ' ' :
                               if( flag == 0 )
                                   break;
                               else    
                                  Buffer[i] = 0;
                               break;    
                       default :
                                flag = 1;
                                break;
                    }           
                 }
                 strcat(p, Buffer );    
                 strcat(p, "%02d" );  // GS-Parameter for multiple files  
                 break;       
       default :
                 InputName.GetStr(p + len,MAX_COMMAND_LENGTH - len );
                 break;
    }
    
    strcat(p, "." );
    SetRameauExt ( p, ext );
}

char *rwfn::FileName() 
{   
    return p;
}

rwfn RameauWorkFileName;

#define  SCREEN_BLUE_WHITE  1
#define  SCREEN_WHITE_BLACK 2

/*---------------------------*/
/*               */
/*---------------------------*/

char   *root_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* 00 */ "                                                                                ",
 /* 01 */ "                                                                                ",
 /* 02 */ "                                                                                ",
 /* 03 */ "                                                                                ",
 /* 04 */ "                                                                                ",
 /* 05 */ "                                                                                ",
 /* 06 */ "                                                                                ",
 /* 07 */ "                                                                                ",
 /* 08 */ "                                                                                ",
 /* 09 */ "                                                                                ",
 /* 10 */ "                                                                                ",
 /* 11 */ "                                                                                ",
 /* 12 */ "                                                                                ",
 /* 13 */ "                                                                                ",
 /* 14 */ "                                                                                ",
 /* 15 */ "                                                                                ",
 /* 16 */ "                                                                                ",
 /* 17 */ "                                                                                ",
 /* 18 */ "                                                                                ",
 /* 19 */ "        ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿        ",
 /* 20 */ "        ³  <A>bout  <I>nput  <O>utput  Op<t>ions  <P>rocedures  <Q>uit ³        ",
 /* 21 */ "        ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ        ",
 /* 22 */ "                                                                                ",
 /* 23 */ "                                                                                ",
 /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};



int root_input_count = 6;


FIELD   root_input[6] =
{
   {
      20, 20, 0, IN_ACTIV, (int) 'I', "<I>nput"
   }
   ,
   {
      29, 20, 0, IN_ACTIV, (int) 'O', "<O>utput"
   }
   ,
   {
      50, 20, 0, IN_ACTIV, (int) 'P', "<P>rocedures"
   }
   ,
   {
      39, 20, 0, IN_ACTIV, (int) 't', "Op<t>ions"
   }
   ,
   {
      11, 20, 0, IN_ACTIV, (int) 'A', "<A>bout"
   }
   ,
   {
      64, 20, 0, IN_ACTIV, (int) 'Q', "<Q>uit"
   }

};




char   *option_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* 00 */ "                                     OPTIONS                                    ",
 /* 01 */ "                                                                                ",
 /* 02 */ "                                                                                ",
 /* 03 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿          ",
 /* 04 */ "   Company/User          ³                                           ³          ",
 /* 05 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ          ",
 /* 06 */ "                                                                                ",
 /* 07 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿          ",
 /* 08 */ "   <W>ork Path           ³                                           ³          ",
 /* 09 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ          ",
 /* 10 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿       ",
 /* 11 */ "   <P>rinter             ³  <E>PSON <D>JET <C>DJET <L>JET2 LJET<3> <P>S ³       ",
 /* 12 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ       ",
 /* 13 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                 ",
 /* 14 */ "   <G>raphics            ³  <G>IF    <P>CX   <T>IFF   <N>ONE  ³                 ",
 /* 15 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                 ",
 /* 16 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                          ",
 /* 17 */ "   Por<t>                ³  LPT<1>         LPT<2>    ³                          ",
 /* 18 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                          ",
 /* 19 */ "                         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                          ",
 /* 20 */ "   <S>creen Color        ³  <W>hite        <B>lue    ³                          ",
 /* 21 */ "                         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                          ",
 /* 22 */ "                                                                                ",
 /* 23 */ "   <RETURN> to previous screen, save options                          <H>elp    ",
 /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};



int option_input_count = 7;


FIELD   option_input[] =
{

   {
      3, 8, 0, IN_ACTIV, 'W', "<W>ork Path"
   }
   ,
   {
      3, 11, 0, IN_ACTIV, 'p', "<P>rinter"
   }
   ,
   {
      3, 14, 0, IN_ACTIV, 'g', "<G>raphics"
   }
   ,
   {
      3, 17, 0, IN_ACTIV, 't', "Por<t>"
   }
   ,
   {
      3, 20, 0, IN_ACTIV, 'S', "<S>creen"
   }
   ,
   {
      3, 23, 0, IN_ACTIV, RET, "<RETURN>"
   }
   ,
   {
      70, 23, 0, IN_ACTIV, 'H', "<H>elp"
   }

};


int option_printer_input_count = 6;

FIELD   option_printer_input[] =
{

   {
      28, 11, PRINTER_EPSON, IN_ACTIV, 'E', "<E>PSON"
   }
   ,
   {
      36, 11, PRINTER_DJET , IN_ACTIV, 'D', "<D>JET"
   }
   ,
   {
      43, 11, PRINTER_CDJET, IN_ACTIV, 'C', "<C>DJET"
   }
   ,
   {
      51, 11, PRINTER_LJET2, IN_ACTIV, 'L', "<L>JET2"
   }
   ,
   {
      59, 11, PRINTER_LJET3, IN_ACTIV, '3', "LJET<3>"
   }
   ,
   {
      67, 11, PRINTER_PS   , IN_ACTIV, 'P', "<P>S"
   }
   ,
};

int     option_graphic_input_count = 4;

FIELD   option_graphic_input[] =
{

   {
      28, 14, GRAPHIC_GIF, IN_ACTIV, 'G', "<G>IF"
   }
   ,
   {
      37, 14, GRAPHIC_PCX, IN_ACTIV, 'P', "<P>CX"
   }
   ,
   {
      45, 14, GRAPHIC_TIFF, IN_ACTIV, 'T', "<T>IFF"
   }
   ,
   {
      54, 14, GRAPHIC_NONE, IN_ACTIV, 'N', "<N>ONE"
   }
   ,
};

int     option_port_input_count = 2;

FIELD   option_port_input[] =
{

   {
      28, 17, 0, IN_ACTIV, '1', "LPT<1>"
   }
   ,
   {
      43, 17, 1, IN_ACTIV, '2', "LPT<2>"
   }
   ,
};

int     option_color_input_count = 2;

FIELD   option_color_input[] =
{

   {
      28, 20, 2, IN_ACTIV, 'W', "<W>hite"
   }
   ,
   {
      43, 20, 1, IN_ACTIV, 'B', "<B>lue"
   }
   ,
};

char   *input_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* 00 */ "                                   DATA INPUT                                   ",
 /* 01 */ "                                                                                ",
 /* 02 */ "                              ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                       ",
 /* 03 */ "    Input File <N>ame:        ³                         ³   Show <D>irectory    ",
 /* 04 */ "                              ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                       ",
 /* 05 */ "                              ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿    ",
 /* 06 */ "    Input File <F>ormat:      ³ PM<X> <M>ID <F>DL <R>DL <V>PS <P>CS VP<L>  ³    ",
 /* 07 */ "                              ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    ",
 /* 08 */ "    File Heade<r>:                                                              ",
 /* 09 */ "    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿    ",
 /* 10 */ "    ³  Analysis No.:                                       Date:           ³    ",
 /* 11 */ "    ³  Title:                                                              ³    ",
 /* 12 */ "    ³  Composer:                                                           ³    ",
 /* 13 */ "    ³  Opus:                                               Year:           ³    ",
 /* 14 */ "    ³  Publisher:                        Editor:                           ³    ",
 /* 15 */ "    ³  Movement:                                                           ³    ",
 /* 16 */ "    ³  Key:                                      Time Signature:           ³    ",
 /* 17 */ "    ³  Bar Nos from:      to:                     Offbeat Value:           ³    ",
 /* 18 */ "    ³  Minimal Rhythm Value:                                               ³    ",
 /* 19 */ "    ³  Input by:                                                           ³    ",
 /* 20 */ "    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    ",
 /* 21 */ "                                                                                ",
 /* 22 */ "                                                                                ",
 /* 23 */ "    <RETURN> to previous screen, save options                         <H>elp    ",
 /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};
       
edi AnalysisNo(      6,  1, 21, 21+ 6, 10, 10 );
edi Date(            8,  1, 65, 65+ 8, 10, 10 );
edi Title(          59,  1, 14, 14+59, 11, 11 );
edi Composer(       56,  1, 17, 17+56, 12, 12 );
edi Opus(           44,  1 ,13, 13+44, 13, 13 );
edi Year(            8,  1, 65, 65+ 8, 13, 13 );
edi Editor(         24,  1, 49, 49+24, 14, 14 );
edi Publisher(      21,  1, 18, 18+21, 14, 14 );
edi Movement(       56,  1, 17, 17+56, 15, 15 );
edi Key(             8,  1, 12, 12+ 8, 16, 16 );
edi TimeSignature(   5,  1, 65, 65+ 5, 16, 16 );
edi BarNodsfrom(     4,  1, 21, 21+ 4, 17, 17 );
edi BarNodsto(       4,  1, 30, 30+ 4, 17, 17 );
edi OffbeatValue(    5,  1, 65, 65+ 5, 17, 17 );
edi MinimalRhythmValue(  5,  1, 29, 29+ 5, 18, 18 );
edi InputBy(        56,  1, 17, 17+56, 19, 19 );

int     input_input_count = 6;
FIELD   input_input[] =
{

   {
      15, 3, 0, IN_ACTIV, 'N', "<N>ame"
   }
   ,
   {
      60, 3, 0, IN_ACTIV, 'D', "Show <D>irectory"
   }
   ,
   {
      15, 6, 0, IN_ACTIV, 'F', "<F>ormat"
   }
   ,
   {
      4, 8, 0, IN_ACTIV, 'r', "File Heade<r>"
   }
   ,
   {
      4, 23, 0, IN_ACTIV, RET, "<RETURN>"
   }
   ,
   {
      70, 23, 0, IN_ACTIV, 'H', "<H>elp"
   }

};

int input_format_input_count = 7;

FIELD   input_format_input[] =
{

   {
      32, 6, PMX, IN_ACTIV, 'X', "PM<X>"
   }
   ,
   {
      38, 6, MID, IN_ACTIV, 'M', "<M>ID"
   }
   ,
   {
      44, 6, ATV, IN_ACTIV, 'T', "A<T>V"
   }
   ,
   {
      50, 6, RDL, IN_ACTIV, 'R', "<R>DL"
   }
   ,
   {
      56, 6, VPS, IN_ACTIV, 'V', "<V>PS"
   }
   ,
   {
      62, 6, PCS, IN_ACTIV, 'P', "<P>CS"
   }
   ,
   {
      68, 6, MVP, IN_ACTIV, 'L', "VP<L>"
   }
   ,
};


char   *output_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* 00 */ "                                   DATA OUTPUT                                  ",
 /* 01 */ "                                                                                ",
 /* 02 */ "   Output Data <F>ile:                                                          ",
 /* 03 */ "   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿        ",
 /* 04 */ "   ³ <R>DL                Reduced data listing                         ³        ",
 /* 05 */ "   ³ RD<A>                Reduced data analysis                        ³        ",
 /* 06 */ "   ³ A<T>V                Average tone value                           ³        ",
 /* 07 */ "   ³ <V>PS                Successive vertical pitch sets               ³        ",
 /* 08 */ "   ³ <P>CS                Pitch class set numbers                      ³        ",
 /* 09 */ "   ³ <M>AP                Scatter diagram for successive vps resp. pcs ³        ",
 /* 10 */ "   ³ VP<L>                Multiple voice pairs listing                 ³        ",
 /* 11 */ "   ³ VP<G>                Multiple voice pairs graphic                 ³        ",
 /* 12 */ "   ³ I<C>V                Intervall Class Vector                       ³        ",
 /* 13 */ "   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ        ",
 /* 14 */ "                   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿      ",
 /* 15 */ "   <V>oice number  ³                   <2>   <3>   <4>   <5>   <6>       ³      ",
 /* 16 */ "                   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ      ",
 /* 17 */ "                                                                                ",
 /* 18 */ "                   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿      ",
 /* 19 */ "   <M>ap Options   ³ Max. Cardinality        <3>   <4>   <5>   <6>       ³      ",
 /* 20 */ "                   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ      ",
 /* 21 */ "                                                                                ",
 /* 22 */ "                                                                                ",
 /* 23 */ "   <RETURN> to previous screen, save options                           <H>elp   ",
 /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};

int output_input_count = 5;

FIELD   output_input[] =
{

   {
      15, 2, 0, IN_ACTIV, 'F', "<F>ile"
   }
   ,
   {
      3, 15, 0, IN_ACTIV, 'V', "<V>oice"
   }
   ,
   {
      3, 19, 0, IN_ACTIV, 'M', "<M>ap"
   }
   ,
   {
      3, 23, 0, IN_ACTIV, RET, "<RETURN>"
   }
   ,
   {
      71, 23, 0, IN_ACTIV, 'H', "<H>elp"
   }
   ,
};


int output_file_input_count = 9;

FIELD   output_file_input[] =
{

   {
      5, 4, RDL, IN_ACTIV, 'R', "<R>DL"
   }
   ,
   {
      5, 5, RDA, IN_ACTIV, 'A', "RD<A>"
   }
   ,
   {
      5, 6, ATV, IN_ACTIV, 'T', "A<T>V"
   }
   ,
   {
      5, 7, VPS, IN_ACTIV, 'V', "<V>PS"
   }
   ,
   {
      5, 8, PCS, IN_ACTIV, 'P', "<P>CS"
   }
   ,
   {
      5, 9, MAP, IN_ACTIV, 'M', "<M>AP"
   }
   ,
   {
      5,10, MVP, IN_ACTIV, 'L', "VP<L>"
   }
   ,
   {
      5,11, MPS, IN_ACTIV, 'G', "VP<G>"
   }
   ,
   {
      5,12, ICV, IN_ACTIV, 'C', "I<C>V"
   }
   ,
};

int output_voice_input_count = 5;

FIELD   output_voice_input[] =
{

   {
      39, 15, 2, IN_ACTIV, '2', "<2>"
   }
   ,
   {
      45, 15, 3, IN_ACTIV, '3', "<3>"
   }
   ,
   {
      51, 15, 4, IN_ACTIV, '4', "<4>"
   }
   ,
   {
      57, 15, 5, IN_ACTIV, '5', "<5>"
   }
   ,
   {
      63, 15, 6, IN_ACTIV, '6', "<6>"
   }
   ,
};

int output_max_input_count = 4;

FIELD   output_max_input[] =
{

   {
      45, 19, 3, IN_ACTIV, '3', "<3>"
   }
   ,
   {
      51, 19, 4, IN_ACTIV, '4', "<4>"
   }
   ,
   {
      57, 19, 5, IN_ACTIV, '5', "<5>"
   }
   ,
   {
      63, 19, 6, IN_ACTIV, '6', "<6>"
   }
   ,
};


char   *procedures_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* 00 */ "                                   PROCEDURES                                   ",
 /* 01 */ "                                                                                ",
 /* 02 */ "                                                                                ",
 /* 03 */ "   <R>un Procedures according to Options                                        ",
 /* 04 */ "                                                                                ",
 /* 05 */ "                       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ",
 /* 06 */ "   Dos<S>hell Command  ³                                                     ³  ",
 /* 07 */ "                       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ",
 /* 08 */ "                       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ÚÄÄÄÄÄÄÄ¿  ",
 /* 09 */ "   Pcs <C>ode          ³                                           ³ ³       ³  ",
 /* 10 */ "                       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ ÀÄÄÄÄÄÄÄÙ  ",
 /* 11 */ "                       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ",
 /* 12 */ "   <D>isplay           ³I<c>V <I>NF <R>DL A<T>V <V>PS <P>CS <M>AP VP<L> VP<G>³  ",
 /* 13 */ "   [after procedures]  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ",
 /* 14 */ "                       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ",
 /* 15 */ "   <P>rint             ³     <I>NF <R>DL <F>DL <V>PS <P>CS       VP<L>       ³  ",
 /* 16 */ "   [after procedures]  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ",
 /* 17 */ "                       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿           ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ",
 /* 18 */ "   Or<i>entation       ³ <L>andscape <P>ortrait ³  <F>ormat ³ A<3> A<4> A<5> ³  ",                       
 /* 19 */ "                       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ           ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ",
 /* 20 */ "                                                                                ",
 /* 21 */ "   Pr<o>cess Protocol                                                           ",
 /* 22 */ "                                                                                ",
 /* 23 */ "   <RETURN> to previous screen                                         <H>elp   ",
 /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};

#define PCS_OUTPUT_X 70
#define PCS_OUTPUT_Y  9
char PcsOutputBuffer[11];

int procedures_input_count = 10;

FIELD   procedures_input[] =
{

   {
      3, 3, 0, IN_ACTIV, 'R', "<R>un"
   }
   ,
   {
      3, 6, 0, IN_ACTIV, 'S', "Dos<S>hell Command"
   }
   ,
   {
      3, 9, 0, IN_ACTIV, 'C', "Pcs <C>ode"
   }
   ,
   {
      3, 12, 0, IN_ACTIV, 'D', "<D>isplay"
   }
   ,
   {
      3, 15, 0, IN_ACTIV, 'P', "<P>rint"
   }
   ,
   {
      3, 18, 0, IN_ACTIV, 'i', "Or<i>entation"
   }
   ,
   {
     51, 18, 0, IN_ACTIV, 'F', "<F>ormat"
   }
   ,
   {
      3, 21, 0, IN_ACTIV, 'o', "Pr<o>cess Protocol"
   }
   ,
   {
      3, 23, 0, IN_ACTIV, RET, "<RETURN>"
   }
   ,
   {
      71, 23, 0, IN_ACTIV, 'H', "<H>elp"
   }

};

int procedures_display_count = 9;

FIELD   procedures_display[] =
{

   {
      24, 12, ICV, IN_ACTIV, 'C', "I<C>V"
   }
   ,
   {
      30, 12, INF, IN_ACTIV, 'I', "<I>NF"
   }
   ,
   {
      36, 12, RDL, IN_ACTIV, 'R', "<R>DL"
   }
   ,
   {
      42, 12, ATV, IN_ACTIV, 'T', "A<T>V"
   }
   ,
   {
      48, 12, VPS, IN_ACTIV, 'V', "<V>PS"
   }
   ,
   {
      54, 12, PCS, IN_ACTIV, 'P', "<P>CS"
   }
   ,
   {
      60, 12, MAP, IN_ACTIV, 'M', "<M>AP"
   }
   ,
   {
      66, 12, MVP, IN_ACTIV, 'L', "VP<L>"
   }
   ,
   {
      72, 12, MPS, IN_ACTIV, 'G', "VP<G>"
   }

};

int procedures_printfile_count = 6;

FIELD   procedures_printfile[] =
{

   {
      29, 15, INF, IN_ACTIV, 'I', "<I>NF"
   }
   ,
   {
      35, 15, RDL, IN_ACTIV, 'R', "<R>DL"
   }
   ,
   {
      41, 15, ATV, IN_ACTIV, 'T', "A<T>V"
   }
   ,
   {
      47, 15, VPS, IN_ACTIV, 'V', "<V>PS"
   }
   ,
   {
      53, 15, PCS, IN_ACTIV, 'P', "<P>CS"
   }
   ,
   {
      65, 15, MVP, IN_ACTIV, 'L', "VP<L>"
   }

};

int ProcedureGraphicOrientationCount = 2;

FIELD   ProcedureGraphicOrientation[] =
{

   {
      25, 18, LANDSCAPE, IN_ACTIV, 'L', "<L>andscape"
   }
   ,
   {
      37, 18, PORTRAIT, IN_ACTIV, 'P', "<P>ortrait"
   }
}; 

int ProcedureGraphicFormatCount = 3;

FIELD   ProcedureGraphicFormat[] =
{

   {
      62, 18, 3, IN_ACTIV, '3', "A<3>"
   }
   ,
   {
      67, 18, 4, IN_ACTIV, '4', "A<4>"
   }
   ,
   {
      72, 18, 5, IN_ACTIV, '5', "A<5>"
   }
};


char   *processing_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
   /* 00 */ "                                                                                ",
   /* 01 */ "                                                                                ",
   /* 02 */ "                                                                                ",
   /* 03 */ "                                                                                ",
   /* 04 */ "          ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                 ",
   /* 05 */ "          ³                                                   ³                 ",
   /* 06 */ "          ³                  Processing data                  ³                 ",
   /* 07 */ "          ³                                                   ³                 ",
   /* 08 */ "          ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                 ",
   /* 09 */ "                                                                                ",
   /* 10 */ "                                                                                ",
   /* 11 */ "                                                                                ",
   /* 12 */ "                                                                                ",
   /* 13 */ "                                                                                ",
   /* 14 */ "                                                                                ",
   /* 15 */ "                                                                                ",
   /* 16 */ "                                                                                ",
   /* 17 */ "                                                                                ",
   /* 18 */ "                                                                                ",
   /* 19 */ "                                                                                ",
   /* 20 */ "                                                                                ",
   /* 21 */ "                                                                                ",
   /* 22 */ "                                                                                ",
   /* 23 */ "                                                                                ",
   /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};

char   *about_menu[25] =
{
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
   /* 00 */ "                                                                                ",
   /* 01 */ "                                                                                ",
   /* 02 */ "                                                                                ",
   /* 03 */ "                     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                      ",
   /* 04 */ "                     ³                                   ³                      ",
   /* 05 */ "                     ³              RAMEAU               ³                      ",
   /* 06 */ "                     ³                                   ³                      ",
   /* 07 */ "                     ³      Music Analysis Software      ³                      ",
   /* 08 */ "                     ³                                   ³                      ",
   /* 09 */ "                     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                      ",
   /* 10 */ "                                                                                ",
   /* 11 */ "                                                                                ",
   /* 12 */ "                        by  A. Rambold  &  G. Rambold                           ",
   /* 13 */ "                                                                                ",
   /* 14 */ "                          Programmer:  D. Neubacher                             ",
   /* 15 */ "                                                                                ",
   /* 16 */ "                                                                                ",
   /* 17 */ "                                                                                ",
   /* 18 */ "                                                                                ",
   /* 19 */ "                                                                                ",
   /* 20 */ "                                                                                ",
   /* 21 */ "                                                                                ",
   /* 22 */ "  Copyright (c) 1994-97 by ARION Verlag, Muenchen                               ",
   /* 23 */ "  All rights reserved                                                           ",
   /* 24 */ "                                                                                "
 /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /*                  1         2         3         4         5         6         7          */
 /*        01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
};


/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 

int test( void )
{        
static int call = 1; 

int i, *list[100];

    // char   out[80];
    // sprintf( out, "test()--- begin ----" );
    // Screen.DispError (out );
    for(i=0; i<100; i++)
       list[i] = (int *)malloc(1000+7*call);
    for(i=0; i<100; i++)
    {
       if( list[i] != NULL )
          free( list[i] );
       else
          Screen.DispError ( "---- test -----"  );
    }
    call++;
    // sprintf( out, "test()--- call %d ----", call );
    // Screen.DispError ( out );
    return 0;
}

/*--------------------------------------------------------------------------
    disp funktion not implemented
----------------------------------------------------------------------------
*/

int not_implemented (void)
{
   Screen.DispError ("Function not implemented in this version");
   return 0;
}
/*--------------------------------------------------------------------------
    disp help file
----------------------------------------------------------------------------
*/

int DispAsciiFile (char *filename)
{
   int     i, j, ch, end_loop = 0;
   char    buffer[TMP_BUFFER_LENGTH];
   FILE   *stream;

   Screen.Normal();
   Screen.Clear();
   if ((stream = fopen (filename, "r")) == NULL)
   {
      sprintf (buffer, "!!! file %s open error !!!", filename);
      Screen.DispError (buffer);
      return (-1);
   }

   /* disp help massage */
   do
   {
      for (i = 0; i < Screen.GetMaxLines(); i++)
      {
         if (end_loop == 1)
         {
             memset (buffer, ' ', Screen.GetMaxCols());
             buffer[Screen.GetMaxCols()+1] = 0;
         }
         else
         for (j = 0; j < Screen.GetMaxCols(); j++)
         {
           switch (ch = fgetc (stream))
           {
              case '\r':
                        break;
              case EOF:
                        end_loop = 1;
              case '\n':
                        memset (buffer + j, ' ', Screen.GetMaxCols() - j);
                        buffer[Screen.GetMaxCols()] = 0;
                        j = Screen.GetMaxCols();
                        break;
              default:
                        buffer[j] = ch;
                        break;
           }
        }
        Screen.Normal ();
        Screen.Put( 0, i, buffer);
      }
      if (Screen.DispError ("---- please hit any key to continue -----") == ESC)
          break;
   }
   while (!end_loop);

   if (ferror (stream))
   {
      sprintf (buffer, "!!! file %s read error !!!", filename);
      Screen.DispError (buffer);
   }
   fclose (stream);
   return (0);

}
/*--------------------------------------------------------------------------
    disp menue flags    ( only one level )
----------------------------------------------------------------------------
*/

int menu_disp_flags (FIELD entry[], int entry_count, int flag)
{
int     i;
   
   Screen.NoCursor();

   /* display act. params */
   for (i = 0; i < entry_count; i++)
   {
      if (flag == entry[i].option_flag)
      {
        entry[i].status_flag = ACTIV;
      }
      else
      {
        entry[i].status_flag = IN_ACTIV;
      }
      /* display flag on screen */
      if (entry[i].status_flag == ACTIV)
      {
        Screen.Invert ();
      }
      else
      {
        Screen.Normal ();
      }
      Screen.Put(entry[i].x, entry[i].y, entry[i].text);
   }
   return (0);
}

/*--------------------------------------------------------------------------
    get menue input ( only one level )
----------------------------------------------------------------------------
*/

int menu_input(FIELD entry[], int entry_count, int mode)
{
int     i, ch, loop_end = 0, activ_flag = 0;
   
   #ifdef DEBUG_SCREEN
      Screen.ToFile( "Rameau.scr" );
   #endif
   
   /* display act. params */
   for (i = 0; i < entry_count; i++)
   {
      if (entry[i].status_flag == ACTIV && mode != INPUT_MODE_NO_RET)
      {
         Screen.Invert ();
      }
      else
      {
         Screen.Normal ();
      }
      Screen.Put( entry[i].x, entry[i].y, entry[i].text);
      Screen.Normal ();
   }

   do
   {
      /* select function */
      ch = GetKey();

      if (ch == ESC)
         return (ESC);

      if (ch == RET && mode == INPUT_MODE_RET)
      {
         for (i = 0; i < entry_count; i++)
         {
            if (entry[i].status_flag == ACTIV)
            {
                return (entry[i].key);
            }
         }
      }
      if (ch == RET && mode == INPUT_MODE_NO_RET)
      {
        return (ch);
      }
      activ_flag = 0;

      for (i = 0; i < entry_count; i++)
      {
        if (LOW_CHAR (entry[i].key) == ch)
        {
            entry[i].status_flag = ACTIV;
            activ_flag = 1;
        }
      }
      if (!activ_flag)
        continue;

      for (i = 0; i < entry_count; i++)
      {
        entry[i].status_flag = IN_ACTIV;
        if (LOW_CHAR (entry[i].key) == ch)
        {
            entry[i].status_flag = ACTIV;
        }
      }
      /* display act. params */
      for (i = 0; i < entry_count; i++)
      {
        if (entry[i].status_flag == ACTIV)
        {
            Screen.Invert ();
        }
        else
        {
            Screen.Normal ();
        }
        Screen.Put( entry[i].x, entry[i].y, entry[i].text);
        Screen.Normal ();
      }
      if (mode == INPUT_MODE_NO_RET)
      {
         return (ch);
      }
   }
   while (!loop_end);

   return (ch);
}


/*--------------------------------------------------------------------------
    Display a menu on the output screen
----------------------------------------------------------------------------
*/


int menu_disp(char *menu[], int lines)
{
int i;

   Screen.NoCursor();
   Screen.Normal();
   Screen.Clear();

   for (i = 0; i < lines; i++)
   {
      Screen.Put( 0, i, menu[i]);
   }
   Screen.Pos(0, 0);

   return (0);
}

/*--------------------------------------------------------------------------
    display input menu
----------------------------------------------------------------------------
*/

int input_disp ()
{

   menu_disp(input_menu, Screen.GetMaxLines());
   menu_disp_flags (input_format_input, input_format_input_count, input_flags.infile);
   InputName.Disp();  
   edit_info (EDI_DISP);
   return (0);

}

/*--------------------------------------------------------------------------
    input menu input
----------------------------------------------------------------------------
*/

int input_inp()
{
int     ch, loop_end = 0;

   input_disp ();
   do
   {  
      ch = menu_input(input_input, input_input_count, INPUT_MODE_NO_RET);
      switch (ch)
      {
      case 'f':
      case 'F':
         ch = menu_input(input_format_input, input_format_input_count, INPUT_MODE_RET);
         switch (ch)
         {
            case 'x':
            case 'X':
                not_implemented ();
                input_disp ();
                // input_flags.infile = PMX;
            break;

            case 'm':
            case 'M':
                input_flags.infile = MID;
                break;
            case 'p':
            case 'P':
                input_flags.infile = PCS;
                break;
            case 'r':
            case 'R':

                input_flags.infile = RDL;
                break;
            case 'f':
            case 'F':
                not_implemented ();
                input_disp ();
                /* input_flags.infile = FDL; */
                break;
            case 't':
            case 'T':
                input_flags.infile = ATV;
                break;
            case 'v':
            case 'V':
                input_flags.infile = VPS;
                break;
            case 'l':
            case 'L':
                input_flags.infile = MVP;
                break;
            case ESC:
                break;
        }
        break;

      case 'r':
      case 'R':
         {
            Screen.Normal ();
            edit_info (EDI_DISP);
            edit_info (EDI_EDIT);
            RameauWorkFileName.Ext( "inf" );
            write_info ( RameauWorkFileName.FileName() );
         }
         break;

      case 'h':
      case 'H':
            DispAsciiFile ("hlp\\input.hlp");
            input_disp ();
            break;

      case 'd':
      case 'D':     /* show directory */
            show_dir ();
            input_disp ();
            break;

      case 'n':
      case 'N':
            {
                Screen.Normal ();
                InputName.Edit();
                /* read and disp info-file */
                RameauWorkFileName.Ext( "inf" );
                InputName.Disp();
                if (read_info(RameauWorkFileName.FileName() ) != 0)
                {      
                    edit_info (EDI_CLOSE);
                    if( edit_info (EDI_INIT) != 0 )  
                    {
                        Screen.DispError ("--- Error in Function Input_inp() ----");
                        return -1;
                    }   
                    edit_info (EDI_DISP);
                }
                edit_info (EDI_DISP);
            }
            break;

      case RET:
            write_config ();
            return (RET);

      case ESC:
            return (ESC);
            break;

      }
   }
   while (!loop_end);
   return (0);
}
/*--------------------------------------------------------------------------
    output menu input
----------------------------------------------------------------------------
*/
int output_disp (void)
{

   menu_disp(output_menu, Screen.GetMaxLines());
   menu_disp_flags (output_file_input, output_file_input_count, input_flags.outfile);
   menu_disp_flags (output_voice_input, output_voice_input_count, input_flags.voices);
   menu_disp_flags (output_max_input, output_max_input_count, input_flags.max_cardinality);

   return (0);
}

int output_inp ()
{
int     ch, loop_end = 0;

   output_disp ();
   do
   {
      ch = menu_input(output_input, output_input_count, INPUT_MODE_NO_RET);
      switch (ch)
      {
      case 'f':
      case 'F':
         ch = menu_input(output_file_input, output_file_input_count, INPUT_MODE_RET);
         switch (ch)
         {
            case 'c':
            case 'C':
                    input_flags.outfile = ICV;
                    break;
            case 'm':
            case 'M':
                    input_flags.outfile = MAP;
                    break;
            case 'p':
            case 'P':
                    input_flags.outfile = PCS;
            break;
            case 'a':
            case 'A':
                    not_implemented ();
                    output_disp ();
                    // input_flags.outfile = RDA;
                    break;
            case 'f':
            case 'F':
                    not_implemented ();
                    output_disp ();

                    // input_flags.outfile = FDL;
                    break;

            case 't':
            case 'T':
                    input_flags.outfile = ATV;
                    break;
            case 'r':
            case 'R':
                    input_flags.outfile = RDL;
                    break;

            case 'v':
            case 'V':
                    input_flags.outfile = VPS;
                    break;

            case 'l':
            case 'L':
                    input_flags.outfile = MVP;
                    break;

            case 'g':
            case 'G':
                    input_flags.outfile = MPS;
                    break;

            case RET:
                    write_config ();
            case ESC:
                    loop_end = 1;
                    break;
        }
        break;

      case 'v':
      case 'V':
        ch = menu_input(output_voice_input, output_voice_input_count, INPUT_MODE_RET);
        switch (ch)
        {
        case '2':
           input_flags.voices = 2;
           break;
        case '3':
           input_flags.voices = 3;
           break;
        case '4':
           input_flags.voices = 4;
           break;
        case '5':
           input_flags.voices = 5;
           break;
        case '6':
           not_implemented();
           // input_flags.voices = 6;
           break;
        case RET:
           write_config();
        case ESC:
           loop_end = 1;
           break;
        }
        break;

      case 'm':
      case 'M':
        ch = menu_input(output_max_input, output_max_input_count, INPUT_MODE_RET);
        switch (ch)
        {
        case '3':
           input_flags.max_cardinality = 3;
           break;
        case '4':
           input_flags.max_cardinality = 4;
           break;
        case '5':
           input_flags.max_cardinality = 5;
           break;
        case '6':
           input_flags.max_cardinality = 6;
           break;
        case RET:
           write_config ();
        case ESC:
           loop_end = 1;
           break;
        }
        break;
      case 'h':
      case 'H':
     DispAsciiFile("hlp\\output.hlp");
     output_disp ();
     break;
      case RET:
     write_config ();
     loop_end = 1;
     break;
      case ESC:
     loop_end = 1;
     return (ESC);
      }
   }
   while (!loop_end);

   return (0);
}
/*--------------------------------------------------------------------------
    display procedures menu
----------------------------------------------------------------------------
*/

int procedures_disp ()
{              
   Screen.Normal();
   menu_disp(procedures_menu, Screen.GetMaxLines());
   menu_disp_flags (procedures_display, procedures_display_count, input_flags.dispfile);
   menu_disp_flags (procedures_printfile, procedures_printfile_count, input_flags.printfile);
   menu_disp_flags (ProcedureGraphicOrientation, ProcedureGraphicOrientationCount, input_flags.Orientation);
   menu_disp_flags (ProcedureGraphicFormat, ProcedureGraphicFormatCount, input_flags.graphicformat);
   Screen.Normal();
   ExternCommand.Disp();
   PcsCode.Disp();
   Screen.Put( PCS_OUTPUT_X, PCS_OUTPUT_Y, PcsOutputBuffer );

   return (0);
}
/*--------------------------------------------------------------------------
    procedures menu input
----------------------------------------------------------------------------
*/

int procedures_inp ()
{
   int     ch, loop_end = 0;

   procedures_disp ();
   do
   {
      ch = menu_input(procedures_input, procedures_input_count, INPUT_MODE_NO_RET);
      switch (ch)
      {
      case 's':
      case 'S':
     /* run Dos Shell Command */
      {
      char ComBuf[MAX_COMMAND_LENGTH]; 
      int i=MAX_COMMAND_LENGTH-1;
      
       Screen.Normal ();
       if( ExternCommand.Edit() == ESC )
       {
          ExternCommand.Disp();
       }
       else
       {   
          Screen.Normal ();
          Screen.Clear();
          ExternCommand.GetStr(ComBuf, MAX_COMMAND_LENGTH );
          while( i-- > 0 && ComBuf[i] == ' ') ComBuf[i] = 0;
          DoSystemCall( ComBuf ); 
          Screen.DispError ("---- please hit any key to continue RAMEAU -----");
          procedures_disp ();
       }  
      } 
      break;
      case 'c':
      case 'C':
     /* disp pcs code */
      {
      char ComBuf[MAX_COMMAND_LENGTH]; 
      int i=MAX_COMMAND_LENGTH-1;
      
       Screen.Normal ();
       if( PcsCode.Edit() == ESC )
       {
          PcsCode.Disp();
       }
       else
       {        
       FILE *tmp;            
       char PcsBuffer[100];
       
          strcpy( PcsOutputBuffer, "Error" );
          ToRameauDir();
          if( (tmp = fopen( "tmp\\tmp.vps", "w" )) != NULL )
          {
            while( i-- > 0 && ComBuf[i] == ' ') ComBuf[i] = 0;
            PcsCode.GetStr(PcsBuffer, 100 );
            fprintf( tmp, "%s\n", PcsBuffer );
            fclose( tmp );
            remove( "tmp\\tmp.pcs");
            remove( "rameau.log");
            strcpy( ComBuf,"bin\\vps2pcs -e -o tmp\\tmp >tmp\\tmp.pcs");
            DoSystemCall( ComBuf );           
            
            /* if ther an error in LOG-file */

            if( (tmp = fopen( RAMEAU_ERROR_FILE, "r" )) != NULL )
            {  
               if( fgetc( tmp ) == EOF )
               {  
                  fclose( tmp );
                  /* get valude from tempfile */
                  if( (tmp = fopen( "tmp\\tmp.pcs", "r" )) != NULL )
                  {
                     fgets( PcsOutputBuffer, 8, tmp);
                     fclose( tmp ); 
                     remove( "tmp\\tmp.pcs");
                     if( PcsOutputBuffer[1] != '-' )
                        strcpy( PcsOutputBuffer, "invalid" );
                  }
               }
               else
                 fclose( tmp );

               remove( "rameau.log");
            }     
          }
          procedures_disp ();
       }  
      } 
      Screen.Normal ();
      procedures_disp ();
     break;
      case 'r':
      case 'R':
     /* run RAMEAU */
     ExecRameau();
     procedures_disp ();
     break;
      case 'd':
      case 'D':
     /* display */
     {
        int     ok = 0;
        ch = menu_input(procedures_display, procedures_display_count, INPUT_MODE_RET);
        switch (ch)
        {
        case 'i':
        case 'I':
           input_flags.dispfile = INF;
           RameauWorkFileName.Ext( input_flags.dispfile );
           if( TestFile( RameauWorkFileName.FileName() ) == 0 ) 
           {
              BuildInfoTempFile( RameauWorkFileName.FileName(),"tmp\\info.tmp", 1 );
              DispAsciiFile( "tmp\\info.tmp");
              remove( "tmp\\info.tmp");
              procedures_disp ();
           }   
           break;

        case 'c':
        case 'C':
           input_flags.dispfile = ICV;
           DispFile();
           break;

        case 'r':
        case 'R':
           input_flags.dispfile = RDL;
           DispFile();
           break;

        case 'f':
        case 'F':
           not_implemented ();
           procedures_disp ();
           //  input_flags.dispfile = FDL; ok=1;
           break;

        case 't':
        case 'T':

           input_flags.dispfile = ATV;
           DispFile();
           break;
        case 'v':
        case 'V':

           input_flags.dispfile = VPS;
           DispFile();
           break;

        case 'p':
        case 'P':
           input_flags.dispfile = PCS;
           DispFile();
           break;

        case 'm':
        case 'M':
           input_flags.dispfile = MAP;
           menu_disp(processing_menu, Screen.GetMaxLines());
           DispFile();
           break;

        case 'l':
        case 'L':
           input_flags.dispfile = MVP;
           DispFile();
           break;

        case 'g':
        case 'G':
           input_flags.dispfile = MPS;
           menu_disp(processing_menu, Screen.GetMaxLines());
           DispFile();
           break;

        case RET:
           // DispFile();
           break;
        case ESC:
           break;
        }
        if (ch == ESC)
           break;
     }
     procedures_disp ();
     break;
#ifndef WIN32
************************* NO PRINT in WIN32 ***********************************************
      case 'p':
      case 'P':
     /* print */
     {
        int     ok = 0;
        ch = menu_input(procedures_printfile, procedures_printfile_count, INPUT_MODE_RET);
        switch (ch)
        {
        case 'i':
        case 'I':                   
              {
                input_flags.printfile = INF;
                menu_disp(processing_menu, Screen.GetMaxLines());
                RameauWorkFileName.Ext( input_flags.printfile );
               
                if( TestFile( RameauWorkFileName.FileName() ) == 0 )
                { 
                   BuildInfoTempFile( RameauWorkFileName.FileName(),"tmp\\info.tmp",0 );
                   PrintFile( "tmp\\info.tmp" );
                   remove( "tmp\\info.tmp");
                }  
              }  
           break;
           
        case 'r':
        case 'R':
           input_flags.printfile = RDL;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case 'f':
        case 'F':
           not_implemented ();
           procedures_disp ();
           //  input_flags.printfile = FDL;
           //  PrintFile();
           break;

        case 't':
        case 'T':
           input_flags.printfile = ATV;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case 'c':
        case 'C':
           input_flags.printfile = ICV;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case 'v':
        case 'V':
           input_flags.printfile = VPS;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case 'p':
        case 'P':
           input_flags.printfile = PCS;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case 'l':
        case 'L':
           input_flags.printfile = MVP;
           menu_disp(processing_menu, Screen.GetMaxLines());
           RameauWorkFileName.Ext( input_flags.printfile );
           PrintFile(RameauWorkFileName.FileName() );
           break;

        case RET:
           break;
        case ESC:
           break;
        }
        if (ch == ESC)
           break;
     }
     procedures_disp ();
     break;
************************* NO PRINT in WIN32 ***********************************************
#endif // WIN32

      case 'i':
      case 'I':
     /* graphic */
     {
        int     ok = 0;
        ch = menu_input(ProcedureGraphicOrientation, ProcedureGraphicOrientationCount, INPUT_MODE_RET);
        switch (ch)
        {
        case 'p':
        case 'P':
           input_flags.Orientation = PORTRAIT;
           break;

        case 'l':
        case 'L':
           input_flags.Orientation = LANDSCAPE;
           break;

        case RET:
           break;
        case ESC:
           break;
        }
        if (ch == ESC)
           break;
           write_config();
     }
     procedures_disp ();
     break;

      case 'f':
      case 'F':
     /* graphic Format */
     {
        int     ok = 0;
        ch = menu_input(ProcedureGraphicFormat, ProcedureGraphicFormatCount, INPUT_MODE_RET);
        switch (ch)
        {
        case '3':
           input_flags.graphicformat = 3;
           break;
        case '4':
           input_flags.graphicformat = 4;
           break;
        case '5':
           input_flags.graphicformat = 5;
           break;

        case RET:
           break;
        case ESC:
           break;
        }
        if (ch == ESC)
           break;
        write_config();
     }
     procedures_disp ();
     break;

      case 'h':
      case 'H':

     DispAsciiFile("hlp\\procedur.hlp");
     procedures_disp ();
     break;

     /* disp process massage -- error file */

      case 'o':
      case 'O':
     {
        char    buffer[MAX_COMMAND_LENGTH];

        sprintf (buffer, "bin\\%s", RAMEAU_ERROR_FILE);
        DispAsciiFile(buffer);
        procedures_disp ();
     }
     break;
      case RET:
     return (RET);
      case ESC:
     return (ESC);
     break;
      }
   }
   while (!loop_end);

   return (0);

}
/*--------------------------------------------------------------------------
    option menu input
----------------------------------------------------------------------------
*/
int option_disp (void)
{
   menu_disp(option_menu, Screen.GetMaxLines());
   menu_disp_flags (option_printer_input, option_printer_input_count, input_flags.printer);
   menu_disp_flags (option_port_input, option_port_input_count, input_flags.prn_port);
   menu_disp_flags (option_graphic_input, option_graphic_input_count, input_flags.graphic);
   menu_disp_flags (option_color_input, option_color_input_count, input_flags.screen_color);
   Screen.NoCursor();
   Screen.Normal ();
   WorkPath.Disp();
   Screen.Put( 26, 4, user_company);

   return (0);

}   

int option_inp ()
{
int     ch, loop_end = 0;
   option_disp ();
   do
   {
      ch = menu_input(option_input, option_input_count, INPUT_MODE_NO_RET);
      switch (ch)
      {

      case 'w':
      case 'W':
                 WorkPath.Edit();
                 BuildWorkPath();
                 WorkPath.Disp();
                 break;
      case 'p':
      case 'P':
     ch = menu_input(option_printer_input, option_printer_input_count, INPUT_MODE_RET);
     switch (ch)
     {
     case 'e':
     case 'E':
        input_flags.printer = PRINTER_EPSON;
        break;

     case 'd':
     case 'D':
        input_flags.printer = PRINTER_DJET;
        break;

     case 'c':
     case 'C':
        input_flags.printer = PRINTER_CDJET;
        break;

     case 'l':
     case 'L':
        input_flags.printer = PRINTER_LJET2;
        break;

     case '3':
        input_flags.printer = PRINTER_LJET3;
        break;

     case 'p':
     case 'P':
        input_flags.printer = PRINTER_PS;
        break;

     case ESC:

        break;

     }
     menu_disp_flags (option_printer_input, option_printer_input_count, input_flags.printer);
     break;

      case 'g':
      case 'G':
     ch = menu_input(option_graphic_input, option_graphic_input_count, INPUT_MODE_RET);
     switch (ch)
     {
     case 'g':
     case 'G':
        input_flags.graphic = GRAPHIC_GIF;
        break;

     case 'p':
     case 'P':
        input_flags.graphic = GRAPHIC_PCX;
        break;

     case 't':
     case 'T':
        input_flags.graphic = GRAPHIC_TIFF;
        break;

     case 'n':
     case 'N':
        input_flags.graphic = GRAPHIC_NONE;
        break;

     case ESC:

        break;

     }
     menu_disp_flags (option_graphic_input, option_graphic_input_count, input_flags.graphic);
     break;

      case 't':
      case 'T':
     ch = menu_input(option_port_input, option_port_input_count, INPUT_MODE_RET);
     switch (ch)
     {
     case '1':
        input_flags.prn_port = PRN_PORT_LPT1;
        break;

     case '2':
        input_flags.prn_port = PRN_PORT_LPT2;
        break;

     case ESC:
        break;

     }
     menu_disp_flags (option_port_input, option_port_input_count, input_flags.prn_port);
     break;

      case 's':
      case 'S':
        ch = menu_input(option_color_input, option_color_input_count, INPUT_MODE_RET);
        switch (ch)
        {
        case 'b':
        case 'B':
            input_flags.screen_color = SCREEN_BLUE_WHITE;
            break;
        case 'w':
        case 'W':
            input_flags.screen_color = SCREEN_WHITE_BLACK;
            break;
        case ESC:
            break;
        }
        Screen.Color(input_flags.screen_color);
        option_disp();
        break;

      case 'h':
      case 'H':
        DispAsciiFile("hlp\\option.hlp");
        option_disp();
        break;

      case RET:
        write_config();
        loop_end = 1;
        break;

      case ESC:
        loop_end = 1;
        return (ESC);
        break;
      }
   }
   while (!loop_end);

   return (0);

}

/*--------------------------------------------------------------------------
    root menu input
----------------------------------------------------------------------------
*/

int root_inp ()
{

   int     ch, loop_end = 0;
   menu_disp(root_menu, Screen.GetMaxLines());
   do
   {
      ch = menu_input(root_input, root_input_count, INPUT_MODE_NO_RET);

      switch (ch)
      {
      case 'i':
      case 'I':      
          input_inp();
          menu_disp(root_menu, Screen.GetMaxLines());
          break;

      case 'o':
      case 'O':
          output_inp();
          menu_disp(root_menu, Screen.GetMaxLines());
          break;

      case 'p':
      case 'P':
          procedures_inp();
          menu_disp(root_menu, Screen.GetMaxLines());
          break;

      case 't':
      case 'T':
          option_inp();
          menu_disp(root_menu, Screen.GetMaxLines());
          break;

      case 'a':
      case 'A':
          menu_disp(about_menu, Screen.GetMaxLines());
          GetKey();
          menu_disp(root_menu, Screen.GetMaxLines());
          break;

      case ESC:
          loop_end = 1;
          return (ESC);

          case 'q':
          case 'Q':
         {
          int     ch = 0;

            if (kbhit ())
                getch ();

            while ((ch = Screen.DispError ("Quit [Y/N]?")) != 'Y'
                    && ch != 'y'
                    && ch != 'n'
                    && ch != 'N'
            );
            if (ch == 'y' || ch == 'Y')
            {
                Screen.Invert ();
                RameauExit(0);
            }
        }
        break;
      }
   }
   while (!loop_end);

   return (0);
}
/*--------------------------------------------------------------------------
    main function for menu
----------------------------------------------------------------------------
*/
int main(int argc, char *argv[])
{
int i;
        
   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
         switch (argv[i][1])
         {
            case 'D':   /* DEBUG : don't remove Tmp-Files */
                        DebugFlag = 1;
                        break;
            case 'e':   /* use Editor for display files */
                        if(argv[i][2] == '=' )
                        {
                            EditorFlag = 1;
                            EditorName = argv[i] + 3;        
                        }    
                        break;
           default:
                        break;
         }               
      }
   }
#ifdef WIN32
   // inti Win Console from Sreen Constructor
#else
   SetRameauVideomode();
#endif
   
   /* display about menu 3 sec. */
   menu_disp(about_menu, Screen.GetMaxLines());
   
   WaitTime( 3000L, ESC );
   
   /* init */
   input_flags.from_bar = 0;
   input_flags.to_bar = 9999;
   input_flags.time_sig_z = 4;
   input_flags.time_sig_n = 4;
   input_flags.off_beat_z = 0;
   input_flags.off_beat_n = 1;

   if (read_config() != 0)
   {
      /* set default values */                       
      Screen.DispError ("!!! Error : Invalide RameauConfigFile !!!");
      
      input_flags.infile = MID;
      input_flags.prn_port = PRN_PORT_LPT1;
      input_flags.printer = PRINTER_NONE;
      input_flags.voices = 4;
      input_flags.max_cardinality = 5;
      input_flags.outfile = MAP;
      input_flags.screen_color = SCREEN_WHITE_BLACK;
      input_flags.graphic = GRAPHIC_NONE;
      input_flags.graphicfile = MAP;
      input_flags.graphicformat = 1; // Screen
   }

   Screen.Color (input_flags.screen_color);
   ToRameauDir();
   if( edit_info (EDI_INIT) != 0 )
   {
      Screen.DispError ("!!! init main system : no more memory available !!!");
      RameauExit(2);
   }
   Screen.NoCursor();
   while (root_inp () != 0);
   edit_info (EDI_CLOSE);
   
   RameauExit( 0 );

   return 0;
}           

void RameauExit( int err )
{
   switch( err )
   {
     case 0  : // no error
              write_config();
              break;
     default :         
              break;
    }
    Screen.OldColor();
    Screen.Clear();
    Screen.NormalCursor();
#ifdef WIN32
   Screen.Normal();
   // free console will be done by screen destructor
   // FreeConsole();
   // DoSystemCall( "cls" );
#else
    RestoreVideomode();
    // Screen.DispVideoConfig();
#endif
    exit( err );          
}
/*--------------------------------------------------------------------------
    build rameau path and filename
    global variables : 
                 RameauPath
                 WorkPath
                 InputName
----------------------------------------------------------------------------
*/  
int BuildNamePath (void)
{
int ret;
    ret = BuildName();     
    if( ret != 0 )
       return ret;
    return BuildWorkPath();
}
int BuildWorkPath( void )
{
   int     i, len, pos;
   int     ch;
   char    path[MAX_PATH_LENGTH],workpath[MAX_PATH_LENGTH];

   /* work path */
   WorkPath.GetStr(workpath,MAX_PATH_LENGTH);         
   len = strlen( workpath );
   if( len >= MAX_PATH_LENGTH ) 
   {
      Screen.DispError ("!!! internal error in function BuildName() !!!");
      return (1);
   }
   path[0] = 0;
   for (pos = 0, i = 0; i < len; i++)
   {
      switch (ch = workpath[i])
      {
#ifndef WIN32
      case ' ':
                break;
#endif
      default:
                path[pos++] = ch;
                path[pos] = 0;    
                workpath[i] = ' ';
                break;
      }
   }
   len = strlen(path);
   if( len == 0 )
   {
      path[0] = '\\';
      path[1] = 0;
   }
   else if (path[len-1] != '\\')
   {
      path[len] = '\\';
      path[len+1] = 0;
   }
   WorkPath.SetStr( path );
   return( 0 );
}   

int BuildName( void )
{
   int     i, len, pos;
   int     ch;
   char    name[MAX_NAME_LENGTH+2], inputname[MAX_NAME_LENGTH+2];
   /* file name */
   name[0] = 0;  
   InputName.GetStr(inputname,MAX_NAME_LENGTH+2);              
   len = strlen (inputname);
#ifdef WIN32
   if( len > MAX_NAME_LENGTH ) 
#else
   if( len > 8 ) 
#endif
   {
      Screen.DispError ("!!! internal error in function BuildName() !!!");
      return (1);
   }
   for (pos = 0, i = 0; i < len; i++)
   {
      switch (ch = inputname[i])
      {
      case ' ':
      case '.':
                break;
      default:
                name[pos++] = ch;
                name[pos]   = 0;    
                break;
      }
   }
   if (name[0] == 0)
   {
      Screen.DispError ("!!! no input file name !!!");
      return (1);
   }
   return (0);
}
/*--------------------------------------------------------------------------
    exec_rameau_command
----------------------------------------------------------------------------
*/
int ExecRameau(void)
{
   char    buffer[MAX_COMMAND_LENGTH];

   if (BuildNamePath () != 0)
   {
      return (1);
   }
   RameauWorkFileName.Ext( input_flags.infile );
   if( TestFile( RameauWorkFileName.FileName() ) != 0 ) 
      return -1;

   /* create rameau error file */
   {
   FILE *tmp;

       strcpy( buffer, "bin\\" );
       strcat( buffer,  RAMEAU_ERROR_FILE );
       tmp = fopen( buffer ,"w" );
       if( tmp != NULL )
	   {
		   rameau_version( tmp );
           fclose( tmp );
	   }
	   else
	   {
		   Screen.DispError ("RAMEAU: can't open rameau error file\n" );
	   }

   }

   strcpy (rameau_command, "rameau ");

   /* Debug : Don't remove temp-Files */
   
   if( DebugFlag )
   {
      strcat (rameau_command, " -D" );
   }
   
   /* Color : Printer OutPutFile in Color  */
   
   if( input_flags.printer == PRINTER_CDJET )
   {
      strcat (rameau_command, " -C" );
   }
   /* Postscript page format  */
   
   if( input_flags.Orientation == LANDSCAPE )
   {
      strcat (rameau_command, " -L" );
   }

   switch( input_flags.graphicformat )
   {
	case 3: strcat (rameau_command, " -A3" ); break;
	case 4: strcat (rameau_command, " -A4" ); break;
	case 5: strcat (rameau_command, " -A5" ); break;
   }

   /* set Rameau flags */
   /* max_cardinality */

   sprintf (buffer, " -%d", input_flags.max_cardinality);
   strcat (rameau_command, buffer);

   /* set error output */
   strcat (rameau_command, " -e");

   /* set voices parameter */

   sprintf( buffer, " -ch=0..%d", input_flags.voices -1 );
   strcat (rameau_command, buffer );

   /* set timesignature, frombar , tobar */
   sprintf (buffer, " -f=%d -t=%d -b=%d/%d -s=%d/%d ",
        input_flags.from_bar,
        input_flags.to_bar,
        input_flags.off_beat_z,
        input_flags.off_beat_n,
        input_flags.time_sig_z,
        input_flags.time_sig_n
      );

   strcat (rameau_command, buffer);
   strcat (rameau_command, "\"" );
   strcat(rameau_command, RameauWorkFileName.FileName());
   strcat (rameau_command, "\"" );

   /* output file flag */
   switch( input_flags.outfile )
   {
    case RDL : strcat(rameau_command, " -RDL");
			break;
    case VPS : strcat(rameau_command, " -VPS");
			break;
    case ATV : strcat(rameau_command, " -ATV");
			break;
    case ICV : strcat(rameau_command, " -ICV");
			break;
    case PCS : strcat(rameau_command, " -PCS");
			break;
    case MAP : strcat(rameau_command, " -MAP");
			break;
    case MVP : strcat(rameau_command, " -MVP");
			break;
    case MPS : strcat(rameau_command, " -MPS");
			break;
    default  :
			strcat(rameau_command, " -");
			SetRameauExt(rameau_command, input_flags.outfile);
			break;
   }
   Screen.Pos(0, 0);

#ifdef WIN32
   ToRameauBinDir();
   sprintf (buffer, "Echo %s >>%s", rameau_command, RAMEAU_ERROR_FILE );
   DoSystemCall (buffer);
#ifdef DEBUG_PROG_FLAGS
   Screen.DispError (rameau_command);
#endif /*  */
   DoSystemCall (rameau_command);
   ToRameauDir();
   
#else /*  */
#error exec rameau
#endif /*  */

   return (0);

}
/*--------------------------------------------------------------------------
    edit rameau info
----------------------------------------------------------------------------
*/

int edit_info (int mode)
{
int     i;                    

   if (mode == EDI_INIT)
   {
      for (i = 0; i < INFO_MAX_EDI; i++)
      {  
         Info[IndexAnalysisNo] = &( AnalysisNo );
         Info[IndexDate] = &( Date );
         Info[IndexTitle] = &( Title );
         Info[IndexComposer] = &( Composer );
         Info[IndexOpus] = &( Opus );
         Info[IndexYear] = &( Year );
         Info[IndexEditor] = &( Editor );
         Info[IndexPublisher] = &( Publisher );
         Info[IndexMovement] = &( Movement );
         Info[IndexKey] = &( Key );
         Info[IndexTimeSignature] = &( TimeSignature );
         Info[IndexBarNodsfrom] = &( BarNodsfrom );
         Info[IndexBarNodsto] = &( BarNodsto );
         Info[IndexOffbeatValue] = &( OffbeatValue );
         Info[IndexMinimalRhythmValue] = &( MinimalRhythmValue );
         Info[IndexInputBy] = & (InputBy );
         if (Info[i] == NULL)
         {
            Screen.DispError ("!!! Info : no more memory available !!!");
            Screen.Color (input_flags.screen_color);
            return (1);
         }
         else
         Info[i]->Clear();
      }

      /* init  */
      input_flags.time_sig_n = input_flags.time_sig_z = 4;

      Info[IndexTimeSignature]->SetStr("4/4");

      input_flags.time_sig_n = 4;
      input_flags.time_sig_z = 0;

      Info[IndexOffbeatValue]->SetStr("0/4");
      input_flags.from_bar = 0;
      Info[IndexBarNodsfrom]->SetStr("0");
      input_flags.to_bar = 9999;
      Info[IndexBarNodsto]->SetStr("9999");
   }
   else if (mode == EDI_DISP)
   {
      for (i = 0; i < INFO_MAX_EDI; i++)
      {
         Screen.Normal();
         Info[i]->Disp();
      }
   }
   else if (mode == EDI_EDIT)
   {
      int     edi_num = 0;

      while ((edi_num >= 0) && (edi_num < INFO_MAX_EDI))
      {
         switch (Info[edi_num]->Edit() )
         {
            case EDI_B_TAB:
                 edi_num = (edi_num > 0) ? edi_num - 1 : edi_num;
                 break;

            case EDI_TAB:
                 edi_num = (edi_num < INFO_MAX_EDI - 1) ? edi_num + 1 : edi_num;
                 break;

            case EDI_ESC:      /* abort */
                 edi_num = -1;
                 /* break_esc_flag = 1; */
                 break;

            case EDI_RETURN:   /* save data */
                 edi_num = -1;
                 break;
         }
      }
      Screen.Color (input_flags.screen_color);
      Screen.Normal();
   }
   
   if (mode != EDI_CLOSE)
   {                              
   char Buffer[20];
   
      /*---------------------*/
      /* evaluate input data */
      /*---------------------*/
      /* TimeSignature */
      {
         Info[IndexTimeSignature]->GetStr(Buffer,20);
         if(sscanf (Buffer, "%d/%d", &input_flags.time_sig_z, &input_flags.time_sig_n) != 2)
         {
            /* invalid */
            Screen.DispError ("!!! invalid time signature !!!");
            input_flags.time_sig_n = input_flags.time_sig_z = 4;
         }                 
         Info[IndexOffbeatValue]->GetStr(Buffer,20);
         if(sscanf (Buffer, "%d/%d", &input_flags.off_beat_z, &input_flags.off_beat_n) != 2)
         {
            /* invalid */
            Screen.DispError ("!!! invalid offbeat value !!!");
            input_flags.time_sig_n = input_flags.time_sig_z = 4;
         }                 
         Info[IndexBarNodsfrom]->GetStr(Buffer,20);
         if(sscanf (Buffer, "%d", &input_flags.from_bar) != 1)
         {
            input_flags.from_bar = 0;
         }         
         Info[IndexBarNodsto]->GetStr(Buffer,20);
         if(sscanf (Buffer, "%d", &input_flags.to_bar) != 1)
         {
            input_flags.to_bar = 9999;
         }
      }
   }
   Screen.Color (input_flags.screen_color);
   Screen.NoCursor();
   return (0);

}
/*--------------------------------------------------------------------------
    read rameau info
----------------------------------------------------------------------------
*/

#define MAX_ENTRY_LENGTH 40

int read_info (char *filename)
{

   FILE   *stream;

   int     line = 1, end_loop = 0, str_pos;
   char    buffer[TMP_BUFFER_LENGTH];
   char    str[TMP_BUFFER_LENGTH], entry[MAX_ENTRY_LENGTH];

   if ((stream = fopen (filename, "r")) == NULL)
   {
      sprintf (buffer, "!!! Info file %s open error !!!", filename);
      Screen.DispError (buffer);
      return (-1);
   }
   do
   {                /* read info entry and info string */
      if (fgets (buffer, TMP_BUFFER_LENGTH, stream) == NULL)
      {
         end_loop = 1;
         break;         /* edn of while loop */
      }
      str_pos = strlen (buffer);/* clear new line character */

      if(str_pos > 2)
      switch (buffer[str_pos])
      {
         case '\n':
         case '\r':
         buffer[str_pos - 1] = 0;
      }

      switch ( buffer[str_pos])
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
         Info[IndexAnalysisNo]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Date#"))
      {
         Info[IndexDate]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Title#"))
      {
         Info[IndexTitle]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Composer#"))
      {
         Info[IndexComposer]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Opus#"))
      {
         Info[IndexOpus]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Year#"))
      {
         Info[IndexYear]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Editor#"))
      {
         Info[IndexEditor]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Publisher#"))
      {
         Info[IndexPublisher]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Movement#"))
      {
         Info[IndexMovement]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#Key#"))
      {
         Info[IndexKey]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#TimeSignature#"))
      {
         Info[IndexTimeSignature]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#BarNodsfrom#"))
      {
         Info[IndexBarNodsfrom]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#BarNodsto#"))
      {
         Info[IndexBarNodsto]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#OffbeatValue#"))
      {
         Info[IndexOffbeatValue]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#MinimalRhythmValue#"))
      {
         Info[IndexMinimalRhythmValue]->SetStr( str );
      }
      else if (0 == strcmp (entry, "#InputBy#"))
      {
         Info[IndexInputBy]->SetStr( str );
      }
      else
      {
         sprintf (buffer, "!!! Parameter error in info file line #%d!!!", line);
         Screen.DispError (buffer);
      }
      line++;
   }
   while (!end_loop);

   fclose (stream);

   return (0);
}                   
/*--------------------------------------------------------------------------
    Create a Info temp-File for Printing or Display
    
    if( SpaceFlag != 0 ) Print a Spaceline after every line 
----------------------------------------------------------------------------
*/
int BuildInfoTempFile( char *InFileName, char *TmpFileName, int SpaceFlag )
{   
FILE *stream;
int i;
    
    // Get Info Information
    
    if( read_info(InFileName) != 0 )
    {
        return -1;
    }
    // Create Tmp File
    
    if( (stream = fopen( TmpFileName, "wt" )) == NULL )
    {
         Screen.DispError("Can't create temp file for info");
         return -2;
    }
    // Build Output
    
    fprintf(stream, "InfoFile: %s\n",InFileName);         
    fprintf(stream, "\nAnalysis No.: "); AnalysisNo.Write(stream);
    for( i = 0; i<30;i++) { fputc(' ',stream); }        
    fprintf(stream, "  Date: "); Date.Write(stream);        
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nTitle: "); Title.Write(stream);         
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nComposer: "); Composer.Write(stream);         
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nOpus: "); Opus.Write(stream);        
    fprintf(stream, "  Year: "); Year.Write(stream);         
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nPublisher: ");Publisher.Write(stream);         
    fprintf(stream, "  Editor: "); Editor.Write(stream);        
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nMovement: ");Movement.Write(stream);         
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nKey: "); Key.Write(stream);         
    for( i = 0; i<28;i++) { fputc(' ',stream); }        
    fprintf(stream, " Time Signature: "); TimeSignature.Write(stream);        
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nBar Nos from: "); BarNodsfrom.Write(stream);        
    fprintf(stream, " to: "); BarNodsto.Write(stream);        
    for( i = 0; i<15;i++) { fputc(' ',stream); }        
    fprintf(stream, " Offbeat Value: "); OffbeatValue.Write(stream);        
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nMinimal Rhythm Value: "); MinimalRhythmValue.Write(stream);                 
    if( SpaceFlag == 1 ) fputc('\n',stream);
    fprintf(stream, "\nInput by: "); InputBy.Write(stream);         
    fprintf(stream, "\n\n");         
                                           
    fclose(stream);
                                           
    return 0;
}  
/*--------------------------------------------------------------------------
    write rameau info
----------------------------------------------------------------------------
*/

int write_info (char *filename)
{
   int    i;
   FILE   *stream;
   char    MsgBuffer[MAX_PATH_LENGTH + 100];

   if ((stream = fopen (filename, "w")) == NULL)
   {
      sprintf (MsgBuffer, "!!! Info file %s open error !!!", filename);
      Screen.DispError (MsgBuffer);
      return (-1);
   }
  
   for( i=0; i <= InfoMaxIndex; i++ )
   {                                                   
      Info[i]->GetStr( MsgBuffer, 100 );
      fprintf( stream,"%s %s\n", InfoName[i], MsgBuffer );
   }
   fclose (stream);

   return (0);

}
/*--------------------------------------------------------------------------
    read rameau config
----------------------------------------------------------------------------
*/
int read_config()
{
FILE   *stream;
int     err = 0;               
char    buffer[MAX_PATH_LENGTH];

   if ((stream = fopen (RAMEAU_CONFIG_FILE, "r")) == NULL)
   {
      Screen.DispError ("!!! rameau config file not found !!!" );
      return (-1);
   }
   
   fgets ( buffer, MAX_PATH_LENGTH, stream);
   
   if( fscanf (stream, "%d %d %d %d %d %d %d %d %d %d %d %d\n",
       &input_flags.outfile,
       &input_flags.infile,
       &input_flags.dispfile,
       &input_flags.printfile,
       &input_flags.voices,
       &input_flags.max_cardinality,
       &input_flags.prn_port,
       &input_flags.printer,
       &input_flags.screen_color,
       &input_flags.graphic,
       &input_flags.graphicfile,
       &input_flags.graphicformat
             ) != 12 )
       err = 2;      
             

   fgets (user_company, 50, stream);
   fclose (stream);
   WorkPath.SetStr( buffer );
   return (err);
}
/*--------------------------------------------------------------------------
    Write rameau config

    1. line   work path
    2. line   rameau flags
    3. line   User name
    int    outfile;    input file ext.
    int    infile;     output file ext.
    int    max_cardinality;
    int    prn_port;
    int    prn_file_format;
    int    printer;         
    int    screen_color;
    int    graphic;
    int    graphicfile;
    int    graphicformat;

----------------------------------------------------------------------------
*/
int write_config()
{
FILE   *stream;
int     err = 0;                  
char buffer[ MAX_PATH_LENGTH ];
   
   ToRameauDir();
   
   if ((stream = fopen (RAMEAU_CONFIG_FILE, "w")) == NULL)
   {
      Screen.DispError ("!!! can't write rameau config file !!!");
      return (-1);
   }          
   WorkPath.GetStr( buffer, MAX_PATH_LENGTH );
   fprintf (stream, "%s\n%d %d %d %d %d %d %d %d %d %d %d %d\n%s",
        buffer,
        input_flags.outfile,
        input_flags.infile,
        input_flags.dispfile,
        input_flags.printfile,
        input_flags.voices,
        input_flags.max_cardinality,
        input_flags.prn_port,
        input_flags.printer,
        input_flags.screen_color,
        input_flags.graphic,
        input_flags.graphicfile,
        input_flags.graphicformat,
        user_company
      );

   fclose (stream);
   return (0);
}
/*--------------------------------------------------------------------------
     print file
----------------------------------------------------------------------------
*/

int PrintFile ( char *FileName )
{
char buffer[ 2 * MAX_COMMAND_LENGTH];

   if( TestFile( FileName ) != 0 ) 
      return -1;
    
   if( input_flags.printer == PRINTER_PS )
   {
      // this is a postscript printer

      switch( input_flags.printfile )
      {
        default  :
             // outputfile is an ascii file
             {
                strcpy( buffer, "COPY ");
                strcat( buffer, FileName );
                strcat( buffer, " ");

                switch( input_flags.prn_port )
                {
                    case PRN_PORT_LPT1 :
                        strcat( buffer, PRN_LPT1_STR );
                        break;
                    case PRN_PORT_LPT2 :
                        strcat( buffer, PRN_LPT2_STR );
                        break;
                    default   :
                        return -1;
                        break;
                }
                strcat( buffer, " > Rameau.Nul" );
                not_implemented ();

                #ifdef DEBUG_PRINTER_CALL
                    Screen.DispError( buffer );
                #else       
                    return 1;
                    DoSystemCall( buffer );
                #endif
            }
            break;
      }
   }
   else
   {
      // this is no postscript printer
      // init printer string

      switch( input_flags.printfile )
      {

        default  :

             // outputfile is an ascii file
             {
                strcpy( buffer, "COPY ");
                strcat( buffer, FileName ); 
                strcat( buffer, " " );

                switch( input_flags.prn_port )
                {
                    case PRN_PORT_LPT1 :
                         strcat( buffer, PRN_LPT1_STR );
                         break;
                    case PRN_PORT_LPT2 :
                         strcat( buffer, PRN_LPT2_STR );
                         break;
                    default   :
                         return -1;
                         break;
                }
                strcat( buffer, " > Rameau.Nul" );

                #ifdef DEBUG_PRINTER_CALL
                    Screen.DispError( buffer );
                #else
                    DoSystemCall( buffer );
                #endif
             }
             break;
      }
   }
   return 0;
}
/*--------------------------------------------------------------------------
   display file
----------------------------------------------------------------------------
*/
int DispFile()
{
   switch( input_flags.dispfile )
   {
     // use GS for display while dispfile is a PostScriptFile

     case MAP :
     case MPS :
          { 
            RameauWorkFileName.Ext( input_flags.dispfile );

            if( TestFile( RameauWorkFileName.FileName() ) != 0 ) 
               return -1;
            else
               GS.GsToGsView( RameauWorkFileName.FileName() );
          }
          break;

     // dispfile is no PostScriptFile


     default  :
          {
            RameauWorkFileName.Ext( input_flags.dispfile );
            if( EditorFlag == 1 )
            {                
            char Buffer[MAX_PATH_LENGTH];
                               
               strcpy( Buffer, EditorName );                
               strcat( Buffer, " " );                
               strcat( Buffer, RameauWorkFileName.FileName() );
               if( TestFile( RameauWorkFileName.FileName()) == 0 )
               {                 
                  ReadOnly( RameauWorkFileName.FileName() ); 
                  DoSystemCall( Buffer );
                  ReadWrite( RameauWorkFileName.FileName() ); 
               }   
            }
            else   
            {
               DispAsciiFile( RameauWorkFileName.FileName() );
            }
          }
          break;
   }
   return 0;
}
/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
/*#define  DIR_SHOW_FILE_FLAGS
*/
int show_dir (void)
{
char    buffer[MAX_COMMAND_LENGTH];
FILE    *output;
struct  _finddata_t find;
long    hFile; 
   
   BuildWorkPath ();
   if ((output = fopen (RAMEAU_DIR_FILE, "w")) == NULL)
   {
      /* open error */
      return (-1);
   }
   /* Find first matching file, then find additional matches. */
   
   WorkPath.GetStr(  buffer,MAX_COMMAND_LENGTH );
   strcat (buffer, "*.*");

   
   if( (hFile = _findfirst( buffer, &find )) == -1L )
       fprintf( output, "no files in current directory!\n" );
   else
   {
      fprintf (output, "%-20s  %-8s   %-8s\n",
           "FILE", "SIZE", "DATE");
     do
     {
      fprintf (output, "%-20s  %-9ld  %.24s \n",
				find.name,
				find.size,
				ctime( &( find.time_write ) )
		      );
     }
     while ( _findnext( hFile, &find) == 0 );
   }

   _findclose( hFile );
   fclose (output);

   DispAsciiFile( RAMEAU_DIR_FILE );
   return 0;
}
