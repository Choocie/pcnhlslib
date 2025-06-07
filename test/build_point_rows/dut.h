#ifndef DUT_H__
#define DUT_H__

#include "gravnetconv.h"
#include "util.h"

const int F_C = 1;
const int N   = 8;
const int PAR = 2;
const int II  = 4;

typedef ap_fixed<8,1>  T; 

void dut(hls::stream<array<T,F_C>> inCoordinateStream[PAR],
		 T query[N * F_C],
		 T points[N * F_C]) ;
#endif