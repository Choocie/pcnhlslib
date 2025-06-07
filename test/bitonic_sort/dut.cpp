#include "dut.h"

void dut(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<D, N>> &keysOutStream,
                  hls::stream<array<P, N>> &valuesOutStream){
  #pragma HLS dataflow
  bitonic_sort<D,P,N>
  (keysInStream,
  valuesInStream,
  keysOutStream,
  valuesOutStream);
}

