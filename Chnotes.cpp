/*-------------------------------------------------------------------------
    Dieter Neubacher	     Vers.: 1.00		     Wuhu, 09-07-94
    -----------------------------------------------------------------------
    ChNotes.cpp

    -----------------------------------------------------------------------
*/


#include "ChNotes.h"

void ChNotes::Clear( void )
{
int i,j;

    for( i=0; i<MAX_CHANNEL; i++ )
    {
		for( j=0; j<MAX_NOTE_VAL; j++ )
		{
			Notes[i][j].Clear();
		}
    }
    ClearFlags();
}

void ChNotes::ClearNew( void )
{
int i,j;

    for( i=0; i<MAX_CHANNEL; i++ )
    {
		for( j=0; j<MAX_NOTE_VAL; j++ )
		{
			Notes[i][j].ClearNew();
		}
    }
    ClearFlags();
}

void ChNotes::NewTime( void )
{
int i,j;

    for( i=0; i<MAX_CHANNEL; i++ )
    {
		for( j=0; j<MAX_NOTE_VAL; j++ )
		{
			Notes[i][j].NewTime();
		}
    }
}

void ChNotes::ClearFlags( void )
{
   for( int i=0; i<MAX_CHANNEL; i++)
   {
      SpeedupChFlag[i] = 1;
      SpeedupChNote[i] = 0;
   }
}
/*---------------------------------------------------------------------
  set note informations
-----------------------------------------------------------------------
*/

int  ChNotes::SetChNotes( int notes[MAX_CHANNEL] )
{
int ret = 0;
int channel;

   for (channel = 0; channel < MAX_CHANNEL; channel++)
   {
      if (notes[channel] > MAX_NOTE_VAL || notes[channel] < -MAX_NOTE_VAL)
      {
    	 fprintf (ErrStream, "invalid note info %d \n", notes[channel]);
	     ret++;
      }
      else if (notes[channel] != 0)
      {
	     if (notes[channel] > 0)
		 {
	        SpeedupChFlag[ channel ] = 0;
	        Inc(channel , notes[channel] );
		 }
	     else
		 {
	        if ( Dec( channel , -notes[channel] ) < 0)
			{
	           fprintf (ErrStream, "note off error %d\n", notes[channel]);
	           ret++;
			}
	        SpeedupChFlag[ channel ] = 1;
	        for( int i = 0; i < MAX_NOTE_VAL; i++)
			{
	           if( Get( channel, i ) )
			   {
		           SpeedupChFlag[ channel ] = 0;
		           SpeedupChNote[ channel ] = i;
			   }

			}
		 }
      }
   }
   return ret;
}
/*-----------------------------------------*/
/* Set Min and Max Value for each Channel  */
/*-----------------------------------------*/

void  ChNotes::SetMinMax()
{
int i,j, min, max, newmin, newmax;

	TotalMinVal = TotalNewMin = MAX_NOTE_VAL;
	TotalMaxVal = TotalNewMax = 0;
    for( i=0; i<MAX_CHANNEL; i++ )
    {	
		max = newmax = 0; 
		min = newmin = MAX_NOTE_VAL;

		for( j=0; j<MAX_NOTE_VAL; j++ )
		{
			if( Get(i,j) > 0 )
			{	
				if( j < min ) min = j;
			    if( j > max ) max = j;
			}
			if( GetNew(i,j) > 0 )
			{
				if( j < newmin ) newmin = j;
				if( j > newmax ) newmax = j;
			}
		}
		MinVal[i] = min;
		NewMin[i] = newmin;
		MaxVal[i] = max;
		NewMax[i] = newmax;

		if( min    < TotalMinVal ) TotalMinVal = min;
		if( newmin < TotalNewMin ) TotalNewMin = newmin;
		if( max    > TotalMaxVal ) TotalMaxVal = max;
		if( newmax > TotalNewMax ) TotalNewMax = newmax;
    }
}

// Total number of notes whith given value
	
int   ChNotes::Get( int NoteValue )
{
int ch, num =0; 

	for( ch = 0; ch < MAX_CHANNEL; ch++)
	{
		// num += Get( ch, NoteValue );
		num += Notes[ch][NoteValue].Get();
	}
	return num;
}

int   ChNotes::GetNew( int NoteValue )
{
int ch, num =0; 

	for( ch = 0; ch < MAX_CHANNEL; ch++)
	{
		// num += GetNew( ch, NoteValue );
		num += Notes[ch][NoteValue].GetNew();
	}
	return num;
}
