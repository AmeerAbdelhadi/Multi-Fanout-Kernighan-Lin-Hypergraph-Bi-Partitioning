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


#ifndef __UARRAY_HEADER__
#define __UARRAY_HEADER__

/* return memory allocate unsigned int array; 'uarrSize' size                           */
unsigned int* uarrAlloc (unsigned int uarrSize                                          );

/* returns index of minimum number                                                      */
unsigned int  uarrMinInd(unsigned int *uarr, unsigned int uarrSize, short int isSigned  );

/* returns index of maximum number                                                      */
unsigned int  uarrMaxInd(unsigned int *uarr, unsigned int uarrSize, short int isSigned  );

/* returns minimum number of an array                                                   */
unsigned int  uarrMin   (unsigned int *uarr, unsigned int uarrSize, short int isSigned  );

/* returns index of maximum number of an array                                          */
unsigned int  uarrMax   (unsigned int *uarr, unsigned int uarrSize, short int isSigned  );

/* returns the index of the only positive value (all others 0), else return UINT_MAX    */
unsigned int  uarrOnePositiveInd(unsigned int *uarr, unsigned int uarrSize              );

/* if all array element are equal, return that value, else return UINT_MAX              */
unsigned int  uarrSameValue     (unsigned int *uarr, unsigned int uarrSize              );

/* returns the count of 'value' in 'arr'                                                */
unsigned int  uarrCountValue(unsigned int *uarr, unsigned int uarrSize, unsigned int val);

/* copy array values from source array to target array                                  */
void          uarrCopy(unsigned int *source, unsigned int *target,unsigned int uarrSize );

/* add values in first array to second array                                            */
void          uarrAdd (unsigned int *uarr  , unsigned int *uacc  ,unsigned int uarrSize );

/* initialize all array elements to initVal value                                       */
void          uarrInit(unsigned int *uarr, unsigned int initVal, unsigned int uarrSize  );

/* generate a random numbers array between minRand and maxRand with different numbers   */
void          uarrRandInit( unsigned int *uarr   , unsigned int uarrSize                 ,
                            unsigned int  minRand, unsigned int maxRand                 );

/* return 1 if 'value' is found in the array between fromInd to toInd, inclusive        */
int           uarrValueFound( unsigned int *uarr, unsigned int uarrSize, unsigned int val,
                              unsigned int fromInd, unsigned int toInd                  );

/* dump array's values to stdout delimited with 'delimiter' string                      */
void          uarrDump( unsigned int *uarr , unsigned int uarrSize                       ,
                        char         *delim, short    int isSigned                      );

#endif /* __UARRAY_HEADER__                                                             */
