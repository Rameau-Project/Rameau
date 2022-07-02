
/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                16.12.93
    Dieter Neubacher        Vers.: 1.1                02.02.96
    -------------------------------------------------------------------
    edi.c   edit functions for RAMEAU
-----------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __GNUC__
#include <memory.h>
#include <conio.h>
#else /*  */
#define putch(a) putchar(a)
#define getch() getchar()
#endif /*  */

#include "rameau.h"
#include "screen.h"
#include "edi.h"


#define EDIT_TAB_MODE
#define USE_GLOBAL_COLORS


void ediwin::WinPos(int xi, int yi)
{
   if(InWin( xi, yi) == 0)
   {
      Screen.Pos(xi - WinXoffset + WinXmin, yi - WinYoffset + WinYmin);
   }
}

ediwin::ediwin()
{
   WinXoffset = WinYoffset = 0;
   WinXmin = 0;
   WinYmin = 0;
   WinXmax = 0;
   WinYmax = 0;
   WinXsize = 0;
   WinYsize = 0;
};
ediwin::ediwin(int xmin, int xmax, int ymin, int ymax)
{
   WinXoffset = WinYoffset = 0;
   WinXmin = xmin;
   WinYmin = ymin;
   WinXmax = xmax;
   WinYmax = ymax;
   WinXsize = xmax - xmin + 1;
   WinYsize = ymax - ymin + 1;
};

void ediwin::WinSet(int xmin, int xmax, int ymin, int ymax)
{
   WinXoffset = WinYoffset = 0;
   WinXmin = xmin;
   WinYmin = ymin;
   WinXmax = xmax;
   WinYmax = ymax;
   WinXsize = xmax - xmin + 1;
   WinYsize = ymax - ymin + 1;
};

void ediwin::WinMove(int xp, int yp)
{
   if(xp < WinXoffset)
      WinXoffset = xp;
   if(yp < WinYoffset)
      WinYoffset = yp;
   if(xp >= WinXoffset + WinXsize - 1)
      WinXoffset = xp - WinXsize + 1;
   if(yp >= WinYoffset + WinYsize - 1)
      WinYoffset = yp - WinYsize + 1;
}


int ediwin::InWin(int xi, int yi)    /* pos. in Window ? */
{
   xi -= WinXoffset - WinXmin;
   yi -= WinYoffset - WinYmin;

   if( xi < WinXmin ||
       xi > WinXmax ||
       yi < WinYmin ||
       yi > WinYmax
     )
     return 1;
   else
     return 0;
}
      
void ediwin::WinDisp(int xi, int yi, char *p)
{
int     i;
   
   if(xi < WinXoffset)
   {
      p += WinXoffset - xi;
      xi = WinXoffset;
   }

   if( InWin( xi, yi) )
      return;

   WinPos( xi, yi);

   for(i = xi - WinXoffset + WinXmin; i <= WinXmax; i++)
      Screen.Put(*p++);
}
/*----------------------------------------------------------------------------------------------
    edi functions
------------------------------------------------------------------------------------------------
*/    

void edi::SetWin( int wXmin, int wXmax, int wYmin, int wYmax )
{
    WinSet( wXmin, wXmax, wYmin, wYmax );
}            

void edi::Clear()
{
   memset(p, ' ', Xsize * Ysize );
   p[Xsize * Ysize] = 0;
   SetPos( 0, 0);
}

char   *edi::Ptr(int x, int y)
{
   x =(x < 0) ? 0 : x;
   y =(y < 0) ? 0 : y;
   return(p + y * Xsize + x);

}

void edi::SetPos(int x, int y)
{
   if(x < 0)
      Xpos = 0;
   else if(x >= Xsize - 1)
      Xpos = Xsize - 1;
   else
      Xpos = x;

   if(y < 0)
      Ypos = 0;
   else if(y >= Ysize)
      Ypos = Ysize - 1;
   else
      Ypos = y;

   return;
};

void edi::LineEndPos(int y)
{
   if(y < 0)
      Ypos = 0;
   else if(y >= Ysize)
      Ypos = Ysize - 1;
   else
      Ypos = y;

   Xpos = Xsize - 1;

   while((Xpos >= 0) &&(*(p + Ypos * Xsize + Xpos) == ' '))
   {
      Xpos--;
   }
   if(Xpos + 1 < Xsize)
      Xpos++;
   return;
};


int edi::GetX()
{
   return(Xpos);
};
int edi::GetY()
{
   return(Ypos);
};
void edi::NextPos()
{
   SetPos( Xpos + 1, Ypos);
};
char edi::GChar()
{
char    ch;

   ch = *(p + Ypos * Xsize + Xpos);
   NextPos();
   return(ch);
};
char edi::GChar(int x, int y)
{
char    ch;

   SetPos( x, y);
   ch = *(p + Ypos * Xsize + Xpos);
   NextPos();
   return(ch);
};
void edi::PChar(int ch)
{
   *(p + Ypos * Xsize + Xpos) = ch;
   WinDisp( Xpos, Ypos, Ptr( Xpos, Ypos));
   NextPos();
};
void edi::PChar(int x, int y, int ch)
{
   SetPos( x, y);
   PChar( ch);
};
void edi::Disp()
{
int     i;

#ifndef USE_GLOBAL_COLORS
   Screen.Color( BackColor,TextColor )     
#endif 

   for(i = 0; i < Ysize; i++)
   {
      WinDisp( 0, i, Ptr( 0, i));
   }
}
void edi::InChar(int ch)
{
int     i = Xsize - 1;

   while(i > Xpos)
   {
      *(p + Ypos * Xsize + i) = *(p + Ypos * Xsize + i - 1);
      i--;
   }
   PChar( ch);
};


void edi::InChar(int x, int y, int ch)
{
int     i = Xsize - 1;

   SetPos( x, y);
   while(i > x)
   {
      *(p + Ypos * Xsize + i) = *(p + Ypos * Xsize + i - 1);
      i--;
   }
   PChar( ch);
};

void edi::DelChar()
{
int     x = Xpos;

   while(x < Xsize - 1)
   {
      *(p + Ypos * Xsize + x) = *(p + Ypos * Xsize + x + 1);
      x++;
   }
   *(p + Ypos * Xsize + x) = ' ';
   WinDisp( Xpos, Ypos, Ptr( Xpos, Ypos));

};

int edi::SetStr(char * str )
{
int i=0, pos=0, EndFlag = 0;;
    
    while( i < Xsize  && EndFlag == 0 )
    {                
       switch( str[i] )
       {
          case '\t' : 
                      p[pos++] = ' ';
                      i++;
                      break;      
          case  0   :
          case '\n' :       
          case '\r' : 
                
                      EndFlag = 1;      
                      break;
          default   :
                      p[pos++] = str[i++];
                      break;
       }
    } 
    p[i++] = 0;
    while( pos < Xsize )
    {   
       p[pos++] = ' ';
    } 
    return 0;
}

int edi::GetStr( char *str, int MaxLen )
{
int i = Xsize;

    while( i > 0 &&( p[i] == ' ' || p[i] == 0) ) i--;
    if( i > MaxLen ) i = MaxLen;
    strncpy( str, p, i+1 );
    str[++i] = 0;
    return i;
}              
                          
void edi::DelChar(int x, int y)
{
   SetPos( x, y);
   while(x < Xsize - 1)
   {
      *(p + Ypos * Xsize + x) = *(p + Ypos * Xsize + x + 1);
      x++;
   }
   *(p + Ypos * Xsize + x) = ' ';
   WinDisp( Xpos, Ypos, Ptr( Xpos, Ypos));
}

int edi::Edit()
{
int     ch, sc;
int     EDI_ret = 0;

#define EDI_TAB_SIZE 8

#ifndef USE_GLOBAL_COLORS
   Screen.Color( BackColor,TextColor )     
#endif 

   Screen.Invert();
   Disp();
   WinPos( Xpos, Ypos);

#ifdef __MS_DOS__
   while(!kbhit())
   {
      if(Ins == 1)
         Screen.NormalCursor();
      else                 
         Screen.BigCursor();
   }
#else /*  */
#error set cursor
#endif /*  */
   while(1)
   {
#ifdef WIN32

      ch = getch();

      if(ch == 0 || ch == 0xE0 )
      {
     sc = getch();
#else
#error Get Keybord Scancode not impementet
#endif
     switch(sc)
     {

     case EDI_RIGHT:
        SetPos( Xpos + 1, Ypos);
        break;
     case EDI_LEFT:
        SetPos( Xpos - 1, Ypos);
        break;
     case EDI_P_UP:
        SetPos( Xpos, Ypos - 8);
        break;
     case EDI_P_DOWN:
        SetPos( Xpos, Ypos + 8);
        break;
     case EDI_UP:
        SetPos( Xpos, Ypos - 1);
        break;
     case EDI_DOWN:
        SetPos( Xpos, Ypos + 1);
        break;
     case EDI_POS_1:
        SetPos( 0, Ypos);
        break;
     case EDI_ENDE:
        LineEndPos( Ypos);
        break;
     case EDI_DEL:
        DelChar();
        break;
     case EDI_INS:
        Ins =(Ins == 1) ? 0 : 1;
        break;
     case EDI_B_TAB:
#ifdef EDIT_TAB_MODE
        //ch = EDI_ESC;
        EDI_ret = EDI_B_TAB;
#else /*  */
        SetPos((Xpos / EDI_TAB_SIZE - 1) * EDI_TAB_SIZE, Ypos);
#endif /*  */
        break;
     default:
        break;
     }
      }
      else
      {

     switch(ch)
     {

     case EDI_ESC:
#ifdef EDIT_TAB_MODE
        EDI_ret = EDI_ESC;
#else /*  */
#endif /*  */
        break;
     case EDI_RETURN:
#ifdef EDIT_TAB_MODE
        EDI_ret = EDI_RETURN;
#else /*  */
        SetPos( 0, Ypos + 1);
#endif /*  */
        break;
     case EDI_BS:
        if(Xpos > 0)
           DelChar( Xpos - 1, Ypos);
        break;
     case EDI_TAB:
#ifdef EDIT_TAB_MODE
        EDI_ret = EDI_TAB;
#else /*  */
        SetPos((Xpos / EDI_TAB_SIZE + 1) * EDI_TAB_SIZE, Ypos);
#endif /*  */
        break;

     default:
        if(Ins == 1)
        {
           InChar( ch);
        }
        else
        {
           PChar( ch);
        }
        break;
     }

      }
      if(InWin( Xpos, Ypos) != 0)
      {
        WinMove( Xpos, Ypos);
        Disp();
      }
      WinPos( Xpos, Ypos);

      /* break for while loop */
      if(EDI_ret != 0)
     break;

#ifdef __MS_DOS__
      while(!kbhit())
      {
        if(Ins == 1)
          Screen.NormalCursor();
        else
          Screen.BigCursor();
      }
#else /*  */
#error set cursor
#endif /*  */
   }
   Screen.NoCursor();
   Screen.Normal();
   Disp(); 
   return(EDI_ret);
};


/*-------------------------------------------------------------------------
   open a edit field
   
   x,y Bufer Size
   wXmin, wXmax, wYmin, wYmax     Window params
---------------------------------------------------------------------------
*/
edi::edi(int x, int y, int wXmin, int wXmax, int wYmin, int wYmax)
{
   SetWin( wXmin, wXmax, wYmin, wYmax);

   p = new char[x * y + 1];
   
   if( p == NULL )                                                   
   {
      Screen.DispError ("!!! no more memory available !!!");
      exit(0);
   }
   Ins = 0;
   Xsize = x;
   Ysize = y;
   memset(p, ' ', x * y);
   p[x * y] = 0;
   SetPos( 0, 0);

   TextColor = White;
   BackColor = Black;
};

edi::~edi()
{
   delete p;
}

/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/
/*----------------------------------------
        Function not user in this Version
------------------------------------------
void edi::SetStrEnd()
{
int i = Xsize -1;
    
   RemoveStrEnd();
   while( i > 0 && p[i] == ' ' ) i--;
    p[++i] = 0;
} 

void edi::RemoveStrEnd()
{
int i = 0;

    while( i < Xsize ) 
    {
        if( p[i] == 0 ) 
           p[i] = ' ';
        i++;
    }    
} 
-----------------------------------------------------*/


