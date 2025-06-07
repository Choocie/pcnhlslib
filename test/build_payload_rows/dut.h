#ifndef DUT_H__
#define DUT_H__

#include "gravnetconv.h"
#include "util.h"

const int F_P = 1;
const int N   = 8;
const int PAR = 2;
const int II  = 4;

typedef ap_fixed<8,1>  T; 

void dut(hls::stream<array<T,F_P>> inPayloadStream[PAR],
		 hls::stream<array<array<T,F_P>,N>> outPayloadStream[PAR]) ;
#endif