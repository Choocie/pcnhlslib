#ifndef DUT_H__
#define DUT_H__

#include "top_k_bitonic_sort.h"
#include "hls_stream.h"



const int N = 8;
const int K = 4;

typedef ap_fixed<7,1>  D;
typedef ap_fixed<7,1>  P;

void dut(hls::stream<array<D, K>> keysInStream[N / K],
        hls::stream<array<P, K>> valuesInStream[N / K],
        hls::stream<array<D, K>> &keysOutStream,
        hls::stream<array<P, K>> &valuesOutStream);

#endif
