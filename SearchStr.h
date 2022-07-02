/*----------------------------------------------------------
	Dieter Neubacher   
	--------------------------------------------------------
	SearchStr.h
------------------------------------------------------------
*/

#ifndef _SEARCHSTR_H_ 
#define _SEARCHSTR_H_ 

#include <stdio.h>
#include "TonInt.h"

#define SEARCH_DATA_LIST_RESIZE_VALUE 4

// ---------------------------------------
// Found() Print Output to SearchStrStream
// !!!!! must by extern init. !!!!
//----------------------------------------

extern FILE *SearchStrStream;

#define SEARCH_TRANS_FLAG	0x0001
#define SEARCH_REVERS_FLAG	0x0002
#define SEARCH_PERMUT_FLAG	0x0004

class SearchParameter
{
public:
		SearchParameter() { flags = trans = 0; };

void	TransOn() { flags |= SEARCH_TRANS_FLAG; };
void	TransOff() { flags &= ~SEARCH_TRANS_FLAG;};
void	ReversOn() { flags |= SEARCH_REVERS_FLAG;};
void	ReversOff() { flags &= ~SEARCH_REVERS_FLAG; };
void	PermutationOn() { flags |= SEARCH_PERMUT_FLAG;};
void	PermutationOff() { flags &= ~SEARCH_PERMUT_FLAG;};
int     GetRevers() { return flags & SEARCH_REVERS_FLAG; };
int     GetPermut() { return flags & SEARCH_PERMUT_FLAG; };
int     GetTrans() { return flags & SEARCH_TRANS_FLAG; };
int     GetFlags() {return flags; };
void	SetFlags( int Flags ) { flags = Flags; };
int     GetTransVal() { return trans; };
void	SetTransVal( int Trans ) { trans = Trans; };

private:

int trans;
int flags;
};

class SearchStr : public SearchParameter
{
public:
	
		SearchStr() { Pos = Length;  EndTime = 0; Time = NULL; Channel = NULL; Note = NULL; PermutVec = NULL;};
        SearchStr( int len, TonInterval *Tint, SearchParameter sparam )
		{
			SetFlags( sparam.GetFlags() );
			SetTransVal( sparam.GetTransVal() );
			Length =len;
			Time = new long[len];
			Channel = new int[len];
			Note = new int[len];
			PermutVec = NULL;
			Pos  = 0;
			Set( Tint ); 
		};
		~SearchStr() { delete Time; delete Channel; delete Note; delete PermutVec; };

int		GetPos() { return Pos; };
int		GetNote(int pos) { return Note[pos]; };
long	GetTime(int pos) { return Time[pos]; };
int		GetChannel(int pos) { return Channel[pos]; };
int		Found();
int		Print( FILE *stream );
long	GetEndTime() {return EndTime; };
int		Set( TonInterval *tint ); 
int     Copy( SearchStr *p );
int     SetPermutVec(  int min, int max, int *vec );
int 	GetPermutVal( int val ) { return PermutVec[ val ]; };
int 	SetPermutVal( int val ) { return --PermutVec[ val ]; };
int		PrintPermutVec( FILE *stream );

private:

int Pos, Length;

long EndTime;
long *Time;
int  *Channel;
int  *Note; 

int  *PermutVec; // used for calculation of permutations 

};


//----------------------------------------------------------
// Search Data List
//----------------------------------------------------------

class SearchData : public SearchParameter
{
public:

		SearchData() 
		{ 
			first=0; 
			last=0; 
			SearchDataListLength = 4; 
			List = new SearchStr * [SearchDataListLength]; 

			for( int i=0; i<SearchDataListLength; i++ )
			{
				List[i] = NULL;
			}
		};
		~SearchData() 
		{ 
		  delete SearchStringData; 
		  delete ReversSearchStringData; 
		  delete List; 
		};
int		SetData( TonInterval  *Tint );
int     CreateNewEntries( TonInterval  *Tint, int flags );
int		FreeListEntry(); 
int		TestAndSetData( SearchStr *sstr, TonInterval *tint ); 
int		SetSearchString( int len, int str[] );

private:

int SearchDataListLength;

int			length;
int			first, last;
int         *SearchStringData;
int         *ReversSearchStringData;
SearchStr   **List;
int			SearchVector[ MAX_NOTE_VAL+1 ];
int			SearchMin, SearchMax;

int         NewEntry( TonInterval  *Tint, SearchParameter param );

};


#endif // _SEARCHSTR_H_ 