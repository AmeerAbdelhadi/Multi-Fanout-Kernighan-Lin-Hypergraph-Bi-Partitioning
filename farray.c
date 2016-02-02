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
** farray.c: float array module                                                   **
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
#include "farray.h"
#include "allocation.h"
#include "random.h"

/* returns memory allocation of a float array; 'farrSize' size */
float* farrAlloc(unsigned int farrSize) {
  return (float*)safeMalloc(sizeof(float)*farrSize);
}

/* returns index of minimum number of an array */
unsigned int farrMinInd(float *farr, unsigned int farrSize) {
    unsigned int i;
    float min=farr[0];
    unsigned int minind=0;
    for (i=0;i<farrSize;i++) {
      if (farr[i]<min) { /* new minimum */
        min=farr[i];
        minind=i;
      }
    }
    return minind;
}

/* returns index of maximum number of an array */
unsigned int farrMaxInd(float *farr, unsigned int farrSize) {
    unsigned int i;
    float max=farr[0];
    unsigned int maxind=0;
    for (i=0;i<farrSize;i++) {
      if (farr[i]>max) { /* new maximum */
        max=farr[i];
        maxind=i;
      }
    }
    return maxind;
}

/* returns minimum number of an array */
float farrMin(float *farr, unsigned int farrSize ) {
    unsigned int i;
    float min=farr[0];
    for (i=0;i<farrSize;i++)
      if (farr[i]<min) min=farr[i]; /* new minimum */
    return min;
}

/* returns index of maximum number of an array */
float farrMax(float *farr, unsigned int farrSize ) {
    unsigned int i;
    float max=farr[0];
    for (i=0;i<farrSize;i++)
      if (farr[i]>max) max=farr[i]; /* new maximum */
    return max;
}

/* if all array element are equal, return that value, else return FLT_MAX */
float  farrSameValue(float *farr, unsigned int farrSize) {
  unsigned int i;
  if (farrSize == 0) return FLT_MAX;
  if (farrSize == 1) return farr[0];
  for(i=1;i<farrSize;i++)
    if (farr[i] != farr[0]) return FLT_MAX;
  return farr[0];
}

/* copy array values from source array to target array */
void farrCopy(float *source, float *target,unsigned int farrSize){
  unsigned int i;
  for (i=0;i<farrSize;i++)
    target[i]=source[i]; /* copy source to target element */
}


/* add values in first array to second array */
void farrAdd(float *farr, float *facc,unsigned int farrSize) {
  unsigned int i;
  for (i=0;i<farrSize;i++)
    facc[i]+=farr[i]; /* add farr to facc by element */
}

/* initialize all array elements to initVal value */
void farrInit(float *farr, float initVal, unsigned int farrSize) {
  unsigned int i;
  for (i=0;i<farrSize;i++) farr[i]=initVal; /* init all arr elements to initVal */
}

/* return 1 if 'value' is found in the array between fromInd to toInd, 0 otherwise */
short int farrValueFound( float *farr         , unsigned int farrSize , float val   ,
                          unsigned int fromInd, unsigned int toInd                 ){
  unsigned int i;
  if ((fromInd>toInd)||(toInd>=farrSize)) return 0;
  for(i=fromInd; i<=toInd; i++)
    if (farr[i] == val) return 1;
  return 0;
}

/* dump array's values to stdout delimited with 'delimiter' string */
void farrDump(float *farr, unsigned int farrSize, char *delim){
  unsigned int i;
  for (i=0;i<farrSize;i++) {
    printf("%f",farr[i]);
    if (i != farrSize-1) /* not last element */
      printf("%s",delim);
  }
}
