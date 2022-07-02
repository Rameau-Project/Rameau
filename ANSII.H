/*---------------------------------------------------------------------
    Dieter Neubacher	    Vers.: 1.0			      16.12.93
    -------------------------------------------------------------------
    ansii.h	 functions for RAMEAU
-----------------------------------------------------------------------
*/


/*------------------*/
/* cursor modes     */
/*------------------*/


#define C_HIDE	0
#define C_LOW	1
#define C_BIG	2


/*---------------------------*/
/* use ansii screen commands */
/*---------------------------*/

#define BLACK	0
#define WHITE  15


void   attrib_on( void );
void   attrib_invert( void );
void   set_cursor_mode( int );
int    set_screen_color( int color );
int    massage( FILE *stream, char *str, int flag );
int    clear_massage( int flag );
int    get_old_color( void );
int    set_old_color( void );

/*---------------------------*/
/* screen size		     */
/*---------------------------*/

extern const int max_screen_lines;
extern const int max_screen_cols;



#ifdef __MS_DOS__

extern struct _xycoord screen_pos;

#define ANSII_CLEAR_SCREEN    _clearscreen( _GCLEARSCREEN )
#define SET_SCREEN_POS(a,b)   _settextposition( (b)+1,(a)+1 )
#define CURSOR_STORE	      screen_pos = _gettextposition()
#define CURSOR_RESTORE	      _settextposition( screen_pos.xcoord, screen_pos.ycoord)

#define ANSII_ATTRIB_HIDE     _settextcursor( 0x2000 );
#define ANSII_ATTRIB_SHOW     _settextcursor( 0x0707 );

#else /* use Ansii sequenses for screen moving */

#define CLEAR_SCREEN	      fprintf( stdout, "\x1b[2J" )

#define ANSII_CLEAR_SCREEN    fprintf( stdout, "\x1b[2J" )
#define SET_SCREEN_POS(a,b)   fprintf( stdout, "\x1b[%d;%dH", (b), (a) )
#define CURSOR_UP(a)	       fprintf( stdout, "\x1b[%dA", (a) )
#define CURSOR_DOWN(a)	      fprintf( stdout, "\x1b[%dB", (a) )
#define CURSOR_LEFT(a)	      fprintf( stdout, "\x1b[%dC", (a) )
#define CURSOR_RIGTH(a)       fprintf( stdout, "\x1b[%dD", (a) )
#define CURSOR_STORE	       fprintf( stdout, "\x1b[s", (a) )
#define CURSOR_RESTORE	      fprintf( stdout, "\x1b[u", (a) )

#define ANSII_ATTRIB_OFF      fprintf( stdout, "\x1b[0m" )
#define ANSII_ATTRIB_FETT     fprintf( stdout, "\x1b[1m" )
#define ANSII_ATTRIB_INVERT   fprintf( stdout, "\x1b[7m" )

#define ANSII_ATTRIB_HIDE     fprintf( stdout, "\x1b[8m" )

#endif
