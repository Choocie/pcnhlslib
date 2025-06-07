#ifndef DUT_H__
#define DUT_H__


#include "gravnetconv.h"
#include "util.h"

const int K     = 4;
const int F_P   = 2;
const int II    = 2;

typedef ap_fixed<8,1>  T; 

void dut(hls::stream<array<T,(K-1)*F_P>> &inputStream,
	     hls::stream<array<T,F_P>> &maxStream,
	     hls::stream<array<T,F_P>> &avgStream) ;
#endif