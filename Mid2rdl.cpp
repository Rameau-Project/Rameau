/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                        26.02.93
                                   1.1                        28.06.94
                                   1.2                        22.02.95
                                   1.4                        03.06.96
								   1.5                        20.06.97 
    -------------------------------------------------------------------
    mid2rdl.c                                                 
    
    Vers.: 1.2       Use *.BAR file for bar informations
    Vers.: 1.4       erase tmp-files (if TempFileFlag == 0 )
                     option -d
                     Infoline in RDL file
	Vers.: 1.5       Use system Temp-Files
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>                              

#include "rameau.h"
#include "version.h" 
#include "midi.h"
#include "rdl.h" 


#define PROG_NAME   "mid2rdl"


/*-----------------------------------*/
/* debug note in C, CIS, D, DIS, ... */
/* if note_code_flag == 1            */
/*-----------------------------------*/

/*#define DEBUG_NOTE_CODE
*/
/*#define DEBUG_NEXT_BAR_TIME
*/


/*---------------------------*/
/* debug note on / off       */
/*---------------------------*/

/*
#define NOTE_DEBUG
*/
/*---------------------------*/
/* debug read_header_chunk() */
/*---------------------------*/

/*
#define RHC_DEBUG
*/
/*---------------------------*/
/* debug read_var_len()      */
/*---------------------------*/

/*
#define RVL_DEBUG
*/
/*---------------------------*/
/* debug read_track_chunk()  */
/*---------------------------*/

/*
#define RTC_DEBUG
*/
/*---------------------------*/
/* debug read_mtrk_event()   */
/*---------------------------*/

/*
#define RTE_DEBUG
*/
/*---------------------------*/
/* debug read_mtrk_event()   */
/* debug data input          */
/*---------------------------*/

/*
#define RTE_INPUT_DEBUG
*/
/*---------------------------*/
/* debug read meta event's   */
/*---------------------------*/

/*
#define ME_DEBUG
*/
/*---------------------------*/
/* debug time information    */
/*---------------------------*/

/*
#define TIME_DEBUG
*/


#define RDL_FLUSH  0x0100       /* flush output buffers */
#define INIT_RDL_OUTPUT -1      /* init new rdl output */

/*---------------------------*/
/*  function definitions     */
/*---------------------------*/


void    mid_to_rdl_usage (void);
int     write_output_line (FILE * stream, long midi_clock, int *channel);
int     input_read_error (void);
long    read_var_len (FILE * stream);
char   *read_n_byte (FILE * stream, long n);
int     read_header_chunk (FILE * stream);
int     read_track_chunk (FILE * stream);
int     read_mtrk_event (FILE * stream);
int     rdl_output (long time, int mode, int channel, int note);
int     read_chunk_info (FILE * stream);
long    read_line (FILE * tmp_stream, int channel_nodes[16]);
int     my_getc (FILE * stream);
long    to32bit (int c1, int c2, int c3, int c4);
int     to16bit (int c1, int c2);
long    read32bit (FILE * stream);
int     read16bit (FILE * stream);
/* Vers.: 1.2 */                     
long    GetNextBarTime( FILE * stream);                     

/*------------------*/
/* global variables */
/*------------------*/
long    GlobalTime = 0;
uint    NodeFlag = 0;
int     channel, channel_buf[16];
int     GlobalChannel = 0;
int     GlobalChannelFlag = 0;
long    TrackByteCount;
FILE    *in_stream = NULL, *out_stream = NULL, *err_stream = stderr;
int         FromBar = 0, ToBar = MAX_BAR_NUMBER;
int     TicksPerQuaterNote;
long    NextBarTime;
int     BarFlag = 0;
int     BarCount = 1;
int     VerboseFlag  = 0;
int     ReadLineFlag = 0;

#ifdef TEMP_BAR_INFO
long    BarTime;
#endif                    
                    
/*------------------------*/
/* print note codes       */
/*------------------------*/

int     note_code_flag = 0;

/*------------------*/
/* global variables */
/*------------------*/

char    note_code[12][5] =
{
   " C  ",                      /*  0 */
   " CIS",                      /*  1 */
   " D  ",                      /*  2 */
   " DIS",                      /*  3 */
   " E  ",                      /*  4 */
   " F  ",                      /*  5 */
   " FIS",                      /*  6 */
   " G  ",                      /*  7 */
   " GIS",                      /*  8 */
   " A  ",                      /*  9 */
   " AIS",                      /* 10 */
   " H  "                       /* 11 */
};


/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
mid_to_rdl_usage ()
{
   fprintf (err_stream, "usage : %s [flags] [%s_file_name]\n", PROG_NAME, INP_EXT);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags : -? this output\n");
   fprintf (err_stream, "      : -h this output\n");
   fprintf (err_stream, "      : -v disp program version\n");
   fprintf (err_stream, "      : -o output to stdout\n");
   fprintf (err_stream, "      : -i input from stdin\n");
   fprintf (err_stream, "      : -l Disp Read line numbers\n");
   fprintf (err_stream, "      : -b=x/y off beat Time\n");
   fprintf (err_stream, "      : -s=x/y Time Signature\n");
   fprintf (err_stream, "      : -f=nnn Nots form\n");
   fprintf (err_stream, "      : -t=nnn Nots to\n");
// fprintf (err_stream, "      : -d don't erase tmp-files\n");
#ifdef DEBUG_NOTE_CODE
   fprintf (err_stream, "      : -n note information in output file\n");
#endif /*  */
   fprintf (err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int my_getc (FILE * stream)
{
   int     ch;

   if ((ch = getc (stream)) == EOF)
   {
      /* END OF FILE */
      fprintf (err_stream, "END OF FILE\n");
      exit (0);
      return 1;
   }
   else
   {
      return (ch);
   }

}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

long
read_var_len (FILE * stream)
{
   register long value;
   register char c;

#ifdef TEST_END_OF_LINE
   if ((value = getc (stream)) == EOF)
   {
      fprintf (err_stream, "read_var_len() value = end of file\n");
      exit (1);
   }
#else /* TEST_END_OF_LINE  */
   value = getc (stream);
#endif /* TEST_END_OF_LINE */
   if (value & 0x80)
   {
      value &= 0x7f;
      do
      {
         if ((c = getc (stream)) == EOF)
         {
            fprintf (err_stream, "read_var_len() c = end of file\n");
            break;
         }
         value = (value << 7) + (c & 0x7f);
      }
      while (c & 0x80);
   }
#ifdef RVL_DEBUG
   fprintf (err_stream, "read var len %8x\n", value);
#endif /* RVL_DEBUG */
   return (value);

}


/*---------------------------------------------------------------------
    read n bytes from midi-stream to buffer
-----------------------------------------------------------------------
*/

char   *read_n_byte (FILE * stream, long n)
{
   char   *ret = NULL;
   int     i;

   if ((ret = (char *) malloc (sizeof (char) * (n + 1))) == NULL)
   {
      fprintf (err_stream, "no more memory\n");
      for (i = 0; i < n; i++)
          my_getc (stream);
   }
   else
   {
      for (i = 0; i < n; i++)
             ret[i] = my_getc (stream);
      ret[n] = 0;
   }
   return (ret);

}


long
to32bit (int c1, int c2, int c3, int c4)
{
   long    value = 0L;

   value = (c1 & 0xff);
   value = (value << 8) + (c2 & 0xff);
   value = (value << 8) + (c3 & 0xff);
   value = (value << 8) + (c4 & 0xff);
   return (value);
}


int
to16bit (int c1, int c2)
{
   return ((c1 & 0xff) << 8) + (c2 & 0xff);
}


long
read32bit (FILE * stream)
{
   int     c1, c2, c3, c4;

   c1 = my_getc (stream);
   c2 = my_getc (stream);
   c3 = my_getc (stream);
   c4 = my_getc (stream);
   return to32bit (c1, c2, c3, c4);
}


int
read16bit (FILE * stream)
{
   int     c1, c2;
   c1 = my_getc (stream);
   c2 = my_getc (stream);
   return to16bit (c1, c2);
}




/*---------------------------------------------------------------------
    Read Mid Header Chunk

    return ntrks   Number of Midi Tracks
-----------------------------------------------------------------------
*/

int
read_header_chunk (FILE * stream)
{
   long    length;
   int     type, ntrks, division;
   char   *str;

#ifdef DHC_DEBUG
   fprintf (err_stream, "read_header_chunk()\n");
#endif /* DHC_DEBUG */
   /* read chunk type */
   str = read_n_byte (stream, 4);

   if (strncmp (str, "MThd", 4))
   {
      fprintf (err_stream, "read_header_chunk() invalid chunk type : %s\n", str);
   }
   free (str);

   if (feof (stream))
   {
      fprintf (err_stream, "read_header_chunk() end of file\n");
      exit (1);
   }

   /* read length */
   length = read32bit (stream);

#ifdef RHC_DEBUG
   fprintf (err_stream, "length : %8ld\n", length);
#endif /* RHC_DEBUG */
   /* read type */
   my_getc (stream);            /* first byte  */
   type = my_getc (stream);     /* second byte */

   switch (type)
   {
   case 0:
#ifdef DHC_DEBUG
      fprintf (err_stream, "singel multi-channel track\n");
#endif /* DHC_DEBUG */
      break;
   case 1:
#ifdef DHC_DEBUG
      fprintf (err_stream, "one or more simultaneous tracks\n");
#endif /* DHC_DEBUG */
      //GlobalChannelFlag = 1;
      break;
   case 2:
#ifdef DHC_DEBUG
      fprintf (err_stream, "one or more simultaneous independent single-track patterns\n");
#endif /* DHC_DEBUG */
      fprintf (err_stream, "MIDI-FILE-TYPE : one or more simultaneous independent single-track patterns\n");
      exit (0);
      break;
   }

   /* read ntrks */
   ntrks = read16bit (stream);

#ifdef RHC_DEBUG
   fprintf (err_stream, "ntrks : %d\n", ntrks);
#endif /* RHC_DEBUG */
   /* read division */
   division = read16bit (stream);

#ifdef RHC_DEBUG
   fprintf (err_stream, "division : %04x\n", division);
#endif /* RHC_DEBUG */
   if (division & 0x8000)
   {
#ifdef RHC_DEBUG
      fprintf (err_stream, "negativ SMPTE format %d\n", ((division & 0x7F00) >> 8));
      fprintf (err_stream, "ticks per frame %d\n", (division & 0xFF));
#endif /* RHC_DEBUG */
      fprintf (err_stream, "negativ SMPTE format %d\n", ((division & 0x7F00) >> 8));
      fprintf (err_stream, "ticks per frame %d\n", (division & 0xFF));
      exit (1);
   }
   else
   {
      TicksPerQuaterNote = division & 0x7FFF;

#ifdef RHC_DEBUG
      fprintf (err_stream, "ticks per quater-note %d\n", (division & 0x7FFF));
#endif /* RHC_DEBUG  */
   }

   /* flush any extra stuff, in case the length of header is not 6 */
   length -= 6;

   while (length-- != 0L) my_getc (in_stream);

   return (ntrks);
}
/*---------------------------------------------------------------------
    Display chunk infos
-----------------------------------------------------------------------
*/
int
read_chunk_info (FILE * stream)
{
   char   *str;
   long    length, i;

   /* read chunk type */
   str = read_n_byte (stream, 4);
#ifdef RTC_DEBUG
   fprintf (err_stream, "chunk type : %s\n", str);
#endif /* RTC_DEBUG */
   fprintf (err_stream, "read_chunk_info() chunk type : %s\n", str);
   free (str);

   if (feof (stream))
   {
      fprintf (err_stream, "read_chunk_info() end of file\n");
   }

   /* read length */
   length = read32bit (stream);

#ifdef RTC_DEBUG
   fprintf (err_stream, "length : %8ld\n", length);
#endif /* RTC_DEBUG */
   for (i = 0; i < length; i++)
   {
      my_getc (stream);
   }
   return (0);
}

/*---------------------------------------------------------------------
    read midi track chunk
-----------------------------------------------------------------------
*/
int
read_track_chunk (FILE * stream)
{
   char   *str;
   long    length;

   /* read chunk type */
   str = read_n_byte (stream, 4);
#ifdef RTC_DEBUG
   fprintf (err_stream, "chunk type : %s\n", str);
#endif /* RTC_DEBUG */
   if (strncmp (str, "MTrk", 4))
   {
      fprintf (err_stream, "read header chunk invalid chunk type : %s\n", str);
   }
   free (str);
   if (feof (stream))
   {
      fprintf (err_stream, "read_track_chunk() end of file\n");
   }

   /* read length */
   length = read32bit (stream);

   if (feof (stream))
   {
      fprintf (err_stream, "read_track_chunk() end of file\n");
   }
#ifdef RTC_DEBUG
   fprintf (err_stream, " length : %8ld", length);
#endif /* RTC_DEBUG */
   GlobalTime = 0L;
   rdl_output (0L, INIT_RDL_OUTPUT, 0, 0);
   rdl_output (GlobalTime, RDL_FLUSH, 0, 0);

   while (read_mtrk_event (stream) != ME_END_OF_TRACK);

   return (0);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int
read_mtrk_event (FILE * stream)
{
   long    delta_time;
   int     ch, mode=NONE, channel;
   uchar   byte, running_status;
   int     note, velocity;

   if (feof (stream))
   {
      /* flush output buffer */
      rdl_output (GlobalTime, RDL_FLUSH, 0, 0);
      return (ME_END_OF_TRACK);
   }

   delta_time = read_var_len (stream);
   GlobalTime += delta_time;

#ifdef TIME_DEBUG
   fprintf (err_stream, "delta time   : %ld\n", delta_time);
#endif /* TIME_DEBUG */
   ch = getc (stream);

   if (feof (stream))
   {
      /* flush output buffer */
      rdl_output (GlobalTime, RDL_FLUSH, 0, 0);
      return (ME_END_OF_TRACK);
   }
   else
   {
      byte = (uchar) ch;

#ifdef RTE_INPUT_DEBUG
      fprintf (err_stream, " %2X %3d %c\n", (int) ch, (int) ch, ch);
#endif /* RTE_INPUT_DEBUG */
      if (!(byte & MIDI_EVENT)) /* midi event */
      {
         switch (mode)
         {
         case NOTE_ON:
         case NOTE_OFF:
            ungetc (byte, stream);
            byte = running_status;
            break;
         default:
            fprintf (err_stream, "%2x, %3d <-- no MIDI EVENT\n", byte, byte);
            break;
         }
      }

      {
         mode = NONE;
         running_status = byte;

         switch (byte & 0xF0)   /* midi channel events */
         {

         case NOTE_ON:
            {
               if (GlobalChannelFlag)
                  channel = GlobalChannel;
               else
                  channel = byte & CHANNEL;

               mode = NOTE_ON;

               note = my_getc (stream);
               velocity = my_getc (stream);

#ifdef NOTE_DEBUG
               fprintf (err_stream, "note on %d ", channel);
               fprintf (err_stream, "%2x ", note);
               fprintf (err_stream, "%2x \n", velocity);
#endif /* NOTE_DEBUG */
               if (velocity == 0)
                  rdl_output (GlobalTime, NOTE_OFF, channel, ++note);
               else
                  rdl_output (GlobalTime, mode, channel, ++note);

            }
            break;

         case NOTE_OFF:
            {
               if (GlobalChannelFlag)
                  channel = GlobalChannel;
               else
                  channel = byte & CHANNEL;

               mode = NOTE_OFF;
               note = my_getc (stream);
               velocity = my_getc (stream);

#ifdef NOTE_DEBUG
               fprintf (err_stream, "%2x ", note);
               fprintf (err_stream, "%2x ", velocity);
               fprintf (err_stream, "note off %d\n", channel);
#endif /* NOTE_DEBUG */
               rdl_output (GlobalTime, mode, channel, ++note);
            }
            break;

         case ALL_NOTES_OFF:
            {
               if (GlobalChannelFlag)
                  channel = GlobalChannel;
               else
                  channel = byte & CHANNEL;

               mode = NONE;
#ifdef NOTE_DEBUG
               fprintf (err_stream, "all notes off %d \n", channel);
#endif /* NOTE_DEBUG */
               rdl_output (GlobalTime, mode, channel, 0);
            }
            break;

         default:
            break;
         }

         switch (byte)          /* midi events */
         {

         case META_EVENT:
            {
               long    len;
               uchar   meta_event;

               meta_event = my_getc (stream);

#ifdef RTE_DEBUG
               fprintf (err_stream, "meta event %x\n", meta_event);
#endif /* RTE_DEBUG */
               len = read_var_len (stream);
               if (feof (stream))
               {
                  /* flush output buffer */
                  rdl_output (GlobalTime, RDL_FLUSH, 0, 0);
                  return (ME_END_OF_TRACK);
               }

               switch (meta_event)
               {
               case ME_SEQUENCE_NUMBER:
                  {
                     fprintf (out_stream, "* ME_SEQUENCE_NUMBER");
                  }
                  break;

               case ME_TEXT_EVENT:
                  {
                     fprintf (out_stream, "* ME_TEXT_EVENT");
                  }
                  break;

               case ME_COPYRIGTH:
                  {
                     fprintf (out_stream, "* ME_COPYRIGTH");
                  }
                  break;

               case ME_TRACK_NAME:
                  {
                     fprintf (out_stream, "* ME_TRACK_NAME");
                  }
                  break;

               case ME_INSTRUMENT_NAME:
                  {
                     fprintf (out_stream, "* ME_INSTRUMENT_NAME");
                  }
                  break;

               case ME_LYRIC:
                  {
                     fprintf (out_stream, "* ME_LYRIC");
                  }
                  break;

               case ME_MARKER:
                  {
                     fprintf (out_stream, "* ME_MARKER");
                  }
                  break;

               case ME_CUE_POINT:
                  {
                     fprintf (out_stream, "* ME_CUE_POINT");
                  }
                  break;

               case ME_CHANNEL_PREFIX:
                  {
                     fprintf (out_stream, "* ME_CHANNEL_PREFIX");

                     GlobalChannel = my_getc (stream);
                     //fprintf (stdout, "GLOBAL CHANNEL NUMBER %d", GlobalChannel);
                     len--;
                  }
                  break;

               case ME_END_OF_TRACK:
                  {
                     /* flush output buffer */
                     rdl_output (GlobalTime, RDL_FLUSH, 0, 0);
                     fprintf (out_stream, "* ME_END_OF_TRACK\n");

                     return (ME_END_OF_TRACK);
                  }
                  break;

               case ME_SET_TEMPO:
                  {
                     long    tempo;
                     char   *str;

                     fprintf (out_stream, "* ME_SET_TEMPO");

                     str = read_n_byte (stream, len);
                     len = 0;
                     tempo = to32bit (0, (int) str[0], (int) str[1], (int) str[2]);
                     if (str != NULL)
                     {
                        fprintf (out_stream, " microseconds-per-MIDI-quarter-note=%d\n", tempo);
                        free (str);
                     }
                  }
                  break;

               case ME_SMPTE_OFFSET:
                  {
                     char   *str;

                     fprintf (out_stream, "* ME_SMPTE_OFFSET");

                     str = read_n_byte (stream, len);
                     len = 0;
                     if (str != NULL)
                     {
                        fprintf (out_stream, " hour=%d minute=%d second=%d frame=%d fract-frame=%d\n",
                                 (int) str[0], (int) str[1], (int) str[2], (int) str[3]);
                        free (str);
                     }
                  }
                  break;

               case ME_TIME_SIGNATURE:
                  {
                     int     denom = 1;
                     char   *str;

                     fprintf (out_stream, "* ME_TIME_SIGNATURE");
                     str = read_n_byte (stream, len);
                     while (str[1]-- > 0)
                        denom *= 2;
                     len = 0;
                     if (str != NULL)
                     {
                        fprintf (out_stream, " %d/%d  MIDI-clocks/click=%d  32nd-notes/24-MIDI-clocks=%d\n",
                           (int) str[0], denom, (int) str[2], (int) str[3]);
                        free (str);
                     }
                  }
                  break;

               case ME_KEY_SIGNATUR:
                  {
                     fprintf (out_stream, "* ME_KEY_SIGNATUR");
                  }
                  break;

               case ME_SEQUENCER_SPECIFIC:
                  {
                     fprintf (out_stream, "* ME_SEQUENCER_SPECIFIC");
                  }
                  break;

               default:

                  fprintf (err_stream, "ERROR invalid metaevent\n");
                  break;
               }                /* end of meta event switch */

               if (len > 0)
               {
                  char   *str;

                  str = read_n_byte (stream, len);
                  if (str != NULL)
                  {
                     fprintf (out_stream, " %s", str);
                  }
                  fputc ('\n', out_stream);
                  free (str);
               }
            }
            break;

         case SYS_EXCL_F0:      /* system exclusiv massage */
         case SYS_EXCL_F7:      /* system exclusiv massage */
            {
               long    len;
               int     i;

#ifdef RTE_DEBUG
               fprintf (err_stream, "sys excl\n");
#endif /* RTE_DEBUG */
               len = read_var_len (stream);
               if (feof (stream))
               {
                  /* flush output buffer */
                  rdl_output (GlobalTime, RDL_FLUSH, 0, 0);
                  return (ME_END_OF_TRACK);
               }
#ifdef RTE_DEBUG
               fprintf (err_stream, "length  : %ld\n", len);
#endif /* RTE_DEBUG */
               for (i = 0; i < len; i++)
               {
#ifdef RTE_DEBUG
                  fprintf (err_stream, " %3d %x\n", i, my_getc (stream));
#endif /* RTE_DEBUG */
               }

               if (byte == SYS_EXCL_F0)
               {
#ifdef RTE_DEBUG
                  fprintf (err_stream, " end %x\n", i, my_getc (stream));
#endif /* RTE_DEBUG */
               }
            }
            break;

         default:
            break;
         }
      }

   }
   return (0);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/


int
input_read_error ()
{

   fprintf (err_stream, "read error on input stream\n");
   return (0);
}


/*---------------------------------------------------------------------
    write_output_line           
    clock     1C  2C  3C  ... 16C
    xxxxxxxx #nnn#nnn#nnn ... #nnn
    10 + 16*4 = 74 byte / line
-----------------------------------------------------------------------
*/

int
write_output_line (FILE * stream, long midi_clock, int *channel)
{
   int     i;
#ifdef DEBUG_NOTE_CODE
   int     nc;
#endif
#ifdef TEMP_BAR_INFO
   if (BarFlag)
   {
      if (NextBarTime <= midi_clock)
      {
         do
         {
            NextBarTime += BarTime;
            BarCount++;
            if (BarCount < FromBar || BarCount > ToBar)
            {
               return (0);
            }
            else
            {
               fprintf (stream, "# bar %d\n", BarCount);
            }
         }
         while (NextBarTime <= midi_clock);
      }
      if (BarCount < FromBar || BarCount > ToBar)
      {
         return (0);
      }
   }
#endif /* TEMP_BAR_INFO */
#ifdef DEBUG
   fprintf (err_stream, "\n");
#endif /*  */
#ifdef DEBUG_NOTE_CODE
   if (note_code_flag)
   {
      fprintf (stream, "***       ");

      for (i = 0; i < 16; i++)
      {
         if (channel[i] == 0)
         {
            fprintf (stream, "%s", " .. ");
         }
         else
         {
            nc = channel[i];
            if (nc < 0)
               nc = -nc;
            nc--;
            nc = nc % 12;
            fputs (note_code[nc], stream);
         }
      }
      fprintf (stream, "\n");
   }

#endif /*  */
   fprintf (stream, "%9ld ", midi_clock);

   for (i = 0; i < 16; i++)
   {
      if (channel[i] == 0)
      {
         fprintf (stream, "%s", " .. ");
      }
      else
      {
         fprintf (stream, "%+4d", channel[i]);
      }
   }
   fprintf (stream, "\n");

   return (0);
}

/*-------------------------------------------------------------------------
     read line form temp file
     return : timeinfo
              timeinfo == -1 : end of file
---------------------------------------------------------------------------
*/
/* #define DEBUG_READ_LINE
*/

long read_line (FILE * tmp_stream, int channel_nodes[16])
{
   long    time;
   int     i = 0, ch, ret;
   char    buffer[100];

   while ((ch = fgetc (tmp_stream)) == '*')
   {
      //putc (ch, out_stream);
      while ((ch = my_getc (tmp_stream)) != '\n')       /* commentar */
      {
         //putc (ch, out_stream);
      }
      //putc ('\n', out_stream);
   }
   ungetc (ch, tmp_stream);

   if ((ret = fscanf (tmp_stream, "%9ld", &time)) != 1)
   {
      if (ret == EOF)
      {
         return (-1L);
      }
      else
      {
         fprintf (err_stream, "tmp file read error\n");
         return (-1L);
      }
   }
   fgetc (tmp_stream);          /* read blank */

#ifdef DEBUG_READ_LINE
   fprintf (err_stream, "\n");
#endif /*  */
   for (i = 0; i < 16; i++)
   {
      if (fread (buffer, 1, 4, tmp_stream) != 4)
      {
         fprintf (err_stream, "tmp file read error\n");
      }
      buffer[4] = 0;            /* end of string */

#ifdef DEBUG_READ_LINE
      fprintf (err_stream, "%s-", buffer);
#endif /*  */
      if (strcmp (buffer, " .. ") == 0)
      {
         channel_nodes[i] = 0;
      }
      else
      {
         sscanf (buffer, "%4d", &channel_nodes[i]);
      }
   }
#ifdef DEBUG_READ_LINE
   fprintf (err_stream, "\n", buffer);
#endif /*  */
   /* goto end of file  */
   while ((ch = my_getc (tmp_stream)) != '\n');

   return (time);
}


/*---------------------------------------------------------------------
   Vers.: 1.2 
   
   if ther is no BarInfoFile   stream = NULL;              
-----------------------------------------------------------------------
*/                                 
BarInfo::BarInfo()
{
   InfoStream = NULL;
   CallCounter =0;
   Time = DeltaTime = TicksPerNote = 0L;
   InputLineCount =  Z = N = 0;
   Bar = -1;
}           
void BarInfo::SetInfoStream(FILE *inFile)
{
   InfoStream = inFile;
   InputLineCount = 1L;
   if( NextLine() == 0 )
   {  
      if( Bar == 0 ) /* offbeat value */
      {
         SetOffBeat( Z,N );
         if( NextLine() == 0 )
         {  
            if( Bar == 1 ) 
               SetSpeed( Z,N );  /* set speed */
            else
               return; 
         }
      }
      else if( Bar == 1 ) /* Use -b= param for offbeet and set speed value */
      {
         SetSpeed( Z,N );
         NextLine(); /* init next data */
         return;
      }   
   }                                                        
   else
   {
      fprintf( err_stream,"Error in Barfile line %d\n", InputLineCount );
   }                               
}

void BarInfo::SetOffBeat( int OffBeatZ, int OffBeatN )
{
    if (OffBeatN == 0)
       Time = 0L;                             
    else
       Time = (long) TicksPerNote *(long)OffBeatZ / (long) OffBeatN;

}
void BarInfo::SetSpeed( int BarTimeZ, int BarTimeN )
{    
     if( BarTimeN == 0 )
        DeltaTime = TicksPerNote;
     else
        DeltaTime = TicksPerNote * (long) BarTimeZ / (long) BarTimeN;
}
long BarInfo::NextBarTime()
{           
      if( TicksPerNote == 0L )
      {
         return MAX_LONG_VAL;  /* can't calculate Time */
      }
      if( CallCounter == 0 ) /* first call */
      {                   
         CallCounter++;                         
         return Time;
      }
      if( InfoStream == NULL )
      {
         Time += DeltaTime;  /* constant beet info from program arguments */
      }
      else
      {  
         if( CallCounter >= Bar )
         {
            /* get new beet information */
            SetSpeed( Z,N );
            NextLine();
         }
         Time += DeltaTime;
      }    
      CallCounter++;                         
      return Time;    
}

int BarInfo::NextLine()
{ 
int     ch;
int     Bar,Z,N;

           if( (ch=fgetc( InfoStream )) == EOF )
           {
              /* end of BarInfofile or Error */ 
              this->Bar = MAX_BAR_NUMBER;
              return -1;
           } 
           
           while( ch == '*') /* this is a comment */
           {
               while( (ch=fgetc( InfoStream )) != EOF && ch != '\n' );
               if( (ch=fgetc( InfoStream )) == EOF )
               {
                  /* end of BarInfofile or Error */ 
                  this->Bar = MAX_BAR_NUMBER;
                  return -1;
               } 
               InputLineCount++;
           }  
           ungetc( ch, InfoStream );
           
           /* read next line */
               if (fscanf (InfoStream, "%d %d/%d", &Bar,&Z, &N) != 3)
               {  
                  if( !feof( InfoStream ) )
                         fprintf( err_stream, "Error in BarInfoFile Line %d\n", InputLineCount);
               }
               else /* Format correct ? */
           {  
              if( Bar <= this->Bar || N == 0 || Z ==0 )
              {
                 fprintf( err_stream, "Error in BarInfoFile Line%5d-->%d %d/%d<--\n",InputLineCount,Bar,Z,N );
                 /* fprintf( err_stream, "Bar %d Counter %d\n",this->Bar, CallCounter );
                 */
                 this->Bar = MAX_BAR_NUMBER;
              }
              else
              {
                 this->Bar = Bar;
                 this->Z = Z;
                 this->N = N;
              }   
           } 
           /* goto end of line */
           while( (ch=fgetc( InfoStream )) != EOF && ch != '\n' );
           InputLineCount++;
           return 0;
}
void BarInfo::Debug( FILE *OutSteam )
{
    fprintf( OutSteam,"Next(Bar %d Z %d N %d) Time %ld DeltaTime %ld %5.3f\n",Bar,Z,N,Time,DeltaTime ,(float)DeltaTime/(float)TicksPerNote );
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int
main (int argc, char *argv[])
{
   int     i, byte_count = 0, next;
   int     ntrks;
   int     TmpFileFlag[16];     /* flag for used temp channel files */
   FILE    *TmpFile[16];        /* streams for temp files */
   FILE    *OutputFile = NULL; 
   FILE    *BarInfoFile = NULL;
   long    TimeInfo[16], MinTime, LastTime;
   int     NodeInfo[16][16];
   long    LineReadCounter;

   int     BarTimeZ = 4, BarTimeN = 4;
   int     OffBeatN = 4;                /* Nenner off beat time */
   int     OffBeatZ = 0;       /* Zaehler  */
   int     DebugFlag = 0; 
   
#ifdef DEBUG
   long    input_byte_count = 0;
#endif /*  */

Rdl     rdl;
BarInfo bar;


   /*----------------------------------------------------------
     programm flags setting und validation
     ------------------------------------------------------------
   */
   
   for (i = 0; i < 16; i++)
   {
      TmpFileFlag[i] = 0;       /* tracks not used */
	  TmpFile[i]	 = NULL; 
   }

   /*----------------------------------------------------------
     programm flags setting und validation
     ------------------------------------------------------------
                             */
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
         switch (argv[i][1])
         {
         case '?':
         case 'h':
         case 'H':
            mid_to_rdl_usage ();
            return (0);
            break;
         case 'v':
            rameau_version (err_stream);
            return (0);
            break;
         case 'e':
            {
               if ((err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
               {
                     err_stream = stderr;
                     fprintf (err_stream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
               }
            }
            break;
         case 'i':
            in_stream = stdin;
            break;
         case 'o':
            OutputFile = stdout;
            break;

         case 'l':
            ReadLineFlag = 1;
            break;

         case 'n':
            note_code_flag = 1;
            break;

         case 'b':              /* off beat time signature */
            {
               if (argv[i][2] != '=')
               {
                      mid_to_rdl_usage ();
                      return (1);
               }
               if (sscanf (argv[i] + 3, "%d/%d", &OffBeatZ, &OffBeatN) != 2)
               {
                      mid_to_rdl_usage ();
                      return (1);
               }
            }
            break;

         case 's':              /* time signature */
            {
               if (argv[i][2] != '=')
               {
                      mid_to_rdl_usage ();
                      return (1);
               }
               if (sscanf (argv[i] + 3, "%d/%d", &BarTimeZ, &BarTimeN) != 2)
               {
                      mid_to_rdl_usage ();
                      return (1);
               }

            }
            break;

         case 'f':              /* notes from */

            if (argv[i][2] != '=')
            {
               mid_to_rdl_usage ();
               return (1);
            }
            if (sscanf (argv[i] + 3, "%d", &FromBar) != 1)
            {
               mid_to_rdl_usage ();
               return (1);
            }
            break;

         case 't':              /* notes to */

            if (argv[i][2] != '=')
            {
               mid_to_rdl_usage ();
               return (1);
            }
            if (sscanf (argv[i] + 3, "%d", &ToBar) != 1)
            {
               mid_to_rdl_usage ();
               return (1);
            }
            break;

         case 'D':
            DebugFlag = 1;
            break;
         default:
            mid_to_rdl_usage ();
            return (0);
            break;
         }
      }
      else
         /* input/output file */
      {
          char    buf[_MAX_PATH];

              /* first parameter is input file */
             if (in_stream == NULL)
             {
                 strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
                 strcat (buf, ".");
                 strcat (buf, INP_EXT);
#endif /*  */
                 in_stream = fopen (buf, "rb");

                 if (in_stream == NULL)
                 {
                     fprintf (err_stream, "!!! File %s open error !!!\n", buf);
                     return (1);
                 }

                 if (OutputFile == NULL)
                 {
                    strcpy (buf, argv[i]);
#ifdef AUTO_EXTENION
                    strcat (buf, ".");
                    strcat (buf, OUT_EXT);
#endif /*  */                     
                    remove( buf );
                    OutputFile = fopen (buf, "w");

                    if (OutputFile == NULL)
                    {
                            fprintf (err_stream, "!!! File %s open error !!!\n", buf);
                            return (2);
                    }
                    SetFileInfoLine( OutputFile, buf );
                    strcpy (buf, argv[i]);
                    #ifdef AUTO_EXTENION
                    strcat (buf, ".");
                    strcat (buf, INP_EXT);
                    #endif /*  */
                    SetFileInfoLine( OutputFile, buf );
                 }
             }
             else
             {
                mid_to_rdl_usage ();
                return (3);
             }
             
             /* Vers.: 1.2 if there is a .BAR file open this file */
             
             strcpy (buf, argv[i]);
#ifdef AUTO_EXTENION
             strcat (buf, ".");
             strcat (buf, BAR_EXT);
#endif /*  */
             BarInfoFile = fopen (buf, "r");
      }
   }

   /*-----------------------------------------------
     if no in/out file specified use stdin/stdout
     -------------------------------------------------
                             */
   if (in_stream == NULL)
   {
      in_stream = stdin;
   }

   if (OutputFile == NULL)
   {
      OutputFile = stdout;
   }

   rdl.SetErrStream( err_stream );

   /*----------------------------------------------------------
     read midi header
     ------------------------------------------------------------
   */
   ntrks = read_header_chunk (in_stream);

   /*  display chunk information
     while( read_chunk_info( in_stream ) == 0 );
     exit( 0 );
   */
   /*----------------------------------------------------------
     main loop for tempfiles
     ------------------------------------------------------------
   */
   i = 0;
   LineReadCounter = 0L;
   do
   {
      if( ReadLineFlag )  // read track( number )
      {
          fprintf( stdout, "\rTrack %2ld", LineReadCounter++ );
      }
	  
	  TmpFile[i] = out_stream = tmpfile();	

      if (out_stream == NULL)
      {
             fprintf (err_stream, "Could not open new temporary file\n" );
             return (2);
      }
      TmpFileFlag[i] = 1;
	  i++; 
      fprintf (out_stream, "* TRACK %d\n", i);

      if (feof (in_stream))
      {
         fprintf (err_stream, "main() end of input file\n");
         break;
      }
   }
   while (ntrks-- > 0 && read_track_chunk (in_stream) == 0);

   /*----------------------------------------------------------
     main loop for tempfiles end
     ------------------------------------------------------------
   */

   if (in_stream != stdin)
      fclose (in_stream);

   out_stream = OutputFile;

   rdl.SetOutStream( out_stream );

   /*----------------------------------------------------------
   main loop for output file ( build RDL-Files )
   ------------------------------------------------------------
   */                                          
   
   BarFlag = 1;
   bar.SetTicks( TicksPerQuaterNote );
   bar.SetOffBeat( OffBeatZ,OffBeatN); 
   bar.SetSpeed( BarTimeZ,BarTimeN);
   if( BarInfoFile != NULL )
   {
       fprintf (err_stream, "   Use BarInfoFile\n");
       bar.SetInfoStream( BarInfoFile);
   }

   /* set temp-files position */

   for (i = 0; i < 16; i++)
   {  
   
      if (TmpFileFlag[i])
      {
			 // Set tmp-file position

			 fseek( TmpFile[i], 0L, SEEK_SET);
			 TimeInfo[i] = read_line (TmpFile[i], NodeInfo[i]);
      }
   }

   /* open output file */
   rdl_output (0L, INIT_RDL_OUTPUT, 0, 0);

   LastTime = 0;
   LineReadCounter = 0L;
   
   /* Vers.: 1.2 init NextBarTime */  
   if (BarFlag) NextBarTime = bar.NextBarTime();
   /* printf( "\nNextBarTime %6ld", NextBarTime );
   */
   while (1 /* getch() */ )
   {
      if( ReadLineFlag )
      {
              fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }

      MinTime = MAX_LONG_VAL;
      for (i = 0; i < 16; i++)
      {
            if (TmpFileFlag[i])
            {
               if (MinTime > TimeInfo[i])
               {
                   MinTime = TimeInfo[i];
                   next = i;
               }
             }
      }

      //fprintf (stdout, "%ld\n", MinTime);

      if (MinTime > LastTime)
      {
             LastTime = MinTime;
      }
        
        
   if (BarFlag)
   {                   
      if (NextBarTime <= TimeInfo[next] )
      {
            do
            {
           /* Vers.: 1.2 */
           /* get next bar time */
           
           NextBarTime = bar.NextBarTime();
           #ifdef DEBUG_NEXT_BAR_TIME
                  printf( "\nNextBarTime %6ld", NextBarTime );
               #endif
               if ( BarCount > ToBar)
               {
                  goto GOTO_END;
               }
               else
               if( BarCount < FromBar )
               {
              // dont write to rdl file
               }
               else
               {
                  // flush output
                  rdl.WriteBuf ( -1L, NodeInfo[next] );
                  // bar.Debug( out_stream );
                  fprintf ( out_stream, "# bar %d\n", bar.GetBarNum());
               }
            }
            while (NextBarTime <= TimeInfo[next]);
      }
      BarCount = bar.GetBarNum();
      if (BarCount < FromBar || BarCount > ToBar)
      {
             goto GOTO_END;
      }
   }
   rdl.WriteBuf ( TimeInfo[next], NodeInfo[next] );

GOTO_END:

      if ((TimeInfo[next] = read_line (TmpFile[next], NodeInfo[next])) == -1)
      {	
         /* end of file */
         TmpFileFlag[next] = 0;

         /* all files are closed ? */
         for (i = 0; i < 16; i++)
         {
            if (TmpFileFlag[i] != 0)
               break;
         }
         if (i == 16)
         {
            /* flush output */
            rdl.WriteBuf ( -1L, NodeInfo[next] );
            break;
         }
      }
   }
   /*----------------------------------------------------------
   main loop for output file end
   ------------------------------------------------------------*/
   
   if (out_stream != stdout)
      fclose (out_stream);
   if (err_stream != err_stream)
      fclose (err_stream);

   return (0);
}

/*-----------------------------------------------------------------------
-------------------------------------------------------------------------
*/

int rdl_output (long midi_clock, int mode, int channel, int note)
{
   int static init = 0;
   long static last_midi_clock;

   if (midi_clock < 0)
      return (0);

   if (channel > 15)
   {
      fprintf (err_stream, "rdl_output() invalid channel number\n");
   }

   if (mode == INIT_RDL_OUTPUT)
   {
      memset (channel_buf, 0, sizeof (channel_buf));
      last_midi_clock = midi_clock;
   }

   if (last_midi_clock < midi_clock)
   {
      /* write infos to file */
      write_output_line (out_stream, last_midi_clock, channel_buf);
      NodeFlag = 0;
      memset (channel_buf, 0, sizeof (channel_buf));

      last_midi_clock = midi_clock;
   }

   switch (mode)
   {
   case NOTE_ON:
      {
         mode = NOTE_ON;

#ifdef DEBUG
         fprintf (err_stream, "note on %d\n", channel);
#endif /*  */
      }
      break;

   case NOTE_OFF:
      {
         mode = NOTE_OFF;

#ifdef DEBUG
         fprintf (err_stream, "note off %d \n", channel);
#endif /*  */
      }
      break;

   case ALL_NOTES_OFF:
      {
         if (NodeFlag & (0x01 << channel))
         {
            /* note flage is true */
            write_output_line (out_stream, midi_clock, channel_buf);
            NodeFlag = 0;
            memset (channel_buf, 0, sizeof (channel_buf));
         }

         NodeFlag |= (0x01 << channel); /* set note flag on channel */

         channel_buf[channel] = -999;
         mode = NONE;
#ifdef DEBUG
         fprintf (err_stream, "all notes off %d\n", channel);
#endif /*  */
      }
      break;

   default:
      break;
   }

#ifdef DEBUG
   printf (" note %d", note);
#endif /*  */
   switch (mode)
   {
   case NOTE_ON:
      {
         if (NodeFlag & (0x01 << channel))
         {
            /* note flage is true */
            write_output_line (out_stream, midi_clock, channel_buf);
            NodeFlag = 0;
            memset (channel_buf, 0, sizeof (channel_buf));
         }

         NodeFlag |= (0x01 << channel); /* set note flag on channel */

         channel_buf[channel] = note;
      }
      break;

   case NOTE_OFF:
      {
         if (NodeFlag & (0x01 << channel))
         {
            /* note flage is true */
            write_output_line (out_stream, midi_clock, channel_buf);
            NodeFlag = 0;
            memset (channel_buf, 0, sizeof (channel_buf));
         }

         NodeFlag |= (0x01 << channel); /* set note flag on channel */

         channel_buf[channel] = -note;
      }
      break;

   case RDL_FLUSH:
      {
         write_output_line (out_stream, midi_clock, channel_buf);
         NodeFlag = 0;
         memset (channel_buf, 0, sizeof (channel_buf));
      }
      break;
   }
   return (0);
}

/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
