#ifndef DUT_H__
#define DUT_H__

#include "bitonic_sort.h"
#include "hls_stream.h"



const int N = 5;

typedef ap_fixed<16,1> T;
typedef ap_fixed<16,1> D;
typedef ap_fixed<16,1> P;

void dut(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<D, N>> &keysOutStream,
                  hls::stream<array<P, N>> &valuesOutStream);

#endif
