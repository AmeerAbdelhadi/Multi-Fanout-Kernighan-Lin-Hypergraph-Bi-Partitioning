/***********************************************************************************
** Copyright (c) 2011, University of British Columbia (UBC); All rights reserved. **
**                                                                                **
** Redistribution  and  use  in  source   and  binary  forms,   with  or  without **
** modification,  are permitted  provided that  the following conditions are met: **
**   * Redistributions   of  source   code  must  retain   the   above  copyright **
**     notice,  this   list   of   conditions   and   the  following  disclaimer. **
**   * Redistributions  in  binary  form  must  reproduce  the  above   copyright **
**     notice, this  list  of  conditions  and the  following  disclaimer in  the **
**     documentation and/or  other  materials  provided  with  the  distribution. **
**   * Neither the name of the University of British Columbia (UBC) nor the names **
**     of   its   contributors  may  be  used  to  endorse  or   promote products **
**     derived from  this  software without  specific  prior  written permission. **
**                                                                                **
** THIS  SOFTWARE IS  PROVIDED  BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" **
** AND  ANY EXPRESS  OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE **
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE **
** DISCLAIMED.  IN NO  EVENT SHALL University of British Columbia (UBC) BE LIABLE **
** FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL **
** DAMAGES  (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR **
** SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER **
** CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN CONTRACT, STRICT LIABILITY, **
** OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE **
** OF  THIS SOFTWARE,  EVEN  IF  ADVISED  OF  THE  POSSIBILITY  OF  SUCH  DAMAGE. **
***********************************************************************************/

/***********************************************************************************
** partitioning.c: multi-fanout Kernighan-Lin hypergraphs bi-partitioning - Main  **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include "geometry.h"
#include "graphics.h"
#include "allocation.h"
#include "floorplan.h"
#include "uarray.h"
#include "farray.h"
#include "text.h"

/* global variables declaration                                   */
floorplan *gfp          ; /* global floorplan database            */
char       gFooterLabel[1024]  =""; /* global footer text label   */
char       gHeaderLabel[1024]  =""; /* global header text label   */
char       gFooterMessage[1024]=""; /* global footer text message */
float      gWorldStep             ; /* grid step                  */
float      gWorldX = 1000         ; /* world X dimension          */
float      gWorldY = 1000         ; /* world X dimension          */

/* commandline arguments parsing, sets global variables                   */
/* returns filename index in argv                                         */
unsigned int commandlineParse(int argc, char *argv[]                      );
/* results used to set the following global variables                     */
int          gVerbose        = 0       ; /* enable verbose logging        */
int          gGUI            = 0       ; /* enable GUI mode               */
int          gPostScript     = 0       ; /* enable postscript plotting    */
char         gRunMode        = 's'     ; /* run mode, default is step     */
unsigned int gIterations     = UINT_MAX; /* iteration/passes              */
unsigned int gMaxIneffective = 3       ; /* max ineffective passes        */
unsigned int gGainFunc       = 2       ; /* gain function, def: enhanced2 */
float        gAlpha          = 2     ; /* parameter for gain function 2 */

/* functions associated with buttons                                                 */
inline void enablePS (void (*drawScreen_ptr)(void)) {gPostScript=1;} /* enable  PS   */
inline void disablePS(void (*drawScreen_ptr)(void)) {gPostScript=0;} /* disable PS   */
inline void runStep  (void (*drawScreen_ptr)(void)) {gRunMode='s' ;} /* run one step */
inline void runPass  (void (*drawScreen_ptr)(void)) {gRunMode='p' ;} /* run one pass */
inline void runAll   (void (*drawScreen_ptr)(void)) {gRunMode='a' ;} /* run all      */

/* redrawing routine for still pictures. Redraw if user changes the window           */
void drawScreen() { clearscreen(); fpDraw(gfp,gWorldX,gWorldY); } /* clear & redraw  */

/* called whenever event_loop gets a button press in the graphics area.              */
void buttonPress  (float x, float y, int flags) {                                     }

/* receives the current mouse position in the current world as in init_world         */
void mouseMove    (float x, float y) {                                                }

/* function to handle keyboard press event, the ASCII character is returned          */
void keyPress     (int i) {                                                           }

/* show global message, wait until 'Proceed' pressed then draw screen                */
void waitLoop () {
  update_message(gFooterMessage);
  drawScreen();
  event_loop(buttonPress,mouseMove,keyPress,drawScreen);
}


/*************************************  main  ****************************************/

int main(int argc, char *argv[]) {

  /* randomize seed  */
  srandom(time(NULL));

  /* parse commandline returns filename index in argv */
  /* create floorplan data structure                  */
  gfp=fpCreate(  argv[ commandlineParse(argc,argv) ]  );

  if (gPostScript) gGUI      = 1 ;  /* enable gui if post script required      */
  if (!gGUI      ) gVerbose  = 1 ;  /* enable verbose if if gui is not enabled */

  if (gGUI) {

    /* initialize display with WHITE 1000x1000 background */
    init_graphics((char*)"Enhanced Kernighan-Lin (K&L) based bi-partitioning algorithm for hypergraphs", WHITE, NULL);
    init_world (0.,0.,gWorldX,gWorldY);

    /* Create new buttons */
    create_button ((char*)"Window"    , (char*)"---1"      , NULL     ); /* Separator    */
    create_button ((char*)"---1"      , (char*)"Enable  PS", enablePS ); /* enable PS    */
    create_button ((char*)"Enable  PS", (char*)"Disable PS", disablePS); /* disable PS   */
    create_button ((char*)"Window"    , (char*)"---1"      , NULL     ); /* Separator    */
    create_button ((char*)"---1"      , (char*)"Run Step"  , runStep  ); /* run one step */
    create_button ((char*)"Run Step"  , (char*)"Run Pass"  , runPass  ); /* run one pass */
    create_button ((char*)"Run Pass"  , (char*)"Run All"   , runAll   ); /* run all      */
  }

  /* invoke partitioning with designated parameters */
    fpPartition(gfp,gIterations,gMaxIneffective);
  

  /* finished! wait still until 'Exit" is pressed */
  if (gGUI)
    while(1) waitLoop();

  /* free database */
  fpDelete(gfp);

  return 1;
}

/*************************************  main  ****************************************/


/* commandline arguments parsing        */
/* results used to set global variables */
/* returns filename index in argv       */
unsigned int commandlineParse(int argc, char *argv[]  ){

  /* arguments parsing                                              */
  int argi;                             /* arguments index          */
  int fileNameArgInd=-1;                /* file name argument index */
  for(argi=1;argi<argc;argi++) {        /* check all argument       */
    if (argv[argi][0]=='-') {           /* switch argument          */
      switch (tolower(argv[argi][1])) { /* consider first letter    */
       
        case 'h': /* help */
          prntRed; prntBld;
          printf("\nEnhanced Kernighan-Lin based bi-partitioning algorithm for hypergraphs\n"     );
          prntRst; prntBld;
          printf("Usage:\n"                                                                       );
          prntRst;
          printf("  partitioning INFILE [OPTIONS]\n"                                              );
          prntBld;
          printf("Options:\n"                                                                     );
          prntRst;
          printf("  -help       (or -h): Print this message\n"                                    );
          printf("  -gui        (or -g): Enable graphical user interface (GUI) \n"                );
          printf("                       Automatically enabled if postscript is enabled\n"        );
          printf("  -verbose    (or -v): Enable verbose logging,\n"                               );
          printf("                       Automatically enabled if GUI is disabled\n"              );
          printf("  -postscript (or -p): Generate PostScript every screen refresh\n"              );
          printf("                       Will enable GUI mode\n"                                  );
          printf("  -runmode    (or -r): Run mode, followed by one of the following\n"            );
          printf("                       'step'  (or 's'): display results every one step\n"      );
          printf("                       'pass'  (or 'p'): display results every full pass\n"     );
          printf("                       'final' (or 'f'): display print final result only\n"     );
          printf("                       Default mode is 'step'\n"                                );
          printf("  -iterations (or -i): Number of K&L algorithms iterations/passes\n"            );
          printf("                       * Followed by an integer, determines exact number of\n"  );
          printf("                         algorithm passes\n"                                    );
          printf("                       * Followed by an m<integer> (e.g. m3), determines the\n" );
          printf("                         number of max ineffective passes before termination\n" );
          printf("                       Default is m3 (run until 3 passes show no improvements\n");
          printf("  -function   (or -f): Gain function, one of: 0, 1, or 2, as follows:\n"        );
          printf("                       0: Original K&L gain func (reduction in crossing nets)\n");
          printf("                       1: Outbalanced K&L 1, equal gain cells are outbalanced\n");
          printf("                          by the ratio of cell amount in each group among all\n");
          printf("                          the cells which are connected to each specific cell\n");
          printf("                       2: Outbalanced K&L 2, equal gain cells are outbalanced\n");
          printf("                          by the ratio of cell amount in each group among all\n");
          printf("                          the cells which are connected to each specific cell\n");
          printf("                          ,calculated for each net individually, & normalized\n");
          printf("                       Default is 2 (Outbalanced K&L 2)\n"                      );
          printf("  -alpha      (or -a): a float positive parameter, used to increase the gain\n" );
          printf("                       of the small groups in order to converge fater\n"        );
          printf("                       Default is 1\n"                                          );
          prntBld;
          printf("Input file syntax:\n"                                                           );
          prntRst;
          printf("  <CELLS#> <NET#> <ROWS#> <COLUMNS#>\n"                                         );
          printf("  <#CELLS_CONNECTED_TO_NET_1> <LIST_OF_CELLS_CONNECTED_TO_NET_1>\n"             );
          printf("  <#CELLS_CONNECTED_TO_NET_2> <LIST_OF_CELLS_CONNECTED_TO_NET_2>\n"             );
          printf("  ...\n"                                                                        );
          printf("  <#CELLS_CONNECTED_TO_NET_n> <LIST_OF_CELLS_CONNECTED_TO_NET_n>\n"             );
          prntBld;
          printf("Examples:\n"                                                                    );
          prntRst;
          printf("  partitioning cps.txt (using default options)\n"                               );
          printf("  partitioning cps.txt -gui -iterations 10 (GUI enabled, run 10 passes)\n"      );
          printf("  partitioning cps.txt -gui -iterations m3 (stop after 3 ineffective passes)\n" );
          printf("  partitioning cps.txt -gui -iterations m3 -runmode pass (draw every pass))\n"  );
          printf("  partitioning cps.txt -gui -verbose -postscr -runmode pass -iter m4 -func 0\n" );
          printf("  partitioning cps.txt -g -v -p -r p -i m4 -f 0 (same as above)\n"              );
          prntRed;
          printf("Report bugs to <ameer.abdelhadi@gmail.com>\n\n"                                 );
          prntRst;
          exit(1);

        case 'v': /* verbose mode    */
          gVerbose=1; /* set verbose */
          break;

        case 'g': /* GUI mode */
          gGUI=1; /* set GUI  */
          break;

        case 'p':  /* PostScript mode */
          gPostScript=1;
          break;

        case 'r': /* run mode      */
          argi++; /* next argument */
          gRunMode = tolower(argv[argi][0]);
          if ((gRunMode != 's')&&(gRunMode != 'p')&&(gRunMode != 'f')) {
            printf("-E- Commandline error: -runmode should be followed by: 'step', 'pass', or 'final'! Exiting...\n");
            exit(-1);              
          }
          break;

        case 'i': /* iterations/passes */
          argi++; /* next argument     */
          if (tolower(argv[argi][0]) == 'm') { /* if max ineffective is set */
            if (sscanf(argv[argi]+1,"%u",&gMaxIneffective) == 0) { /* take next int */
              printf("-E- Commandline error: -iterations should be followed by an integer, or m<integer>! Exiting...\n");
              exit(-1);
            }
          } else if (sscanf(argv[argi],"%u",&gIterations) == 0) {
            printf("-E- Commandline error: -iterations should be followed by an integer, or m<integer>! Exiting...\n");
            exit(-1);
          }
          break;

        case 'f': /* gain function */
          argi++; /* next argument */
          if (sscanf(argv[argi],"%u",&gGainFunc) == 0) {
            printf("-E- Commandline error: -gainfunc should be followed by a positive integer! Exiting...\n");
            exit(-1);
          }
          break;

        case 'a': /* alpha */
          argi++; /* next argument */
          if (sscanf(argv[argi],"%f",&gAlpha) == 0) {
            printf("-E- Commandline error: -alpha should be followed by a positive float! Exiting...\n");
            exit(-1);
          }
          break;

        default : /* unknown argument */
          printf("-E- unknown argument %s\n",argv[argi]);
          exit(-1);

      } /* switch */
    } else fileNameArgInd = argi; /* file name argument index */
  } /* arguments loop (for) */

  /* check if infile is supplied */
  if (fileNameArgInd<0) {printf(" -E- infile should be supplied\n"); exit(-1);}

  /* return filename index in arguments array */
  return fileNameArgInd;
}
