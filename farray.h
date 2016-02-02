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
** farray.h: float array module                                                   **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#ifndef __FARRAY_HEADER__
#define __FARRAY_HEADER__

/* return memory allocate unsigned int array; 'arrSize' size                  */
float*       farrAlloc(unsigned int farrSize                                  );

/* returns index of minimum number                                            */
unsigned int farrMinInd    (float *farr, unsigned int farrSize                );

/* returns index of maximum number                                            */
unsigned int farrMaxInd    (float *farr, unsigned int farrSize                );

/* returns minimum number of an array                                         */
float        farrMin       (float *farr, unsigned int farrSize                );

/* returns index of maximum number of an array                                */
float        farrMax       (float *farr, unsigned int farrSize                );

/* if all array element are equal, return that value, else return FLT_MAX     */
float farrSameValue (float *farr, unsigned int farrSize                       );

/* copy array values from source array to target array                        */
void         farrCopy      (float *source, float *target,unsigned int farrSize);

/* add values in first array to second array                                  */
void         farrAdd       (float *farr, float *facc,unsigned int farrSize    );

/* initialize all array elements to initVal value                             */
void         farrInit      (float *farr, float initVal, unsigned int farrSize );

/* return 1 if 'val' is found in 'farr' between fromInd to toInd, inclusive   */
short int    farrValueFound(float *farr, unsigned int arrSize, float val       ,
                            unsigned int fromInd, unsigned int toInd          );

/* dump array's values to stdout delimited with 'delimiter' string            */
void         farrDump      (float *farr, unsigned int arrSize, char *delim    );

#endif /* __FARRAY_HEADER__                                                   */
