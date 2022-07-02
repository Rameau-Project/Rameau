/*---------------------------------------------------------------------
Dieter Neubacher			  Vers.: 2.0                       29-01-97
-----------------------------------------------------------------------
     R  A  M  E  A  U   Music Analysis Software
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "rameau.h"

int DebugFlag = 0;   // if == 1 don't erase  tmp-files
 

/*----------------------------------------------------------------------
    Get Channels form Parameter


    channel must less eq max

    Param : 1,2,3,4
       or   2..5,1,7..11

    return Param string
       NULL  if error
------------------------------------------------------------------------
*/

int *GetChParam( char *str, int max )
{
int i, *ret, num, end, pos = 0;


    if((ret = new int[max+1]) == NULL )
      return NULL;

    for( i=0; i<=max; i++ )
       ret[i] = -1;

    // scan Param list

    while( *str != 0 )
    {
       if( isdigit( *str ) )
       {
      if( pos > max )
      {
        delete ret;
        return NULL;
      }
      if( (i = sscanf( str, "%d", &num )) == 0 )
      {
        delete ret;
        return NULL;
      }
      if( num < 0 || num > max  )
      {
        delete ret;
        return NULL;
      }
      str +=i;


      if( *str == '.' && *(str+1) == '.' )  // is a reange specified
      {
         str += 2;

         if( (i = sscanf( str, "%d", &end )) == 0 )
         {
           delete ret;
           return NULL;
         }
         str += i;

         if( end < 0 || end > max  )
         {
           delete ret;
           return NULL;
         }

         for( i = num; i <= end; i++ )
         {
            if( pos > max )
           {
             delete ret;
             return NULL;
           }
           ret[ pos++ ] = i;
         }
      }
      else
      {
         // it was a singel number

         ret[ pos++ ] = num;
      }
       }
       else
       if( *str != ','  )
       {
          delete ret;
          return NULL;
       }
       else
       {
          str++;
       }
    }
    return ret;
}

/*--------------------------------------------------------------------------
    set extention
----------------------------------------------------------------------------
*/

int SetRameauExt (char *str, int ect_number)
{

   switch (ect_number)
   {
   case INF:                 // info file
      strcat (str, "inf");
      break;

   case MAP:                 // MAP Screnn
      strcat (str, "map.ps");
      break;

   case MVP:
      strcat (str, "mvp");
      break;

   case MPS:
      strcat (str, "mvp.ps");
      break;

   case PCS:
      strcat (str, "pcs");    // Pitch class Set File
      break;

   case VPS:                  
      strcat (str, "vps");
      break;

   case RDL:
      strcat (str, "rdl");
      break;

   case RDLSTA:				// Statistik
      strcat (str, "rdl.sta");
      break;

   case TINT:				// Tone Interval
      strcat (str, "TInt");
      break;

   case SSTR:				// Search String
      strcat (str, "SStr");
      break;

   case ATV:
      strcat (str, "atv");
      break;

   case PMX:
      strcat (str, "pmx");
      break;

   case FDA:
      strcat (str, "fda");
      break;

   case FLA:
      strcat (str, "fla");
      break;

   case RDA:
      strcat (str, "rda");
      break;

   case RLA:
      strcat (str, "rla");
      break;

   case MID:               // midi file
      strcat (str, "mid");
      break;

   case FDL:
      strcat (str, "fdl");
      break;

   case ICV:
      strcat (str, "icv");
      break;

   default:
      return (-1);

   }
   return (0);
}
