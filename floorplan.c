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
** floorplan.c: floorplan, cell, and net module for the multi-fanout Kernighan-Lin**
**              bi-partitioning algorithm for hypergraphs                         **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include "floorplan.h"
#include "graphics.h"
#include "uarray.h"
#include "farray.h"
#include "allocation.h"
#include "geometry.h"
#include "text.h"

/* returns a new allocated floorplan data structure, parsed from infile     */
floorplan *fpCreate(const char *fileName                                    ){

  FILE        *infile            ; /* input file                            */
  floorplan   *fp=NULL           ; /* floorplan data structure              */
  char         line[1024]        ; /* chars array holds one file line       */
  char         label[32]         ; /* general label                         */
  unsigned int buf[1024]         ; /* holds unsigned int values of one line */
  int          bufInd=-1         ; /* index for previous buffer             */
  char         delims[]=" \t\r\n"; /* token delimiters: space,tap,& newline */
  char        *strToken=NULL     ; /* string token from input file line     */
  unsigned int intToken          ; /* unsigned integer token                */
  unsigned int lineNum=1         ; /* current parsed line numer             */
  unsigned int neti=0,celli=0    ; /* net and cell counters                 */
  unsigned int curCell           ; /* current cell                          */
  unsigned int cellx,celly       ; /* cell palce                            */
  unsigned int nx,ny             ; /* floorplan size                        */
  unsigned int cellsN=0,netsN=0  ; /* number for cells/nets                 */
  unsigned int fpSize            ; /* floorplan area                        */

unsigned int *indicesArr=NULL;

  enum parseStates                 /* parsing states enum types             */
    {HEADER,NET,EXTRA}             /* header line, net line, or extra line  */
      parseState=HEADER          ; /* parsing state                         */

  /* open input file */
  infile=fopen(fileName,"r");
  if(infile==NULL) { /* open failed */
    printf("-E- File open error %s! Exiting...\n", fileName); exit(-1);
  }

  /* read a line, until end of file */
  while (fgets(line,sizeof line,infile) != NULL ) {

    bufInd=-1;
    strToken = strtok( line, delims );     /* read token from line */
    while(strToken != NULL) {              /* read all tokens      */
      if (sscanf(strToken,"%u",&intToken) == 0) {
        printf("-E- Parsing error: file %s, line %u: value is not positive integer! Exiting...\n",fileName,lineNum);
        exit(-1);
      }
          bufInd++                        ; /* holds number of tokens */
      buf[bufInd]=intToken            ; /* hold integer tokens    */
      strToken   = strtok(NULL,delims); /* next token             */
    }
    if (bufInd<0) continue; /* empty line */

    switch(parseState) {

      /*read gird size*/
      case HEADER:
      if (bufInd != 3) {
        printf("-E- Parsing error: file %s, line %u should include four integers! Exiting...\n",fileName,lineNum);
        exit(-1);
      }
      
      /* grid info */
      cellsN = buf[0];
      netsN  = buf[1];
      ny     = buf[2];
      nx     = buf[3];

      /* calculate smallest square/even floorplan dimension */
      nx = ceil(sqrt(cellsN+1));
      if (IsOdd(nx)) nx++;            
      ny = nx;
      
      //while (cellsN>(nx*ny)) ny++;
      fpSize = nx*ny;

      if (cellsN>fpSize) {
        printf("-E- Floorplan error: %u cell sites can't fit %u cells! Exiting...\n",fpSize,cellsN);
        exit(-1);
      }
      
      /* allocate and initialze floorplan */
      fp         = (floorplan*)safeMalloc(sizeof(floorplan));
      fp->nx     = nx;
      fp->ny     = ny;
      fp->cellsN = cellsN;
      fp->netsN  = netsN;
      fp->cells  = (cell*)safeMalloc(sizeof(cell)*cellsN);
      fp->nets   = (net* )safeMalloc(sizeof(net)*netsN  );
      fp->gain   = farrAlloc(cellsN);
      fp->group  = uarrAlloc(cellsN);
      fp->lock   = uarrAlloc(cellsN);

      /* allocate array for random indices */
      indicesArr = uarrAlloc(cellsN);

      /* initialize each cell to zero nets */
      for (celli=0;celli<cellsN;celli++) fp->cells[celli].netsN=0;

      /* read net info next */
      parseState=NET;

      break;

      /* read a net */
      case NET:
      if (neti==netsN-1) parseState=EXTRA; /* if finished check extra lines */
      if (bufInd < 0) { 
        printf("-E- Parsing error: file %s, line %u: Should include at least one integer! Exiting...\n",fileName,lineNum);
        fpDelete(fp);
        exit(-1);
      }
      if (bufInd != buf[0]) { /* incomplete */
        printf("-E- Parsing error: file %s, line %u: Should include %u integer numbers! Exiting...\n",fileName,lineNum, 1+buf[0]);
        fpDelete(fp);
        exit(-1);
      }
      
      /* allocate a net in floorplan nets database */
      fp->nets[neti].cellsN = buf[0];
      fp->nets[neti].cells  = uarrAlloc(buf[0]);
      
      for(celli=0;celli<buf[0];celli++) {
        fp->nets[neti].cells[celli] = buf[celli+1];
        (fp->cells[buf[celli+1]].netsN)++;
      }

      neti++; /* next net */

      break;

      /* check extra lines */
      case EXTRA:
      if (bufInd != -1) { 
        printf("-E- Parsing error in file %s, line %u: Extra line! Exiting...\n",fileName,lineNum);
        fpDelete(fp);
        exit(-1);
      }

    } /* switch */
  lineNum++; /* next line */
  } /* while fgets */

  /* cell initialization */
  uarrRandInit(indicesArr,cellsN,0,cellsN-1); /* random indices */
  cellx=0;celly=0;
  for(celli=0;celli<cellsN;celli++) {

    /* allocate nets list for each cell */
    fp->cells[celli].nets = uarrAlloc(fp->cells[celli].netsN);

    /* reset nets numbers for each cell to use as array index */
    fp->cells[celli].netsN = 0;

    /* associate each cell to a group                        */
    /* divide cells into two equal groups, or different by 1 */
    //indicesArr[celli]=celli;
    if (celli<(cellsN/2))  fp->group[indicesArr[celli]]=0;
    else                   fp->group[indicesArr[celli]]=1;
    
    /* unlock all cells */
    fp->lock[celli]=0;

  }

  /* initialize cells amount for each group */
  fp->groupCellsN[0] = cellsN/2;
  fp->groupCellsN[1] = cellsN-cellsN/2;

  /* upadte nets list for each cell */
  for(neti=0;neti<netsN;neti++){
    for(celli=0;celli<(fp->nets[neti].cellsN);celli++) {
      curCell = fp->nets[neti].cells[celli];
      fp->cells[curCell].nets[fp->cells[curCell].netsN] = neti;
      (fp->cells[curCell].netsN)++;

    }
  }

  /* update gains for all cells */
  fpUpdateGainAll(fp);

  /* update count of crossing nets */
  fp->crossingNetsN = netsN;
  fpUpdateCrossingNetsN(fp);

  /* update design name */
  strcpy(label,fileName);
  if (strrchr(label,'/')) /* find last '/' in the name */
    strcpy(fp->designName,strrchr(label,'/')+1); /* take everything after '/' */
  if (strrchr(fp->designName,'.')) { /* find last '.' in the name */
     strToken = strrchr(fp->designName,'.'); /* mark '.' as end of string */
    *strToken = '\0';
  }

  fclose(infile);
  return fp;

} /* fpCreate */

/* free a floorplan data structure */
void fpDelete(floorplan *fp) {

  unsigned int celli,neti; /* indexes */

  /* free net arrays form cells */
  for(celli=0;celli<(fp->cellsN);celli++) {
    safeFree(fp->cells[celli].nets);
  }
  
  /* free cell arrays from nets */
  for(neti=0;neti<(fp->netsN);neti++) {
    safeFree(fp->nets[neti].cells);
  }

  safeFree(fp->cells);
  safeFree(fp->nets );
  safeFree(fp->gain );
  safeFree(fp->group);
  safeFree(fp->lock);

} /* fpDelete */

/* dump a floorplan data structure to stdout - for verification purpose  */
void fpDump (floorplan  *fp) {

  unsigned int neti,celli; /* indexes */

  /* print floorplan details */
  printf("Floorplan:\n");
  printf("\tSize   : (%u,%u)\n",fp->nx,fp->ny);
  printf("\tCells #: %u\n",fp->cellsN);
  printf("\tNets  #: %u\n",fp->netsN);

  /* list nets */
  printf("Nets:\n");
  for(neti=0;neti<(fp->netsN);neti++) {
    printf("\tNet#%u: cells #: %u, cells:",neti,fp->nets[neti].cellsN);
    uarrDump(fp->nets[neti].cells,fp->nets[neti].cellsN," ",0);
    printf("\n");
  }

  /* list cells */
  printf("Cells:\n");
  for(celli=0;celli<(fp->cellsN);celli++) {
    uarrDump(fp->cells[celli].nets,fp->cells[celli].netsN," ",0);
    printf("\n");
  }

} /* fpDump */

/* draw floorplan using EasyGl graphics module. World size is xWorld*yWorld */
void fpDraw(floorplan *fp, float xWorld, float yWorld) {

  unsigned int nx=fp->nx, ny=fp->ny; /* floorplan dimention               */
  unsigned int i, j, i0, i1        ; /* general counters                  */
  unsigned int celli,neti          ; /* cell/net counters                 */
  point        pnt                 ; /* point to hold cell location       */
  float        step                ; /* grid step                         */
  char         label[1024]         ; /* general label                     */
  float        scaleStep           ; /* a step for improvement scale      */
  float        improvementRate     ; /* crossing_nets / total_nets        */
  unsigned int prvCell , curCell   ; /* previous/current cell number      */
  unsigned int srcCell0, srcCell1  ; /* source cell for each group number */
  net          curNet              ; /* current net                       */
  unsigned int palette=0           ; /* net colors                        */

  /* draw header, including title */
  setcolor(LIGHTGREY);
  fillrect(0,0,1000,40);
  setcolor(BLACK);
  setfontsize(13);
  drawtext(500,20,"Enhanced   Kernighan-Lin   (K&L)   based   bi-partitioning   algorithm   for   hypergraphs",1000);

  /* draw second line header, including partitioning parameters */
  setcolor(LIGHTGREY);
  fillrect(0,60,880,100);
  setcolor(BLACK);
  setfontsize(10);
  drawtext(440,80,gHeaderLabel,880);

  /* draw improvement scale */
  scaleStep=800/20;
  improvementRate = (float)(fp->crossingNetsN)/(float)(fp->netsN);
  setcolor(LIGHTGREY); fillrect(900,60 ,1000,940);
  setcolor(WHITE    ); fillrect(950,120,980 ,920);
  if (fp->isWorst) setcolor(RED); else setcolor(GREEN);
  fillrect(950,920-(improvementRate/0.05)*scaleStep, 980 ,920);
  setcolor(BLACK); drawrect(950,120,980 ,920);
  drawtext(950,75,"Crossing",100);
  drawtext(950,100,"Total Nets",100);
  drawline(910,85,990,85);
  for(i=1;i<=19;i++) {
    drawline(940,i*scaleStep+120,980,i*scaleStep+120);
    sprintf(label,"%.2f",1-i*.05);
    for(j=0; j<4; j++) label[j]=label[j+1]; /* shift label to remove leading zero */
    drawtext(920,i*scaleStep+120,label,30);
  }

  /* draw footer */
  setcolor(LIGHTGREY); fillrect(0,960,1000,1000);
  setcolor(BLACK    ); drawtext(500,980,gFooterLabel,1000);

  /* draw grid */

  /* draw separating line */
  setlinestyle (DASHED); setcolor(RED);
  drawline(440,120,440,900);

  /* draw two groups */
  setlinestyle (SOLID);
  setcolor(LIGHTGREY);
  fillrect(20,140,390,880);
  fillrect(490,140,860,880);
  setcolor(RED);
  drawrect(20,140,390,880);
  drawrect(490,140,860,880);

  /* draw groups/seperator labels */
  setcolor(BLACK);
  sprintf(label,"Cells#:%u",fp->groupCellsN[0]);
  drawtext(205,900,label,370);
  sprintf(label,"Cells#:%u",fp->groupCellsN[1]);
  drawtext(657,900,label,370);
  sprintf(label,"nets#:%u",fp->crossingNetsN);
  drawtext(440,920,label,200);
  step= (740./(float)(2*ny+2));

  /* draw cells */
  i0=0; i1=0;
  for(celli=0; celli<fp->cellsN; celli++) { /*  draw every cell */
    sprintf(label,"%u",celli);
    if (fp->group[celli] == 0) { /* if cell in group 0, draw in the left group */
      pnt = index1Dto2D(i0, nx/2, ny); /* set 2D location */
      fp->cells[celli].location.x = 20 +1.5*step+2*pnt.x*step;
      fp->cells[celli].location.y = 140+1.5*step+2*pnt.y*step;
      setcolor(BLACK);
      i0++;
    }
    else { /* if cell in group 1, draw in the right group */
      pnt = index1Dto2D(i1, nx/2, ny); /* set 2D location */
      fp->cells[celli].location.x = 490+1.5*step+2*pnt.x*step;
      fp->cells[celli].location.y = 140+1.5*step+2*pnt.y*step;
      setcolor(BLACK);
      i1++;
    }
    if (fp->lock[celli]) { /* if locked, draw with yellow */
      setcolor(YELLOW);
      fillarc(fp->cells[celli].location.x, fp->cells[celli].location.y, step/2, 0., 360.);
    }
    /* draw a cell as a filled circle */
    setcolor(BLACK);
    drawarc (fp->cells[celli].location.x, fp->cells[celli].location.y, step/2, 0., 360.);
    drawtext(fp->cells[celli].location.x, fp->cells[celli].location.y, label , step    );
  }

  /* draw nets */
  for(neti=0;neti<(fp->netsN);neti++) {
    curNet = fp->nets[neti];
    prvCell = curNet.cells[0];
    srcCell0 = UINT_MAX;
    srcCell1 = UINT_MAX;
    /* find net cells in each groups, if possible */
    for(celli=curNet.cellsN-1;celli!=UINT_MAX;celli--) {
      curCell = curNet.cells[celli];
        if (fp->group[curCell] == 0)
          srcCell0 = curCell;
        else 
          srcCell1 = curCell;
    }
    setcolor((enum color_types)((palette++)%7+4)); /* cahnge color */
    /* draw line between source cell and all other net cells */
    for(celli=0;celli<curNet.cellsN;celli++) {
      curCell = curNet.cells[celli];
        if (fp->group[curCell] == 0) { /* if chosen cell in group 0 */
          drawline(fp->cells[srcCell0].location.x,fp->cells[srcCell0].location.y,
               fp->cells[curCell ].location.x,fp->cells[curCell ].location.y);
        } else { /* if chosen cell in group 1 */
          drawline(fp->cells[srcCell1].location.x,fp->cells[srcCell1].location.y,
               fp->cells[curCell ].location.x,fp->cells[curCell ].location.y);
        }
    }
    /* draw line between the two groups */
    if ( (srcCell0 != UINT_MAX) && (srcCell1 != UINT_MAX) )
      drawline(fp->cells[srcCell0].location.x,fp->cells[srcCell0].location.y,
           fp->cells[srcCell1].location.x,fp->cells[srcCell1].location.y);
  }

} /* fpDraw */

/* if net is not crossing (in same group), return group number, else return UINT_MAX */
unsigned int fpNetGroups(floorplan *fp, unsigned int netId) {
  net curNet = fp->nets[netId];
  unsigned int* groupsArr = uarrAlloc(curNet.cellsN); /* hold cell groups */
  unsigned int i, curCellId;
  unsigned int netGroups;
  for(i=0;i<curNet.cellsN;i++){
    curCellId = curNet.cells[i];
    groupsArr[i] = fp->group[curCellId]; /* copy cell groups */
  }
  netGroups = uarrSameValue(groupsArr,curNet.cellsN); /* if same valuel, return */
  safeFree(groupsArr);
  return netGroups;
}

/* determines if the net indexed netId is crossing net */
short int fpIsNetCrossing(floorplan *fp, unsigned int netId) {
  return (fpNetGroups(fp,netId)==UINT_MAX)?1:0;
}

/* update total crossing nets amount at the database */
unsigned int fpUpdateCrossingNetsN(floorplan *fp){
  unsigned int totalCrossing=0;
  unsigned int neti;
  /* for each net, check if crossing and update count */
  for(neti=0;neti<(fp->netsN);neti++)
    if (fpIsNetCrossing(fp,neti)) totalCrossing++;
  /* check if new crossing is larger then previous */
  fp->isWorst = (totalCrossing>fp->crossingNetsN) ? 1 : 0;
  fp->crossingNetsN = totalCrossing; /* update db */
  return totalCrossing;
}

/* counts nets which connected to cellId and are crossing */
unsigned int fpCellCrossing(floorplan *fp, unsigned int cellId){
  cell curCell = fp->cells[cellId];
  unsigned int curNetId;
  unsigned int crossingCount=0;
  unsigned int neti;
  /* for each net connected to cellId, check if crossing */
  for(neti=0;neti<(curCell.netsN);neti++) {
    curNetId = curCell.nets[neti];
    if (fpIsNetCrossing(fp,curNetId)) crossingCount++;
  }
  return crossingCount;
}

/* count number of cells connected with netId and are in group 'group' */
unsigned int fpNetGroupCount(floorplan *fp, unsigned int netId, unsigned int group) {
  net curNet = fp->nets[netId];
  unsigned int* groupsArr = uarrAlloc(curNet.cellsN); /* groups array */
  unsigned int i, curCellId;
  unsigned int groupCount;
  for(i=0;i<curNet.cellsN;i++){
    curCellId = curNet.cells[i];
    groupsArr[i] = fp->group[curCellId]; /* copy groups */
  }
  groupCount = uarrCountValue(groupsArr,curNet.cellsN,group); /* return count */
  safeFree(groupsArr);
  return groupCount;
}

/* calculate overbalance for equal gain cells */
float fpCellOverbalance(floorplan *fp, unsigned int cellId){
  cell         curCell   = fp->cells[cellId];
  unsigned int cellGroup = fp->group[cellId];
  unsigned int curNetId;
  unsigned int neti;
  float cellCountA, cellCountB;
  float totalCountA=0, totalCountB=0;
  float overbalance=0;

  for(neti=0;neti<(curCell.netsN);neti++) { /* for each net connected ti cellId */
    curNetId = curCell.nets[neti];
    cellCountA = (float)fpNetGroupCount(fp,curNetId,cellGroup);       /* this net cell in group A */
    cellCountB = (float)fpNetGroupCount(fp,curNetId,(cellGroup+1)%2); /* this net cell in group B */

    switch (gGainFunc) {
      case 1: /* if K&L enhanced 1, just accumulate cells count */
        totalCountA += cellCountA;
        totalCountB += cellCountB;
        break;
      case 2: /* if K&L enhanced 2, calculate overbalance for each net, with extra weight to small group nets */
        overbalance += 0.5+SIGN(cellCountB-cellCountA)*pow(cellCountA+1,-gAlpha)*(1-RATIO(cellCountB,cellCountA))/2;        
    } /* switch gGainFunc*/

  } /* for nets */

  switch (gGainFunc) {
    case 1: /* if K&L enhanced 1, calculate overbalance for final count, with extra weight to small group */
      overbalance = 0.5+SIGN(totalCountB-totalCountA)*pow(totalCountA+1,-gAlpha)*(1-RATIO(totalCountB,totalCountA))/2;  
      break;
    case 2: /* if K&L enhanced 2, normalize overbalance */
      overbalance /= fp->cellsN;
  } /* switch gGainFunc*/

  return overbalance;

}

/* return and update gain for a cell in floorpaln DB */
float fpUpdateGain(floorplan *fp, unsigned int cellId){
  unsigned int crossingBefore, crossingAfter;
  if (  (fp->lock[cellId]) /* if locked or making unbalance */
     || ((fp->groupCellsN[0] < fp->groupCellsN[1]) && (fp->group[cellId] == 0))
     || ((fp->groupCellsN[1] < fp->groupCellsN[0]) && (fp->group[cellId] == 1)) )
       fp->gain[cellId] = (-1*FLT_MAX); /* assign -infinite gain */
  else {
    crossingBefore = fpCellCrossing(fp,cellId); /* calculate crossing before move */
    fp->group[cellId] = (fp->group[cellId] == 0) ? 1 : 0; /* try to move */
    crossingAfter  = fpCellCrossing(fp,cellId); /* calculate crossing after move */
    fp->group[cellId] = (fp->group[cellId] == 0) ? 1 : 0; /* move back */
    fp->gain[cellId] = (float)crossingBefore - (float)crossingAfter; /* K&L: gain is reduced crossing  */
    if (gGainFunc>0) fp->gain[cellId] += fpCellOverbalance(fp,cellId);   /* if enhanced K&L add balance    */
  }
  return fp->gain[cellId];
}

/* update gain for all cells */
void fpUpdateGainAll(floorplan *fp) {
  unsigned int celli;
  for(celli=0;celli<(fp->cellsN);celli++)
    fpUpdateGain(fp, celli); /* update gain for current cell */
}

/* unlock all cells */
void fpUnlockAll(floorplan *fp) {
  uarrInit(fp->lock,0,fp->cellsN);
}

/* perform paratitioning, 'passes is the number if algorithms passes, if passes is UINT_MAX, */
/* then perform the algorithms 'netral time' after no further improvement is achieved        */
void fpPartition(floorplan *fp, unsigned int passes,unsigned int maxIneffective              ){

         float* gainsArr            = farrAlloc(fp->cellsN);     /* gains array              */
  unsigned int* bestGroups          = uarrAlloc(fp->cellsN);     /* group assoiciation array */
  unsigned int  minCrossingNetsN    = fpUpdateCrossingNetsN(fp); /* minimum crossing nets    */
  unsigned int  prvMinCrossingNetsN = UINT_MAX;                  /* crossing# previous pass  */
  unsigned int  passi                ; /* passes counter                                     */
  unsigned int   celli               ; /* cells counter                                      */
  unsigned int  ineffectivePassN = 0 ; /* number of passes with same minimum crossing nets   */
  unsigned int  curCrossingNetsN     ; /* number of crossing nets for current step           */
  unsigned int  maxGainCellId        ; /* index of cell with max gain                        */
  unsigned int  bestGroupCellsN[2]   ; /* number of cells in each group for best solution    */
  char          label1[1024]         ; /* general label                                      */
  char          label2[1024]         ; /* general label                                      */

  uarrCopy(fp->group,bestGroups,fp->cellsN);
  bestGroupCellsN[0] = fp->groupCellsN[0];
  bestGroupCellsN[1] = fp->groupCellsN[1];

  /* update string label with gain function type */
  switch (gGainFunc) {
    case 0 :
      sprintf(label1,"Basic K&L");
      break;
    case 1 :
    case 2 :
      sprintf(label1,"Enhanced K&L #%u, alpha=%3.1f",gGainFunc,gAlpha);
      break;
    default: sprintf(label1,"Unknown");
  }

   if (passes == UINT_MAX)
     sprintf(label2,"m%u",maxIneffective);
   else
     sprintf(label2,"%u",passes);

  if (gVerbose) {
    prntRed; prntBld;
    printf("\nPerforming enhanced Kernighan-Lin (K&L) based bi-partitioning algorithm for hypergraphs:\n");
    prntRst;
    printf("Design name     : %s\n"  , fp->designName);
    printf("Cells#          : %u\n"  , fp->cellsN);
    printf("Nets#           : %u\n"  , fp->netsN);
    printf("Crossing nets#  : %u\n"  , fp->crossingNetsN);
    printf("Gain function   : %s\n"  , label1         );
    printf("Algorithm passes: %s\n\n", label2        );
  }
  if (gGUI) {
    /* update global message and wait for 'Proceed' to proceed */
    sprintf(gHeaderLabel,"Design: %s  |  Cells: %u  |  Nets: %u  |  Passes: %s  |  Gain func.: %s",
            fp->designName, fp->cellsN, fp->netsN, label2, label1);
    if (gRunMode=='f') { /* show initial floorplan for 'final' mode */
      sprintf(gFooterMessage,"Initial partitioning. Press 'Proceed' to find final solution");
      sprintf(gFooterLabel,"Initial partitioning");
      waitLoop();
      if (gPostScript) postscript(drawScreen);
    }
  }

  /******************** K&L ************************/

  /* iterate 'passes' iterations of the algorithms */
  for(passi=0;passi<passes;passi++){

    /* unlock all cells before starting a new pass */
    fpUnlockAll(fp);

    /* for 'pass' mode, show floorplan */
    if (gGUI && (gRunMode=='p')) {
      if (passi == 0) { /* first pass */
        sprintf(gFooterMessage,"Initial partitioning. Press 'Proceed' to preform pass# 1");
        sprintf(gFooterLabel,"Initial partitioning");
      } else {
        sprintf(gFooterMessage,"Initial partitioning. Press 'Proceed' to preform pass# %u",passi+1);
        sprintf(gFooterLabel,"partitioning after pass# %u",passi);
      }
      waitLoop();
      if (gPostScript) postscript(drawScreen);
    }

    if (gVerbose) {
      prntBld;      
      printf("- Performing pass# %u\n",passi+1);
      prntRst;
    }
  
    /* iterate 'cells amount' iteration (until all cells are locked) */
    for(celli=0;celli<(fp->cellsN);celli++){

      /* 'step' mode, display each step */
      if (gGUI && (gRunMode=='s')) {

        if ( (passi == 0) && (celli == 0) ) { /* first pass */
          sprintf(gFooterMessage,"Initial partitioning. Press 'Proceed' to preform step# 1 of pass# 1");
          sprintf(gFooterLabel,"Initial partitioning");
        } else {
          sprintf(gFooterMessage,"Initial partitioning. Press 'Proceed' to preform step# %u of pass# %u",celli+1,passi+1);
          if (celli==0)
            sprintf(gFooterLabel,"partitioning after step# %u of pass# %u",fp->cellsN,passi);
          else
            sprintf(gFooterLabel,"partitioning after step# %u of pass# %u",celli,passi+1);
        }
        waitLoop();
        if (gPostScript) postscript(drawScreen);
      }

      maxGainCellId = farrMaxInd(fp->gain,fp->cellsN); /* find index of max gain cell */
      if (fp->group[maxGainCellId] == 0) { /* if in group 0, move to 1 */
        fp->group[maxGainCellId] = 1;
        fp->groupCellsN[0]--;
        fp->groupCellsN[1]++;
      } else {                             /* if in group 1, move to 0 */
        fp->group[maxGainCellId] = 0;
        fp->groupCellsN[0]++;
        fp->groupCellsN[1]--;
      }

      fp->lock[maxGainCellId] = 1 ; /* lock this cell */
      fpUpdateGainAll(fp)         ; /* update gains   */
      curCrossingNetsN=fpUpdateCrossingNetsN(fp); /* calculate number of crossing nets */

      if ( (curCrossingNetsN <= minCrossingNetsN          ) &&     /* best solution */
           (ABS(fp->groupCellsN[0]-fp->groupCellsN[1]) < 2)    ) { /* and balanced  */
        /* save best and balanced solution */ 
        minCrossingNetsN = curCrossingNetsN;
        uarrCopy(fp->group,bestGroups,fp->cellsN);
        bestGroupCellsN[0] = fp->groupCellsN[0];
        bestGroupCellsN[1] = fp->groupCellsN[1];
      }

    } /* for cells */

    /* roll back to best and balanced solution */
    uarrCopy(bestGroups,fp->group,fp->cellsN);
    fp->groupCellsN[0] = bestGroupCellsN[0];
    fp->groupCellsN[1] = bestGroupCellsN[1];
    fpUpdateCrossingNetsN(fp);

    if (gVerbose) printf("  Minimal crossing nets numbet for this pass: %u\n\n",minCrossingNetsN);
 
    /* is previous crossings# equals to current? */
    if (prvMinCrossingNetsN == minCrossingNetsN) ineffectivePassN++; 
    /* if so, break loop after 3 ineffective passes  */
    if ((ineffectivePassN==(maxIneffective-1)) && (passes==UINT_MAX)) break;
    prvMinCrossingNetsN = minCrossingNetsN;

  } /* for passes */

  /******************** K&L ************************/

  fpUnlockAll(fp);

  /* finished! wait still until 'Exit' is pressed */
  if (gGUI) {
    /* update global message and wait for 'Proceed' to proceed */
    sprintf(gFooterMessage,"Finished! Final crossing nets number is %u - press 'Exit'", fp->crossingNetsN);
    sprintf(gFooterLabel  ,"Convereged after %u passes, with %u crossing nets", passi+1-ineffectivePassN, fp->crossingNetsN);
    waitLoop();
    if (gPostScript) postscript(drawScreen);
  }
  if (gVerbose) {
    prntRed;
    printf("* Convereged after %u passes, with %u crossing nets\n\n", passi+1-ineffectivePassN, fp->crossingNetsN);
    prntRst;
  }

  /* free temporal arrays */
  safeFree(gainsArr);
  safeFree(bestGroups);

}

