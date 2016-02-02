#! /bin/tcsh -f
#############################################################################################
##     Copyright (c) 2011, University of British Columbia (UBC); All rights reserved.      ##
##                                                                                         ##
##     Redistribution  and  use  in  source   and  binary  forms,   with  or  without      ##
##     modification,  are permitted  provided that  the following conditions are met:      ##
##       * Redistributions   of  source   code  must  retain   the   above  copyright      ##
##         notice,  this   list   of   conditions   and   the  following  disclaimer.      ##
##       * Redistributions  in  binary  form  must  reproduce  the  above   copyright      ##
##         notice, this  list  of  conditions  and the  following  disclaimer in  the      ##
##         documentation and/or  other  materials  provided  with  the  distribution.      ##
##       * Neither the name of the University of British Columbia (UBC) nor the names      ##
##         of   its   contributors  may  be  used  to  endorse  or   promote products      ##
##         derived from  this  software without  specific  prior  written permission.      ##
##                                                                                         ##
##     THIS  SOFTWARE IS  PROVIDED  BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"      ##
##     AND  ANY EXPRESS  OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE      ##
##     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE      ##
##     DISCLAIMED.  IN NO  EVENT SHALL University of British Columbia (UBC) BE LIABLE      ##
##     FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL      ##
##     DAMAGES  (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR      ##
##     SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER      ##
##     CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN CONTRACT, STRICT LIABILITY,      ##
##     OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE      ##
##     OF  THIS SOFTWARE,  EVEN  IF  ADVISED  OF  THE  POSSIBILITY  OF  SUCH  DAMAGE.      ##
#############################################################################################

#############################################################################################
## runall.csh:Multi-fanout Kernighan-Lin hypergraphs bi-partitioning - run-in-batch script ##
## Run all test cases in batch for all gain functions and alpha combinations Receives one  ##
## commandline argument indicating the path to testcases files Results are logged in 'out' ##
## folder, and summerized in results.{nets.pass}                                           ##
##                                                                                         ##
##                             Ameer Abdelhadi  (C) April 2011                             ##
#############################################################################################

# tune the following parameters, if required
set alphas       = ( 0 0.2 0.4 0.6 0.8 1 1.5 2 2.5 3 )
set gainFuncs    = ( 0 1 2 )

echo
echo "Enhanced  Kernighan-Lin  based  bi-partitioning algorithm for hypergraphs"
echo "Run all testcases in batch for all gain functions and alphas combinations"
echo "Receives  one commandline argument indicating the path to testcases files"
echo "Results are logged in 'out' folder, and summerized in results.{nets.pass}"
echo

# create out folder, if not existing
if (!(-d out)) then
  mkdir out
endif

# print headers
printf "GainF" > out/results.nets
foreach gainFunc ($gainFuncs)
  if ($gainFunc == 0) then
    printf "\t0" >> out/results.nets
  else
    repeat $#alphas printf "\t$gainFunc" >> out/results.nets
  endif
end
  
printf "\nAlpha" >> out/results.nets
foreach gainFunc ($gainFuncs)
  if ($gainFunc == 0) then
    printf "\tN\\A" >> out/results.nets
  else
    foreach alpha ($alphas)
      printf "\t$alpha" >> out/results.nets
    end
  endif
end

cp -f out/results.nets out/results.pass

# for all infiles
foreach inFile ($1/*.txt)
  set designName=`echo $inFile|cut -d. -f1|rev|cut -d/ -f1|rev`
  echo " Run design: $designName ..."
  printf "\n$designName" >> out/results.nets
  printf "\n$designName" >> out/results.pass

  # for all gain functions
  foreach gainFunc ($gainFuncs)
    if ($gainFunc == 0) then
      echo "  With gain function# $gainFunc ..."
      partitioning $inFile -f $gainFunc >> out/$designName.$gainFunc.out
      set passes2converge = `grep Convereged out/$designName.$gainFunc.out | cut -d" " -f4`
      set crossingNets    = `grep Convereged out/$designName.$gainFunc.out | cut -d" " -f7`
      printf "\t$crossingNets"    >> out/results.nets
      printf "\t$passes2converge" >> out/results.pass
    else

      # if gain functions are 1 or 2, run for all alphas
      foreach alpha ($alphas)
        echo "  With gain function# $gainFunc and alpha=$alpha..."
        partitioning $inFile -f $gainFunc -a $alpha >> out/$designName.$gainFunc.$alpha.out
        set passes2converge = `grep Convereged out/$designName.$gainFunc.$alpha.out | cut -d" " -f4`
        set crossingNets    = `grep Convereged out/$designName.$gainFunc.$alpha.out | cut -d" " -f7`
        printf "\t$crossingNets"    >> out/results.nets
        printf "\t$passes2converge" >> out/results.pass
      end

    endif
  end

end

unset inFile
unset designName
unset gainFunc
unset passes2converge
unset crossingNets
unset alpha
unset alphas
unset gainFuncs
