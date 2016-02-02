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
** geometry.h: geometry module                                                    **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#ifndef __GEOMETRY_HEADER__
#define __GEOMETRY_HEADER__

#define MAX(a,b)       ( ((a) > (b)) ? (a   ) : (b)    ) /* returns max of two numbers      */
#define MIN(a,b)       ( ((a) > (b)) ? (b   ) : (a)    ) /* returns min of two numbers      */
#define ABS(x)         ( ((x) <  0 ) ? (-(x)) : (x)    ) /* returns absolute value          */
#define SIGN(x)        ( ((x)== 0)?0:(((x)>0)?1:-1)    ) /* returns sign                    */
#define RATIO(x,y)     ( (MIN((x),(y)))/(MAX((x),(y))) ) /* returns ratio of x,y (0..1)     */
#define IsOdd(n)       ( (n) & 1                       ) /* determines if n is odd          */
#define IsEven(n)      ( !(ISODD(n))                   ) /* determines if n is even         */
#define IsNotMult(m,d) ( (m) % (d)                     ) /* determines if d isnt m multiple */
#define IsMult(m,d)    ( !(IsNotMult(m,d))             ) /* determines if d is m multiple   */
#define IsNot2ToN(n)   ( (n) & ((n)-1)                 ) /* determines if n is not 2^N      */
#define Is2ToN(n)      ( !(IsNot2ToN(n))               ) /* determines if n is 2^N          */

struct pointStruct {
  unsigned int x; /* x xoordinate of a point */
  unsigned int y; /* y xoordinate of a point */
};
typedef struct pointStruct point;

struct fPointStruct {
  float x; /* x xoordinate of a point */
  float y; /* y xoordinate of a point */
};
typedef struct fPointStruct fPoint;

/* change 2D index (ix,iy) in (nx,ny) size world into 1D linear world index             */
unsigned int index2Dto1D(unsigned int ix,unsigned int iy,unsigned int nx,unsigned int ny);

/* change 1D linear world index ix into 2D index in (nx,ny) size world                  */
point        index1Dto2D(unsigned int ix                ,unsigned int nx,unsigned int ny);

#endif /* __GEOMETRY_HEADER__                                                           */
