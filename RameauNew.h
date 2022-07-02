/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                        26.03.93
                                   1.1                        01.10.94
								   2.0                        29-01-97
    -------------------------------------------------------------------
    rameau.h


    Version 2.0 : WIN32
-----------------------------------------------------------------------
*/

#ifndef _RAMEAU_H_
#define _RAMEAU_H_

#include <stdlib.h>
#define __MS_DOS__  
  
#define DESTRIBUTION     /* Bulid executible file for enduser */


#ifdef __GNUC__
   #define uchar unsigned char
   #define uint  unsigned int
   #define ulong unsigned int
#else
   #define uchar unsigned char
   #define uint  unsigned int
   #define ulong unsigned long
#endif

#define SET_BIT( a, b)  ((a) |= (0x8000 >> (b)) )

/*
#define SET_BIT( a, b)  ((a) |= (0x0001 << (15-b)) )
*/

/* pagesize for ascii map output */

#define ASCII_LINE_LENGTH 120
#define ASCII_PAGE_LENGTH 120


#define AUTO_EXTENION

#define PORTRAIT  0
#define LANDSCAPE 1
/*--------------------*/
/* FILE_SPECIFICATION */
/*--------------------*/

/* File execution structure ******

Path: 0x1*   0x4*    0x2*    0x3*       0x4*       0x5*

Level  
0     INF

1     MID
       |
2     RDL--------------+--------+---------+----------+
       |               |        |         |          | 
3     VPS             MVP      ATV       TINT     RDLSTA
       |               |                  |
4     PCS-----+       MPS                SSTR
       |      |     (P/G MP)
5     MAP    ICV
     

***********************************/

#define INF    0x01    // info File
        
        
#define MID    0x11
#define RDL    0x12
#define VPS    0x13
#define PCS    0x14
#define MAP    0x15    // disp file

#define MVP    0x23
#define MPS    0x24    // disp file

#define ATV    0x33
#define TINT   0x34
#define RDLSTR 0x35
#define SSTR   0x44
#define ICV    0x45

#define PMX    0x91    // not used
#define FDA    0x92
#define FLA    0x93
#define RDA    0x94
#define RLA    0x95
#define FDL    0x96

/*--------------------*/
/*                    */
/*--------------------*/

#define GRAPHIC_NONE  0x80
#define GRAPHIC_GIF   0x81
#define GRAPHIC_PCX   0x82
#define GRAPHIC_TIFF  0x83

/*--------------------*/
/*                    */
/*--------------------*/

#define PRN_PORT_LPT1 0
#define PRN_PORT_LPT2 1

#define PRN_LPT1_STR  "LPT1 "
#define PRN_LPT2_STR  "LPT2 "

/*--------------------*/
/* Printer            */
/*--------------------*/


#define PRINTER_NONE   0
#define PRINTER_EPSON  1
#define PRINTER_DJET   2
#define PRINTER_CDJET  3
#define PRINTER_LJET2  4
#define PRINTER_LJET3  5
#define PRINTER_PS     6

#define PRINTER_NONE_STR    "vga"       // Use Vga vor Gs
#define PRINTER_EPSON_STR   "epson"
#define PRINTER_DJET_STR    "djet500"
#define PRINTER_CDJET_STR   "cdeskjet"
#define PRINTER_LJET2_STR   "ljet2p"
#define PRINTER_LJET3_STR   "ljet3"
#define PRINTER_PS_STR      ""

/*--------------------*/
/* REMEAU FLAGS       */
/*--------------------*/

typedef struct
{
    int    outfile;       /* input file ext.  */
    int    infile;        /* output file ext. */
    int    dispfile;      /* display file ext.*/
    int    printfile;     /* display file ext.*/
    int    graphicfile;
    int    graphicformat; /* Ps Page size */
    int    voices;
    int    max_cardinality;
    int    prn_port;
    int    screen_color;
    int    printer;
    int    graphic;
    int    time_sig_z;  /*                  Zaehler */
    int    time_sig_n;  /* time signature Nenner  */
    int    off_beat_z;
    int    off_beat_n;  /* off beat time */
    int    from_bar;
    int    to_bar;
	int    Orientation;
    char   in_filename[_MAX_PATH];
    char   out_filename[_MAX_PATH];

} REMEAU_FLAGS;

extern REMEAU_FLAGS rameau_flags;

/*-------------------------------*/
/* ERROR MASSAGE TO ERROR STREAM */
/*-------------------------------*/

#define RAMEAU_ERROR_FILE "rameau.log"

#define RAMEAU_DIR_FILE   "tmp\\tmp.tx"
#define RAMEAU_CONFIG_FILE "bin\\rameau.cfg"

/*---------------------------*/
/* buffer size               */
/*---------------------------*/


#define MAX_COMMAND_LENGTH    _MAX_PATH
#define TMP_BUFFER_LENGTH     _MAX_PATH
#define MAX_PATH_LENGTH       _MAX_PATH    /* Dos maxiaml path length */


#define MAX_LONG_VAL   2147483647L
#define MAX_INT_VAL    32767

#endif // _RAMEAU_H_


/*----------------------------*/
/* Postscript Parameter       */
/*----------------------------*/

 
