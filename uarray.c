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
** farray.h: unsigned integer array module                                        **
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
#include "uarray.h"
#include "allocation.h"
#include "random.h"

#define isEqu(a,b,s) ((s) ? ((int)(a)==(int)(b)) : ((unsigned int)(a)==(unsigned int)(b)))
#define isNeq(a,b,s) ((s) ? ((int)(a)!=(int)(b)) : ((unsigned int)(a)!=(unsigned int)(b)))
#define isGth(a,b,s) ((s) ? ((int)(a) >(int)(b)) : ((unsigned int)(a) >(unsigned int)(b)))
#define isSth(a,b,s) ((s) ? ((int)(a) <(int)(b)) : ((unsigned int)(a) <(unsigned int)(b)))

/* return memory allocate unsigned int array; 'uarrSize' size */
unsigned int* uarrAlloc(unsigned int uarrSize) {
  return (unsigned int*)safeMalloc(sizeof(unsigned int)*uarrSize);
}

/* returns index of minimum number of an array */
unsigned int uarrMinInd(unsigned int *uarr, unsigned int uarrSize, short int isSigned ) {
    unsigned int i;
    unsigned int min=uarr[0];
    unsigned int minind=0;
    for (i=0;i<uarrSize;i++) {
      if (isSth(uarr[i],min,isSigned)) { /* new minimum */
        min=uarr[i];
        minind=i;
      }
    }
    return minind;
}

/* returns index of maximum number of an array */
unsigned int uarrMaxInd(unsigned int *uarr, unsigned int uarrSize, short int isSigned) {
    unsigned int i;
    unsigned int max=uarr[0];
    unsigned int maxind=0;
    for (i=0;i<uarrSize;i++) {
      if (isGth(uarr[i],max,isSigned)) { /* new maximum */
        max=uarr[i];
        maxind=i;
      }
    }
    return maxind;
}

/* returns minimum number of an array */
unsigned int uarrMin(unsigned int *uarr, unsigned int uarrSize, short int isSigned ) {
    unsigned int i;
    unsigned int min=uarr[0];
    for (i=0;i<uarrSize;i++)
      if (isSth(uarr[i],min,isSigned)) min=uarr[i]; /* new minimum */
    return min;
}

/* returns index of maximum number of an array */
unsigned int uarrMax(unsigned int *uarr, unsigned int uarrSize, short int isSigned ) {
    unsigned int i;
    unsigned int max=uarr[0];
    for (i=0;i<uarrSize;i++)
      if (isGth(uarr[i],max,isSigned)) max=uarr[i]; /* new maximum */
    return max;
}

/* returns the index of the only positive value (when all others are zero), else return UINT_MAX */
unsigned int uarrOnlyPositiveInd(unsigned int *uarr, unsigned int uarrSize) {
  unsigned int i          ;
  unsigned int foundInd = UINT_MAX;
  unsigned int found    = 0    ;
  for(i=0;i<uarrSize;i++) {
    if ((uarr[i])>0) {
      if (found)  return UINT_MAX;  /* another positive found */
      else {              /* first positive found */
        found    = 1;
        foundInd = i;
      }
    }
  } /* for i */
  if (!found)  return UINT_MAX; /* no positive numbers            */
  else         return foundInd; /* return index of the only positive number */
}

/* if all array element are equal, return that value, else return UINT_MAX */
unsigned int uarrSameValue(unsigned int *uarr, unsigned int uarrSize) {
  unsigned int i;
  if (uarrSize == 0) return UINT_MAX;
  if (uarrSize == 1) return uarr[0] ;
  for(i=1;i<uarrSize;i++)
    if (uarr[i] != uarr[0]) return UINT_MAX;
  return uarr[0];
}

/* returns the count of 'value' in 'arr'                                               */
unsigned int uarrCountValue(unsigned int *uarr, unsigned int uarrSize, unsigned int val){
  unsigned int count=0; /* initial count       */
  unsigned int i      ; /* array index counter */
  for(i=0;i<uarrSize;i++)
    if (uarr[i] == val) count++;
  return count;
}

/* copy array values from source array to target array                */
void uarrCopy(unsigned int *source, unsigned int *target,unsigned int uarrSize){
  unsigned int i;
  for (i=0;i<uarrSize;i++)
    target[i]=source[i]; /* copy source to target element */
}


/* add values in first array to second array */
void uarrAdd(unsigned int *uarr, unsigned int *uacc,unsigned int uarrSize) {
  unsigned int i;
  for (i=0;i<uarrSize;i++)
    uacc[i]+=uarr[i]; /* add uarr to uacc element by element */
}

/* initialize all array elements to initVal value */
void uarrInit(unsigned int *uarr, unsigned int initVal, unsigned int uarrSize) {
  int i;
  for (i=0;i<uarrSize;i++)
    uarr[i]=initVal; /* init all arr elements to initVal */
}

/* generate a random numbers array between minRand and maxRand with different numbers */
void uarrRandInit( unsigned int *uarr   , unsigned int  uarrSize   ,
                   unsigned int  minRand, unsigned int  maxRand   ){
  unsigned int randNum, i;
  for(i=0; i<uarrSize; i++) {
    randNum = getUIntRand(minRand,maxRand);
    while (uarrValueFound(uarr,uarrSize,randNum,0,i-1)) /* until not found previously */
      randNum = getUIntRand(minRand,maxRand);           /* generate new number        */
    uarr[i] = randNum;
  }
}

/* return 1 if 'value' is found in the array between fromInd to toInd, 0 otherwise */
int uarrValueFound( unsigned int *uarr   , unsigned int uarrSize, unsigned int val  ,
                    unsigned int  fromInd, unsigned int toInd                      ){
  unsigned int i;
  if ((fromInd>toInd)||(toInd>=uarrSize)) return 0;
  for(i=fromInd; i<=toInd; i++)
    if (uarr[i] == val) return 1;
  return 0;
}

/* dump array's values to stdout delimited with 'delimiter' string */
void uarrDump(unsigned int *uarr, unsigned int uarrSize, char *delim, short int isSigned){
  unsigned int i;
  for (i=0;i<uarrSize;i++) {
    if (isSigned)
      printf("%d",(int)uarr[i]);
    else
      printf("%u",uarr[i]);
    if (i != uarrSize-1) /* not last element */
      printf("%s",delim);
  }
}
