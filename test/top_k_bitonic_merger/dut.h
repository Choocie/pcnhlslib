#ifndef DUT_H__
#define DUT_H__

#include "top_k_bitonic_merger.h"
#include "hls_stream.h"



const int N = 8;
const int K = 4;

typedef ap_fixed<16,1> T;
typedef ap_fixed<16,1> D;
typedef ap_fixed<16,1> P;

void dut(hls::stream<array<D, N / 2>> &keysAInStream, hls::stream<array<P, N / 2>> &valuesAInStream,
        hls::stream<array<D, N / 2>> &keysBInStream, hls::stream<array<P, N / 2>> &valuesBInStream,
        hls::stream<array<D, K>> &keysOutStream, hls::stream<array<P, K>> &valuesOutStream);

#endif
