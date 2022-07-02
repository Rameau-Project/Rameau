/*-------------------------------------------------------------------------
    Dieter Neubacher	     Vers.: 1.00		     Wuhu, 09-07-94
	                                2.00                   15-02-97  
    -----------------------------------------------------------------------
    ChNotes.h

    -----------------------------------------------------------------------
*/


#if !defined( _CH_NOTES_H_ )

#define _CH_NOTES_H_

#include <stdlib.h>
#include <stdio.h>

#include "global.h"


class Note
{
public:

     Note( void )
     {
		count = 0;
		newCount  = 0;
     };

int  Inc( void )	 // increment
     {
		if( count < 255 )
		{
			count++;
			newCount += 1;
			return 0;
		}
		else
			return -1;
     };

int  Get( void )
     {
		return count;
     };

int  GetNew( void )
     {
		return newCount;
     };

void ClearNew( void )
     {
		newCount = 0;
     };

int  Dec( void )	 // decrement
     {
		if( count > 0 )
			return --count;
		else
		  return -1;
     };

int  DecNew( void )
     {
		if( newCount > 0 )
		{
			newCount--;
			return --count;
		}
		else
			return -1;
     };

void NewTime( void )
     {
		newCount = 0;
     };

void Clear( void )
     {
		count = 0;
		newCount  = 0;
     };

private:

#ifdef NOTE_BUFFER_CHAR

struct {
			unsigned char count;	 // note counter
			unsigned char newCount;  // counter : new notes
       };

#else

struct {
			int count;	 // note counter
			int newCount;	 // counter : new notes
       };
};

#endif

class ChNotes
{
public:

     ChNotes() { Clear(); ClearFlags(); ErrStream = stderr; };

void Clear( void );
void ClearNew( void );
void ClearNew( int ch )
     {
		int note;
		for( note=0; note < MAX_NOTE_VAL ; note++ )
		{
		   Notes[ch][note].ClearNew();
		}
     };


int  Inc( int ch, int note )
     {
		if( ch > MAX_CHANNEL || ch < 0 || note > MAX_NOTE_VAL || note < 0 )
			return -1;
		else
		{
			SpeedupChFlag[ch] = 0;		// reset speetup flag
			return Notes[ch][note].Inc();
		}
     };
int  Get( int ch, int note  )
     {
		if( ch > MAX_CHANNEL || ch < 0 || note > MAX_NOTE_VAL || note < 0 )
			return -1;
		else
			return Notes[ch][note].Get();
     };
int  Dec( int ch, int note  )
     {
		if( ch > MAX_CHANNEL || ch < 0 || note > MAX_NOTE_VAL || note < 0 )
			return -1;
		else
			return Notes[ch][note].Dec();
     };
int  GetNew( int ch, int note  )
     {
		if( ch > MAX_CHANNEL || ch < 0 || note > MAX_NOTE_VAL || note < 0 )
		{
			return -1;
		}
		else
		{
			return Notes[ch][note].GetNew();
		}
     };

void ClearNew( int ch, int note	)
     {
		Notes[ch][note].ClearNew();
     };

int  DecNew( int ch, int note  )
     {
		if( ch > MAX_CHANNEL || ch < 0 || note > MAX_NOTE_VAL || note < 0 )
		{
			return -1;
		}
		else
		{
			return Notes[ch][note].DecNew();
		}
     };

void	SetErrStream( FILE *stream ) { ErrStream = stream; };
int		SetChNotes( int notes[MAX_CHANNEL] );
void	SetMinMax();
int		GetMax( int Channel ) { return MaxVal[ Channel ]; };
int		GetMin( int Channel ) { return MinVal[ Channel ]; };
int		GetNewMax( int Channel ) { return NewMax[ Channel ]; };
int		GetNewMin( int Channel ) { return NewMin[ Channel ]; };
void	NewTime( void );

int     Get( int NoteValue );  // Total number of notes whith given value
int     GetNew( int NoteValue );
int		GetMax() { return TotalMaxVal; };
int		GetMin() { return TotalMinVal; };
int		GetNewMax() { return TotalNewMax; };
int		GetNewMin() { return TotalNewMin; };

private:

FILE	*ErrStream;

Note	Notes[MAX_CHANNEL][MAX_NOTE_VAL];
int		SpeedupChFlag[ MAX_CHANNEL ];
int		SpeedupChNote[ MAX_CHANNEL ];

// Array's for the min max Values of the aktuell Note Informationes
//
// use MinMax() funktion to init the Value's

int		MaxVal[ MAX_CHANNEL ];
int		MinVal[ MAX_CHANNEL ];
int		NewMax[ MAX_CHANNEL ];
int		NewMin[ MAX_CHANNEL ];
int     TotalMinVal, TotalMaxVal;
int     TotalNewMin, TotalNewMax;
void	ClearFlags( void );

protected:

};


#endif	// _CH_NOTES_H_
