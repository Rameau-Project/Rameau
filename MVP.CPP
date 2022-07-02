/*-------------------------------------------------------------------------
  Dieter Neubacher           Vers.: 1.00                     Wuhu, 22-06-94
                             vers.  1.10                           16.07.96
-----------------------------------------------------------------------
    mvp.cpp

    Vers.: 1.10    korrektur   GetModDiff in mvp.h
                   korrektur   Qunitenverhaeltnisse
-----------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mvp.h"
#include "MvpEntry.h"



#define SPEED_UP        // Speedup notebuffer excess

#define MVP_OUT_FILE_WRITE_ERROR -1


// debug options

// #define DEBUG_NEW_NOTE



/*-------------------------------------------------------------------------
    Functions for r/w RDL files
---------------------------------------------------------------------------
*/

void Mvp::Init()
{
int i;

    MvpErrorEntry.SetError();
    for( i=0; i<MVP_MAX_CHANNEL; i++ )
    {
      readChStatus[i][0] = readChStatus[i][1] = 0;
      #ifdef SPEED_UP
      SpeedupChFlag[ i ] = 1;
      #else
      SpeedupChFlag[ i ] = 0;
      #endif
      SpeedupChNote[ i ] = 0;
    }
    flag_all_notes = 0; /* print all notes */
    ClearChFlags();
    ClearNoteBuf();
    ClearHisto();
    InitBar( 0 );
    CopyMsgLine = 0;
    CopyTxtLine = 1;
}

Mvp::Mvp()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    Init();
}

Mvp::Mvp(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = inFile;
    OutStream = outFile;
    Init();
}

Mvp::Mvp(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
    Init();
}

Mvp::Mvp(char *inFile, char *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

Mvp::Mvp(char *inFile, char *outFile, char *errFile, char *msgFile )
{
    ErrStream = OpenErrFile( errFile );
    MsgStream = OpenMsgFile( msgFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}
void Mvp::DispAllNotes( void )
{
    flag_all_notes = 1; /* print all notes */
}
//----------------------------------
//  error handler for rdl
//----------------------------------

void Mvp::Error(int errNum )
{
   switch( errNum )
   {
     case MVP_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit MVP while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;

     default :
              fprintf( ErrStream, "Exit MVP while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}
/*------------------------------------
--------------------------------------
*/

FILE * Mvp::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r" );
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! MVP input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * Mvp::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w" );
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MVP output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * Mvp::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w" );
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MVP error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * Mvp::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! MVP massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}

/*-------------------------------------------------------------------------
     read mvp line
---------------------------------------------------------------------------
*/

long Mvp::ReadMvpLine( void )
{
long      time;
int       tmpBar;
int       i = 0, line_size, bufPos = 0, error=0, EntryCount;
MvpEntry  entry;

   if ((line_size = ReadLine ()) <= 0)
      return (-1L);

   /* commentar mark or note bar mark */

   while (InBuf[0] == '*' || InBuf[0] == '#')
   {
     if (InBuf[0] == '*' && MsgStream != NULL )
     {
        if (CopyTxtLine)          /* copy commentar */
        {
           fwrite (InBuf, 1, line_size, MsgStream);
        }
     }
     else  /* note bar mark */
     if (InBuf[0] == '#' )
     {
       // Get bar info

       if( strncmp( "# bar ", InBuf, 6 ) == 0 )
       {
          if( sscanf( InBuf + 6, "%d", &tmpBar ) != 1 )
          {
             fprintf (ErrStream, "MVP read bar info format error\n");
          }
          else
          {
             SetBar( tmpBar );
             NewTime();
          }
       }
       if (CopyMsgLine)         /* copy massage */
       {
         fwrite (InBuf, 1, line_size, OutStream);
       }
     }

     /* read next line */

     if ((line_size = ReadLine ()) <= 0)
         return (-1L);
   }
   if (sscanf (InBuf, "%ld", &time) != 1)
   {
      fprintf (ErrStream, "file read error\n");
      return (-1L);
   }
   bufPos += 10;                /* read time and blank */

   // read MvpEntries
   ReadPairCols = 0;           // counter for 1/1 1/2 1/3 2/2 ..
   while( InBuf[bufPos] == '(' )
   {
      EntryCount = 0;          // entries per Block z.B. (1/3)
      do
      {
         bufPos++;
         entry.StrToEntry( InBuf+bufPos );
         bufPos += MVP_ENTYRY_STR_SIZE;

         // entry.Write( stderr );
         // fprintf( stderr, "<-- ReadMvpLine Col %d Count %d \n", ReadPairCols, EntryCount );


         if( InBuf[bufPos] != ';' )     // multiple entry ?
         {
            if( InBuf[bufPos] != ')' )  // singel entry
            {
               fprintf( ErrStream, "MVP input file : invalid format\n" );
               return( -2L );
            }
            bufPos++;
         }
         // store entry

         if( EntryCount < MVP_MAX_READ_ENTRIES_PER_COL )
         {
            ReadEntryBuf[ ReadPairCols ][ EntryCount++ ] = entry;
            ReadEntryBufNum[ ReadPairCols ] = EntryCount;
         }
         else
         {
            fprintf( ErrStream, "to many MVP-Entries in colum %d\n", ReadPairCols );
         }

      }while( InBuf[bufPos] == ';' );

      // next channel pairs

      ReadPairCols++;

   }

   if( InBuf[bufPos] != '\n' )
   {
      fprintf( ErrStream, "MVP input file : invalid format\n" );
      return( -2L );
   }

   return (time);
}
//----------------------------------
//  get entry form Read Enty Buffer
//----------------------------------

MvpEntry Mvp::GetEntry( int line, int num )
{

   if( line < ReadPairCols && num < ReadEntryBufNum[ line ] )
   {
       return ReadEntryBuf[ line ][ num ];
   }
   else
   {
       return MvpErrorEntry;
   }

}

//----------------------------------

int Mvp::SetChFlag( int channel )
{
   if( channel < 0 || channel >= MVP_MAX_CHANNEL )
   {
       return -1;
   }
   UsedChFlag[ channel ] = 1;
   return 0;

}

int Mvp::ResetChFlag( int channel )
{
   if( channel < 0 || channel >= MVP_MAX_CHANNEL )
   {
       return -1;
   }
   UsedChFlag[ channel ] = 0;
   return 0;

}

void Mvp::ClearChFlags( void )
{
int i;

   for( i=0; i<MVP_MAX_CHANNEL; i++)
   {
        UsedChFlag[ i ] = 0;
   }

}

/*---------------------------------------------------------------------
    write mvp line
-----------------------------------------------------------------------
*/


int Mvp::Write (long time)
{
int  channel, subchannel ;

    fprintf (OutStream, "%9ld ", time);

    for (channel = 0; channel < MVP_MAX_CHANNEL; channel++)
    {
       for (subchannel = channel + 1; subchannel < MVP_MAX_CHANNEL; subchannel++)
       {
          if (UsedChFlag[channel] && UsedChFlag[subchannel])
          {

             fprintf (OutStream, "[");
             WriteChInfo (channel);
             fprintf (OutStream, ";");
             WriteChInfo (subchannel);
             fprintf (OutStream, "]");
          }
       }
    }
    fprintf (OutStream, "\n");
   return 0;
}
/*---------------------------------------------------------------------
  set note informations
-----------------------------------------------------------------------
*/
int  Mvp::SetNotes( long time, int notes[MVP_MAX_CHANNEL] )
{
int ret = 0;
int channel;

   #ifdef DEBUG_NEW_NOTE
       printf( "\n");
   #endif

   for (channel = 0; channel < MVP_MAX_CHANNEL; channel++)
   {
      if (notes[channel] > MAX_NOTE_VAL || notes[channel] < -MAX_NOTE_VAL)
      {
         fprintf (ErrStream, "invalid note info %d at time %ld\n", notes[channel], time);
         ret++;
      }
      else if (notes[channel] != 0)
      {
         if (notes[channel] > 0)
         {
            Notes.Inc(channel , notes[channel] );

            #ifdef DEBUG_NEW_NOTE
               printf( "SET NOTE CH %d NOTE %d\n",channel , notes[channel] );
            #endif

            #ifdef SPEED_UP
            SpeedupChFlag[ channel ] = 0;
            for( int i = 0; i < MAX_NOTE_VAL; i++)
            {
               if( Notes.Get( channel, i ) )
               {
                  SpeedupChNote[ channel ] = i;
                  break;  // exit for loop
               }

            }
            #endif
         }
         else
         {
            if ( Notes.Dec( channel , -notes[channel] ) < 0)
            {
               fprintf (ErrStream, "note off error %d at time %ld\n", notes[channel], time);
               ret++;
            }
            #ifdef SPEED_UP
            SpeedupChFlag[ channel ] = 1;
            for( int i = 0; i < MAX_NOTE_VAL; i++)
            {
               if( Notes.Get( channel, i ) )
               {
                  SpeedupChFlag[ channel ] = 0;
                  SpeedupChNote[ channel ] = i;
                  break;  // exit for loop
               }

            }
            #endif
         }
      }
   }

   #ifdef DEBUG_NEW_NOTE
   DebugDispNoteBuffer();
   #endif

   return ret;
}
/*---------------------------------------------------------------------
    functions for reading note Informatins form ChannelNotes
-----------------------------------------------------------------------
*/

int    Mvp::initReadChannel( int channel )
{
       readChannel = channel;

       return( initChannel( channel ));
}

int    Mvp::initReadSubChannel( int channel )
{
       readSubChannel = channel;

       return( initChannel( channel ));
}

int    Mvp::initChannel( int channel )
{
       if( channel < 0 || channel >= MVP_MAX_CHANNEL )
       {
          return -1;
       }

       if( UsedChFlag[ channel ] != 1 )
       {
         return -1;
       }
       /* Status info ..[0] note, ..[1] noteCount */
       readChStatus[channel][0] = SpeedupChNote[ channel];
       readChStatus[channel][1] = 0;

       return 0;
}

int    Mvp::ReadChNote( void )
{
       if( SpeedupChFlag[ readChannel ] )
       {
          return -1;
       }

       /* if noteCount != noteCount( cannelNotes ) */

       if( Notes.Get( readChannel, readChStatus[readChannel][0] ) > readChStatus[readChannel][1] )
       {
          readChStatus[readChannel][1]++;
          return readChStatus[readChannel][0];
       }
       else /* search next channeNote */

       while( ++readChStatus[readChannel][0] < MAX_NOTE_VAL )
       {
          readChStatus[readChannel][1] = 0;
          if( Notes.Get(readChannel, readChStatus[readChannel][0] ) > readChStatus[readChannel][1] )
          {
             readChStatus[readChannel][1]++;
             return readChStatus[readChannel][0];
          }
       }
       return -1;
}

int    Mvp::ReadSubChNote( void )
{
       if( SpeedupChFlag[ readSubChannel ] )
       {
          return -1;
       }

       /* if noteCount != noteCount( cannelNotes ) */

       if( Notes.Get( readSubChannel, readChStatus[readSubChannel][0] ) > readChStatus[readSubChannel][1] )
       {
          readChStatus[readSubChannel][1]++;
          return readChStatus[readSubChannel][0];
       }
       else /* search next channeNote */

       while( ++readChStatus[readSubChannel][0] < MAX_NOTE_VAL )
       {
          readChStatus[readSubChannel][1] = 0;
          if( Notes.Get( readSubChannel, readChStatus[readSubChannel][0] ) > readChStatus[readSubChannel][1] )
          {
             readChStatus[readSubChannel][1]++;
             return readChStatus[readSubChannel][0];
          }
       }
       return -1;

}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int Mvp::WriteChInfo (int n)
{
   int     i, count;
   int     flag_all_notes = 1;
   int     p_flag = 0;

   for (i = 0; i < MAX_NOTE_VAL; i++)
   {
      if ( Notes.Get( n, i ) > 0)
      {
         p_flag = 1;
         count = 0;
         if (flag_all_notes)
         {
            do
            {
               fprintf (OutStream, "%3d ", i);
            }
            while (++count < Notes.Get( n,i) );
         }
         else
         {
            fprintf (OutStream, "%3d ", i);
         }
      }
   }
   if (p_flag == 0)
   {
      fprintf (OutStream, "    ");
   }

   return 0;
}
/*---------------------------------------------------------------------
   write pair infos ( Oktaven, Quinten )
-----------------------------------------------------------------------
*/
int  Mvp::WritePairs( long time )
{
int  channel, subchannel ;

    fprintf (OutStream, "%9ld ", time);

    for (channel = 0; channel < MVP_MAX_CHANNEL; channel++)
    {
       for (subchannel = channel + 1; subchannel < MVP_MAX_CHANNEL; subchannel++)
       {
          if (UsedChFlag[channel] && UsedChFlag[subchannel])
          {
             fputc( '(', OutStream );

             #ifdef DEBUG_NEW_NOTE
                 printf("---- Write Pair info ----\n");
                 DebugDispNoteBuffer();
             #endif

             WritePairColorInfo( channel, subchannel );

             fputc( ')', OutStream );
          }
       }
    }

    if( ferror( OutStream ) )
    {
        Error( MVP_OUT_FILE_WRITE_ERROR );
    }
    return 0;
}

/*---------------------------------------------------------------------

    Write (StimmenPaarAbschnitt)SPA-Color informations

-----------------------------------------------------------------------
*/

int Mvp::WritePairColorInfo( int ch1, int ch2)
{
int      pFlag=0, ch1Note,ch2Note;
MvpEntry mvpE, mvpOld;

   /* analyse pairs */

   initReadChannel( ch1 );
   initReadSubChannel( ch2 );

   ch1Note=ReadChNote();
   ch2Note=ReadSubChNote();

   while( ch1Note != -1 )
   {
      if( ch2Note == -1 )              // Break while channel 2 no data
      {
         mvpE.Clear();
         mvpE.SetCh1Note( ch1Note );
         mvpE.SetCh2Note( ch2Note );
         mvpE.SetStatus( MVP_BREAK);
         setHisto( ch1,ch2, mvpE );
         if( pFlag == 1 )
         {
            fputc( ';',OutStream );         // write entry separator
         }
         mvpE.Write( OutStream );
         pFlag=1;
         initReadSubChannel( ch2 );    // int sub channel read
         ch2Note=ReadSubChNote();
         ch1Note=ReadChNote();
         continue;                     // goto first loop
      }

      while( ch2Note != -1 )
      {
         mvpE.Clear();
         mvpE.SetCh1Note( ch1Note );
         mvpE.SetCh2Note( ch2Note );
         mvpE.SetOColor( MVP_NO_COLOR );
         mvpE.SetStatus( MVP_NONE     );


         if( ( Notes.GetNew( ch1, ch1Note ) <= 0 ) &&
             ( Notes.GetNew( ch2, ch2Note ) <= 0 )
           )
         {
            // this is an old value

            ch2Note = ReadSubChNote();

            goto MVP_WRITE_MARK;    // write data then continue while loop

         }

         /* Einklang Oktave mod = 0 */

         if( mvpE.GetModDiff() == 0 )
         {
            mvpE.SetStatus( MVP_OKTAVE );
            mvpOld = getLastE(ch1,ch2);

            if( pFlag == 1 )  // Multiple entry
            {
            }
            else
            if( mvpOld.GetStatus() == MVP_OKTAVE )
            {
              // old status is oktave; new data ?

                // old status is oktave

                if( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 0 )
                {
                   // new and old oktave is a oktave

                   mvpE.SetOColor( MVP_BLUE );
                   setHisto( ch1,ch2, mvpE );

                }
                else
                if( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 5 ||
                    ModDiff( ch1Note,mvpOld.GetCh2Note() ) == 5 ||
                    ModDiff( ch2Note,mvpOld.GetCh1Note() ) == 5 ||
                    ModDiff( ch2Note,mvpOld.GetCh2Note() ) == 5
                  )
                {
                   // new quint

                   mvpE.SetOColor( MVP_BLUE );
                   setHisto( ch1,ch2, mvpE );
                }
                else
                if( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 7 ||
                    ModDiff( ch1Note,mvpOld.GetCh2Note() ) == 7 ||
                    ModDiff( ch2Note,mvpOld.GetCh1Note() ) == 7 ||
                    ModDiff( ch2Note,mvpOld.GetCh2Note() ) == 7
                  )
                {
                   // new quart

                   mvpE.SetOColor( MVP_BLUE );
                   setHisto( ch1,ch2, mvpE );
                }
                else
                {
                   mvpE.SetOColor( MVP_RED );
                   setHisto( ch1,ch2, mvpE );
                }
            }
            else
            {
               setHisto( ch1,ch2, mvpE );
            }
         }
         else                                 /* Quinte mod = 7 */
         if( mvpE.GetModDiff() == 7 )
         {
            mvpE.SetStatus( MVP_QUINTE );
            mvpOld = getLastQ(ch1,ch2);

            if( pFlag == 1 )  // Multiple entry
            {
            }
            else
            if( mvpOld.GetStatus() == MVP_QUINTE )
            {   
                #ifdef  OLD_VERS_1_0
                /*--- old  Ver. 1.0 -----------------------------------------------------------
                if( ( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 0 && ModDiff( ch2Note,mvpOld.GetCh2Note() ) == 0 ) ||
                    ( ModDiff( ch2Note,mvpOld.GetCh1Note() ) == 0 && ModDiff( ch1Note,mvpOld.GetCh2Note() ) == 0 )
                  )
                {
                   if( ch1Note == mvpOld.GetCh1Note()  &&
                       ch2Note == mvpOld.GetCh2Note()
                     )
                   {
                      mvpE.SetQColor( MVP_GREEN );      /* Einklang oder oktave */
                      setHisto( ch1,ch2, mvpE );
                   }
                   else
                   {
                      /* old entry */

                      mvpE.SetQColor( MVP_NO_COLOR );
                      mvpE.SetStatus( MVP_NONE  );
                   }
                }
                ----------------------------------------------------------------------------------------------------*/
                #endif
                if( ( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 0 && ModDiff( ch2Note,mvpOld.GetCh2Note() ) == 0 ))
                {
                      mvpE.SetQColor( MVP_GREEN );      /* Einklang oder oktave */
                      setHisto( ch1,ch2, mvpE );
                }
                else
                if( ModDiff( ch1Note,mvpOld.GetCh1Note() ) == 0 ||
                    ModDiff( ch1Note,mvpOld.GetCh2Note() ) == 0 ||
                    ModDiff( ch2Note,mvpOld.GetCh1Note() ) == 0 ||
                    ModDiff( ch2Note,mvpOld.GetCh2Note() ) == 0
                  )
                {
                   mvpE.SetQColor( MVP_GREEN );  /* mind. ein Ton gemeinsam */
                   setHisto( ch1,ch2, mvpE );
                }
                else
                {
                   mvpE.SetQColor( MVP_RED );
                   setHisto( ch1,ch2, mvpE );
                }
            }
            else
            {
               setHisto( ch1,ch2, mvpE );
            }
         }
         else
         {
            mvpE.SetStatus( MVP_NONE );
         }

MVP_WRITE_MARK :

         if( pFlag == 1 )
         {
            fputc( ';',OutStream );         // write entry separator
         }

         mvpE.Write( OutStream );
         pFlag=1;
         ch2Note=ReadSubChNote();           // get sub channel note info
         mvpE.Clear();

      } // end of while loop

      initReadSubChannel( ch2 );            // init sub Channel for next loop
      ch2Note=ReadSubChNote();
      ch1Note=ReadChNote();

   }
   if( pFlag == 0 )
   {
      mvpE.SetCh1Note( ch1Note );
      mvpE.SetCh2Note( ch2Note );
      mvpE.SetStatus( MVP_BREAK );
      setHisto( ch1,ch2, mvpE );
      mvpE.Write( OutStream );
   }

   return 0;
}

/*---------------------------------------------------------------------
    differenz mod 12
-----------------------------------------------------------------------
*/
int Mvp::ModDiff( int a,int b)
{
   if( a > b )
   {
      return (a-b) % 12;
   }
   else
   {
      return (b-a) % 12;
   }
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int Mvp::Disp ( void )
{
   long    time;

   while ((time = ReadMvpLine()) >= 0L)
   {

      Write( time );
   }
   Write (-1L);
   return 0;

}


/*---------------------------------------------------------------------
   set infos in History area

   pairHistory[][0]     ENTRY  used for oktave

   pairHistory[]1]      ENTRY  used for quinte,quarte

-----------------------------------------------------------------------
*/

int Mvp::setHisto( int ch1, int ch2, MvpEntry pair )
{
    switch( pair.GetStatus() )
    {
      case MVP_BREAK : /* clear histo information */
                       pairHistory[ histoIndex( ch1, ch2) ][0] = pair;
                       pairHistory[ histoIndex( ch1, ch2) ][1] = pair;
                       break;
      case MVP_OKTAVE: /* set OKTAVE to histo */
                       pairHistory[ histoIndex( ch1, ch2) ][0] = pair;
                       break;
      case MVP_QUARTE: /* set QUARTE to histo */
      case MVP_QUINTE: /* set QUINTE to histo */
                       pairHistory[ histoIndex( ch1, ch2) ][1]  = pair;
                       break;
      default        :
                       fprintf( ErrStream, "setHisto() invalid argument\n");
                       return 1;
                       break;
    }

    return 0;
}

int Mvp::setHisto( int ch1, int ch2, int status, int Note1, int Note2 )
{
MvpEntry pair;

    pair.SetStatus  ( status );
    pair.SetCh1Note ( Note1  );
    pair.SetCh2Note ( Note2  );

    return setHisto( ch1,ch2, pair );
}

MvpEntry Mvp::getLastQ( int ch1, int ch2 )
{
   return pairHistory[ histoIndex( ch1, ch2) ][1];
}
MvpEntry Mvp::getLastE( int ch1, int ch2 )
{
   return pairHistory[ histoIndex( ch1, ch2) ][0];
}
/*---------------------------------------------------------------------
   calculate Index for History area
-----------------------------------------------------------------------
*/

int Mvp::histoIndex( int ch1, int ch2 )
{
static int init=0, index[MVP_MAX_CHANNEL]{};
int tmp;

   if( ch1 > ch2 )
   {
      tmp = ch1; ch1=ch2; ch2=tmp;
   }

   if( !init )
   {
      int i, count;

         for( count=0,i=0; i < MVP_MAX_CHANNEL; i++ )
         {
            index[i] = count;
            count += MVP_MAX_CHANNEL - i -1;
         }
   }

   return( index[ch1] + ch2 );
}
/*---------------------------------------------------------------------
   clear all entries in History area
-----------------------------------------------------------------------
*/

void Mvp::ClearHisto( void )
{
int i,j;
MvpEntry mvpE;

    mvpE.Clear();
    mvpE.SetStatus( MVP_BREAK );

    for( i=0; i<MVP_MAX_CHANNEL-1; i++ )
    {
      for( j=i+1; j<MVP_MAX_CHANNEL; j++ )
      {
         setHisto( i, j, mvpE );
      }
    }

}
/*---------------------------------------------------------------------
    Flash all infos while time is changed
-----------------------------------------------------------------------
*/
void Mvp::NewTime ( void )
{
    Notes.NewTime();
    NewBar();
}

void Mvp::NewBar ( void )
{
    // write bar info to outfile

    if( LastBar != Bar )
    {
       if( OutStream != NULL )
       {
          fprintf( OutStream,"# bar %d\n", Bar );
       }
       LastBar = Bar;
    }
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int Mvp::Close ( void )
{
   if( InStream  != NULL )    fclose (InStream);
   if( OutStream != NULL )    fclose (OutStream);
   if( ErrStream != NULL )    fclose (ErrStream);
   if( MsgStream != NULL )    fclose (MsgStream);

   return 0;
}

/*-------------------------------------------------------------------------
    read one line from InStream to InBuf
    set EOL at the end of the line
    Return : number of characters read sucsecfull
             of error  -1
---------------------------------------------------------------------------
*/

int Mvp::ReadLine (void)
{
   int     count = 0, ch;

   while (count < MVP_IO_BUF_SIZE)
   {
      switch (ch = fgetc (InStream))
      {
      case '\n':
         InBuf[count++] = '\n';
         return (count);
         break;

      case '\t':
         do
         {
            InBuf[count++] = ' ';
         }
         while( count % 8 );
         break;
      case '\r':
         break;
      case EOF:
         InBuf[count] = '\n';
         return 0;
         break;

      default:
         InBuf[count++] = ch;
         break;
      }
   }

   return (count);
}

/*--------------------------------------------------------------------------
    clear note buffer
----------------------------------------------------------------------------
*/

void Mvp::ClearNoteBuf( void )
{
  Notes.Clear();
}

/*--------------------------------------------------------------------------
    clear new counter in note buffer
----------------------------------------------------------------------------
*/

void Mvp::ClearNewNoteBuf( void )
{
  Notes.ClearNew();
}

/*--------------------------------------------------------------------------
    display note buffer information
----------------------------------------------------------------------------
*/

void Mvp::DebugDispNoteBuffer( void )
{
int note, i;

   for(i=0; i<4; i++)
   {
     for( note=0; note<128; note++ )
     {
      if( Notes.Get(i,note) )
      printf( "NoteBufferInfo ch: %d, note %d, count %d, newcount %d\n",
            i,
            note,
            Notes.Get( i, note ),
            Notes.GetNew( i, note )
           );
      }
     }
}
