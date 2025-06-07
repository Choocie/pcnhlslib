#include "dut.h"

void dut(input_t queryRow[N*F_C],
         input_t pointRow[N*F_C],
         hls::stream<array<distance_t,N>> distanceStream[PAR],
         hls::stream<int> &numStream) {
                calculateDistances<input_t,distance_t,accum_t,F_C,N,PAR,II>
                (queryRow,
                pointRow,
                distanceStream,
                numStream);
}