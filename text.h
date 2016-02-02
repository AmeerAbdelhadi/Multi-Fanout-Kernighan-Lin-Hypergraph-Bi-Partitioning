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
** text.h: text printing module                                                   **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Apr. 2011                 **
***********************************************************************************/


#ifndef __TEXT_HEADER__
#define __TEXT_HEADER__

#include <stdio.h>
#include <stdlib.h>

/**************************************************************************/
/* printf attributes and colors macros                                    */
/**************************************************************************/
/* foreground colors enumeration                                          */
enum fgColor { fgBlack=30, fgRed    , fgGreen, fgYellow                    ,
               fgBlue    , fgMagenta, fgCyan , fgWhite                    };
/* background colors enumeration                                          */
enum bgColor { bgBlack=40, bgRed    , bgGreen, bgYellow                    ,
               bgBlue    , bgMagenta, bgCyan , bgWhite                    };
/* text attributes enumeration                                            */
enum txtAttr { txtOff =0 ,txtBold   , txtUnderscore                        ,
               txtBlink  ,txtReverse, txtConceal                          };
/**************************************************************************/

/**************************************************************************/
/* macros to set print attributes and colors                              */
/**************************************************************************/
/* set text attributes, foreground color, and background color            */
#define prntSet(txa,fgc,bgc) (printf("%c[%d;%d;%dm",0x1B,(txa),(fgc),(bgc)))
/* reset arreibutes and colors                                            */
#define prntRst              (printf("%c[%dm"      ,0x1B,0)                )
/* print in bold                                                          */
#define prntBld              (printf("%c[%dm"      ,0x1B,1)                )
/* print in red (not bold)                                                */
#define prntRed              (printf("%c[%d;%dm"   ,0x1B,0,fgRed)          )
/* print new line                                                         */
#define prntNln              (printf("\n")                                 )
/**************************************************************************/

#endif /* __TEXT_HEADER__                                                       */
