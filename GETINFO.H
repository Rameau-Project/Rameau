/*---------------------------------------------------------------------------
    Dieter Neubacher		   Vers. 1.0		    04.01.94
    -------------------------------------------------------------------------
    getInfo.h		 header file for Read Rameau Info Files
			 used for PS output
-----------------------------------------------------------------------------
*/


#define AnalysisNo	      0
#define Date		      1
#define Title		      2
#define Composer	      3
#define Opus		      4
#define Year		      5
#define Editor		      6
#define Publisher	      7
#define Movement	      8
#define Key		      9
#define TimeSignature	     10
#define BarNodsfrom	     11
#define BarNodsto	     12
#define OffbeatValue	     13
#define MinimalRhythmValue   14
#define InputBy 	     15

#define INFO_FIELD_COUNT     16

typedef struct
{
   int	x;
   int	y;
   int	input_size;
   char name[80];
   char text[80];

}PS_INFO_FIELD;


extern PS_INFO_FIELD PsInfoTxt[];


/*---------------------------*/
/* function definitions      */
/*---------------------------*/

int disp_error_message( char *str );
int GetInfo( FILE *InfoStream);
