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
** floorplan.h: floorplan, cell, and net module for the multi-fanout Kernighan-Lin**
**              bi-partitioning algorithm for hypergraphs                         **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#ifndef __FLOORPLAN_HEADER__
#define __FLOORPLAN_HEADER__

#include "geometry.h"

/* net data structure                                                                */
struct netStruct {
  unsigned int  cellsN; /* the amount of cells connected to this net                 */
  unsigned int *cells ; /* a listing of cells id's connected with this net           */
};
typedef struct netStruct net;

/* cell data structure                                                               */
struct cellStruct {
  unsigned int  netsN   ; /* the amount of nets connected to this cell (pins#)       */
  fPoint        location; /* cell location in floorplan                              */
  unsigned int *nets    ; /* a listing of the nets connected with this cell          */
};
typedef struct cellStruct cell;

/* floorplan data structure                                                          */
struct floorplanStruct {
  char          designName[32];
  unsigned int  nx            ; /* amount of rows in the floorplan                   */
  unsigned int  ny            ; /* amount of columns in the floorplan                */
  unsigned int  cellsN        ; /* amount of cells in the floorplan                  */
  unsigned int  netsN         ; /* amount of nets in the floorplan                   */
  cell         *cells         ; /* a listing of the cells int the floorplan          */
  net          *nets          ; /* a listing of the cells int the floorplan          */
  float        *gain          ; /* gains array, containts the gain of each cell      */
  unsigned int *group         ; /* attach each cell to a group (cells by index)      */
  unsigned int *lock          ; /* locks array, for every cell indicated if locked   */
  unsigned int groupCellsN[2] ; /* number of cells in each group (groups by index)   */
  unsigned int crossingNetsN  ; /* hold the number of crossing nets at this solution */
  short    int isWorst        ; /* indicates if current solution is worst that prev. */
};
typedef struct floorplanStruct floorplan;

/* external global variables                                                         */
extern              floorplan *gfp      ; /* global floorplan db    (partitioning.c) */
extern int          gVerbose            ; /* verbose status         (partitioning.c) */
extern int          gGUI                ; /* enable GUI mode        (partitioning.c) */
extern int          gPostScript         ; /* enable postscript      (partitioning.c) */
extern char         gRunMode            ; /* run mode               (partitioning.c) */
extern unsigned int gGainFunc           ; /* gain function          (partitioning.c) */
extern float        gAlpha              ; /* parameter for func. 2  (partitioning.c) */

extern char         gFooterMessage[1024]; /* global footer message  (partitioning.c) */
extern char         gHeaderLabel[1024]  ; /* global header label    (partitioning.c) */
extern char         gFooterLabel[1024]  ; /* global header label    (partitioning.c) */
extern void         drawScreen()        ; /* draw graphics routine  (partitioning.c) */
extern void         waitLoop()          ; /* show message,wait for 'Proceed' to draw */


/* returns a new allocated floorplan data structure, parsed from infile              */
floorplan    *fpCreate(const char *fileName                                          );

/* free a floorplan data structure                                                   */
void         fpDelete(floorplan  *fp                                                 );

/* dump a floorplan data structure to stdout - for verification purpose              */
void         fpDump(floorplan  *fp                                                   );

/* draw floorplan using EasyGl graphics module. World size is xWorld*yWorld          */
void         fpDraw    (floorplan *fp, float xWorld, float yWorld                    );

/* if net is not crossing (in same group), return group number, else return UINT_MAX */
unsigned int fpNetGroups(floorplan *fp, unsigned int netId);

/* determines if the net indexed netId is crossing net                               */
short int    fpIsNetCrossing(floorplan *fp, unsigned int netId                       );

/* update total crossing nets amount at the database                                 */
unsigned int fpUpdateCrossingNetsN(floorplan *fp                                     );

/* counts nets which connected to cellId and are crossing                            */
unsigned int fpCellCrossing(floorplan *fp, unsigned int cellId                       );

/* count number of cells connected with netId and are in group 'group'               */
unsigned int fpNetGroupCount(floorplan *fp, unsigned int netId, unsigned int group   );

/* calculate overbalance for equal gain cells                                        */
float        fpCellOverbalance(floorplan *fp, unsigned int cellId                    );

/* return and update gain for a cell in floorpaln DB                                 */
float        fpUpdateGain(floorplan *fp, unsigned int cellId                         );

/* update gain for all cells                                                         */
void         fpUpdateGainAll(floorplan *fp                                           );

/* unlock all cells                                                                  */
void         fpUnlockAll(floorplan *fp                                                );

/* perform K&L paratitioning algorithm 'passes' times, if 'passes' is UINT_MAX, then */
/* perform the algorithm 'netral' times after no further improvement is achieved     */
void       fpPartition(floorplan *fp, unsigned int passes,unsigned int maxIneffective);


#endif /* __FLOORPLAN_HEADER__                                                       */

