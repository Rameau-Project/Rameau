/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 22-06-94
    -----------------------------------------------------------------------
    RamData.c
    Global Functions for RAMEAU DateFile read- und write-ing
    -----------------------------------------------------------------------

    What is to do :

    ReadRdl :
    pointer for cannel data not nice
      bufPos += 10;  * read time and blank *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rdl.h"



#define RDL_OUT_FILE_WRITE_ERROR -1



/*-------------------------------------------------------------------------
    Functions for r/w RDL files
---------------------------------------------------------------------------
*/

void Rdl::Init( void )
{
    CopyTxtLine = CopyMsgLine = 1;
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    InitBar( 0 );
}


Rdl::Rdl()
{
    Init();
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
}

Rdl::Rdl(FILE *inFile, FILE *outFile )
{
    Init();
    InStream  = inFile;
    OutStream = outFile;
}

Rdl::Rdl(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    Init();
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
}

Rdl::Rdl(char *inFile, char *outFile )
{
    Init();
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
}

Rdl::Rdl(char *inFile, char *outFile, char *errFile, char *msgFile )
{
    Init();
    ErrStream = OpenErrFile( errFile );
    MsgStream = OpenMsgFile( msgFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
}
/*------------------------------------
--------------------------------------
*/

FILE * Rdl::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r");
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! RDL input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * Rdl::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w");
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! RDL output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * Rdl::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w");
   if (ErrStream == NULL)
   {
      ErrStream = stderr;
      fprintf (ErrStream, "\n!!! RDL error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * Rdl::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! RDL massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}
//----------------------------------
//  error handler for rdl
//----------------------------------

void Rdl::Error(int errNum )
{
   switch( errNum )
   {
     case RDL_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit RDL while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;

     default :
              fprintf( ErrStream, "Exit RDL while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}

void Rdl::NewTime( void )
{
    WriteBuf (-1L, NULL);       /* flush rdl buffer */
    NewBar();
}

void Rdl::NewBar ( void )
{
    // write bar info to outfile

    if( LastBar != Bar )
    {
       fprintf( OutStream,"# bar %d\n", Bar );
       LastBar = Bar;
    }
}

/*-------------------------------------------------------------------------
     read line form temp file
     return : timeinfo
              timeinfo == -1 : end of file
---------------------------------------------------------------------------
*/

long Rdl::Read (int channel_nodes[16])
{
   long    time;
   int     i = 0, line_size, bufPos = 0;
   int     tmpBar;


   if ((line_size = ReadLine ()) <= 0)
      return (-1L);

   /* commentar mark or note bar mark */

   while (InBuf[0] == '*' || InBuf[0] == '#')
   {
     if( OutStream != NULL )
     {
        WriteBuf (-1L, NULL);   /* flush buffer */
     }
     if (InBuf[0] == '*' && MsgStream != NULL )
     {
        if (CopyTxtLine)          /* copy commentar */
        {
          fwrite (InBuf, 1, line_size, MsgStream);
        }
     }
     else  /* note bar mark */
     if (InBuf[0] == '#' /* vers. 1.4 && OutStream != NULL */)
     {
       // Get bar info

       if( strncmp( "# bar ", InBuf, 6 ) == 0 )
       {
          if( sscanf( InBuf + 6, "%d", &tmpBar ) != 1 )
          {
             fprintf (ErrStream, "RDL read bar info format error\n");
          }
          else
          {
             SetBar( tmpBar );
          }
       }
     }
     else
     if (InBuf[0] == '#' && CopyMsgLine) /* copy massage */
     {
         fwrite (InBuf, 1, line_size, MsgStream);
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

   for (i = 0; i < 16; i++)
   {
      if (strncmp (InBuf + bufPos, " ..", 3) == 0)
      {
         channel_nodes[i] = 0;
      }
      else
      {
         sscanf (InBuf + bufPos, "%3d", &channel_nodes[i]);
      }
      bufPos += 4;
   }
   return (time);

}


int Rdl::WriteBuf (long midi_clock, int *inchannel)
{
   static long LastMidiTime = -1L;
   static int BufPos = 0;
   int     i, MaxIndex;

   if (LastMidiTime == -1)
   {
      LastMidiTime = midi_clock;
   }

   if (LastMidiTime != midi_clock)
   {
      /* flash RDL Buffer */
      MaxIndex = CompressBuf (BufPos);

      for (i = 0; i < MaxIndex; i++)
      {
         Write (Buffer[i].time, Buffer[i].channel);
      }
      LastMidiTime = midi_clock;
      MaxIndex = BufPos = 0;
   }

   if (midi_clock == -1L)
   {
      LastMidiTime = midi_clock;
      MaxIndex = BufPos = 0;
   }
   else if (BufPos < RDL_BUF_SIZE)
   {
      /* write data to buffer */
      Buffer[BufPos].time = midi_clock;
      for (i = 0; i <= 16; i++)
      {
         Buffer[BufPos].channel[i] = inchannel[i];
      }
      BufPos++;
   }
   else
   {
      fprintf (ErrStream, "RDL Buffer overflow\n");
   }
   return 0;
}


int Rdl::CompressBuf (int BufPos)
{
   int     i, j, LastIndex[16], MaxIndex = 0;

   for (i = 0; i < 16; i++)
   {
      LastIndex[i] = 0;
   }

   for (i = 0; i < BufPos; i++)
   {
      for (j = 0; j < 16; j++)
      {
         if (Buffer[i].channel[j] != 0)
         {
            if (LastIndex[j] < i)
            {
               Buffer[LastIndex[j]].channel[j] = Buffer[i].channel[j];
               Buffer[i].channel[j] = 0;
            }
            if (++LastIndex[j] > MaxIndex)
            {
               MaxIndex = LastIndex[j];
            }
         }
      }
   }
   return MaxIndex;
}

/*---------------------------------------------------------------------
    write output line
    clock     1C  2C  3C  ... 16C
    xxxxxxxx #nnn#nnn#nnn ... #nnn
    10 + 16*4 = 74 byte / line
-----------------------------------------------------------------------
*/


int Rdl::Write (long midi_clock, int channel[16] )
{
   int     i;

   fprintf (OutStream, "%9ld ", midi_clock);

   for (i = 0; i < 16; i++)
   {
      if (channel[i] == 0)
      {
         fprintf (OutStream, "%s", " .. ");
      }
      else
      {
         fprintf (OutStream, "%+4d", channel[i]);
      }
   }
   fprintf (OutStream, "\n");

   if( ferror( OutStream ) )
   {
      Error( RDL_OUT_FILE_WRITE_ERROR );
   }

   return 0;
}


int Rdl::Disp ( void )
{
   long    time;
   int     channel_nodes[16];


   while ((time = Read (channel_nodes)) >= 0L)
   {
      WriteBuf (time, channel_nodes);
   }
   WriteBuf (-1L, channel_nodes);
   return 0;


}

int Rdl::Disp ( int *ChannelOrder )
{
   long    time;
   int     InChNotes[16],OutChNotes[16];
   int     channel;

   while ((time = Read (InChNotes)) >= 0L)
   {

      // write notes in new order

      for( channel=0; channel < 16; channel++ )
      {
         if( ChannelOrder[channel] == -1 )
         {
            OutChNotes[ channel ] = 0;
         }
         else
         {
            OutChNotes[ channel ] = InChNotes[ ChannelOrder[channel] ];
         }
      }

      WriteBuf (time, OutChNotes);
   }
   WriteBuf (-1L, OutChNotes);
   return 0;


}


int Rdl::Close ( void )
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

int Rdl::ReadLine (void)
{
   int     count = 0, ch;

   while (count < IO_BUF_SIZE)
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
