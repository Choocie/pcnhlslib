#include "dut.h"

void dut(hls::stream<array<D, K>> keysInStream[N / K],
         hls::stream<array<P, K>> valuesInStream[N / K],
         hls::stream<array<D, K>> &keysOutStream,
         hls::stream<array<P, K>> &valuesOutStream) {
   #pragma HLS dataflow
   top_k_bitonic_sort<D,P,N,K,1>(keysInStream,valuesInStream, keysOutStream,valuesOutStream);
}