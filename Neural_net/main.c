/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <olololwtfomg@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Jaroslav Mucka
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "neural.h"
#include <time.h>
#include "genetics.h"

#define INPUT_NEURONS 6

int main()
{
    //unsigned short input[6]={255,255,0,0,0,0};
    srand(time(NULL));
    //Reg_InitNeural();
    //Reg_PrintWeightsAll();
    //Reg_Neural(input, STEPS);
    //printf("Hello world!\n");
    //Reg_PrintOutputs();
    initTestCases();
    initJedinci(JEDINEC_NUM);
    generationFermentation();
    holocaust();
    return 0;
}
/*
.  ...............
         .ldl:;;;;cccccclloddxkkxxxxoc;.
        lk.   ...,'..........'''......';:lc.
       xd   ..'',.....  ..      .......   .ox
      xo        .;cllc:' .     .'           K,
   .cOo..   ...O0NWWWxc:ok.   ..lxKX00k.   .;xo.
  ol'.,:::cdl;...'lc .,:ll.   :0O;'...  ..'.,;:K:
 kc. ol  ck'';:::;.            cO.  .oo0lc,:..'xx
 K;..k,,oNl;loo;.    .,kcl'.    .x0:.   .K, ..:0'
 .k:'.,  :Xo,.cNlllcc:;k,,,,. ;;lc.. ..cOWW'.,0'
   ;xc    .OkcOM0c,. c0:cccld:lkdlclx0cklOM: k;
     lk     oxcN,cxKNNOl;'.:0..,Xc''oWl0NNM: 0
      ld     .xO:   ,K,cdkKWWMMMMMMMMMMWMNW. 0
       ,xc.    .;ddoX:    '0 ..d0;;Nlo0cN0:  k.
         'odo:,:;..,:oddooOXcccKxc0XdKkdl.   lc
            'cllo:.,;,.......'''''.   ...  . ;d
                .;ccc;........... ...',....  ,x
                     'llc::;.               ,k.
                         .';:c::::cc:::cclll;
                                    ....
*/
