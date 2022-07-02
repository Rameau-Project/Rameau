/*---------------------------------------------------------------------
    Dieter Neubacher	    Vers.: 1.0			      16.12.93
    -------------------------------------------------------------------
    ansii.c	 functions for RAMEAU
-----------------------------------------------------------------------
*/
#include "stdio.h"
#include "rameau.h"
#include "string.h"

#ifdef __MS_DOS__
#include <graph.h>
#else /* __MS_DOS__ */
#error
#endif /* __MS_DOS__ */

#include "ansii.h"


const int max_screen_lines = 25;
const int max_screen_cols = 80;

int	back_color = 0, text_color = 15;
int	old_text_color, old_back_color;

/*---------------------------*/
/* use ansii screen commands */
/*---------------------------*/


#ifdef __MS_DOS__

void
set_cursor_mode (int mode)
{
   static  old_mode;

   switch (mode)
   {
   case C_HIDE:
      _settextcursor (0x2000);
      break;

   case C_LOW:
      _settextcursor (0x0707);
      break;

   case C_BIG:
      _settextcursor (0x0007);
      break;

   }
   old_mode = mode;
   return;
}


void
attrib_on (void)
{

   _setbkcolor (text_color);
   _settextcolor (back_color);

}


void
attrib_invert (void)
{

   _setbkcolor (back_color);
   _settextcolor (text_color);

}


int
set_screen_color (int color)
{

   switch (color)

   {

   case SCREEN_BLUE_WHITE:
      text_color = 1;
      back_color = 15;
      break;

   case SCREEN_WHITE_BLACK:

      text_color = 15;
      back_color = 0;
      break;
   }

   return (0);
}



int
get_old_color (void)
{

   old_text_color = _gettextcolor ();
   old_back_color = (int) _getbkcolor ();

   return (0);
}


int
set_old_color (void)
{

   _settextcolor (old_text_color);
   _setbkcolor (old_back_color);

   return (0);
}

/*---------------------------------------------------------------------
   display working massage
-----------------------------------------------------------------------
*/


int
massage (FILE * stream, char *str, int flag)
{

   if (flag == 0)
   {
      fprintf (stream, "%s\n", str);
   }
   else
   {
      attrib_invert ();
      SET_SCREEN_POS (0, max_screen_lines - 1);
      _outtext (str);
      // SET_SCREEN_POS (0, 0);
      attrib_on ();
   }
   return 0;
}

int
clear_massage (int flag)
{

   if (flag == 1)
   {
      char    buffer[TMP_BUFFER_LENGTH];

      memset (buffer, ' ', max_screen_cols - 1);
      buffer[max_screen_cols - 1] = 0;
      attrib_on ();
      SET_SCREEN_POS (0, max_screen_lines - 1);
      _outtext (buffer);
      SET_SCREEN_POS (0, 0);
   }
   return 0;
}


#else /* __MS_DOS__ */
#error
#endif /* __MS_DOS__ */
