#ifndef DUT_H__
#define DUT_H__

#include "util.h"

const int N = 16;
const int PAR   = 2;
const int II    = 8;
const int BETA_OFFSET = 1;
const int F = 2;

typedef int  T; 

void dut(hls::stream<array<T,F>> inputStream[PAR],
            hls::stream<int> &numStream,
            hls::stream<ap_uint<N>> &condensationPointsStream,
            hls::stream<array<T,F>> outputStream[PAR],
            hls::stream<bool> lastStream[PAR]);

#endif