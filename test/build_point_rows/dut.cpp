#include "dut.h"


void dut(hls::stream<array<T,F_C>> inCoordinateStream[PAR],
		 T query[N * F_C],
		 T points[N * F_C]) {
    buildPointRows<T,
                   F_C,
                   N,
                   PAR,
                   II>(inCoordinateStream,
                       query,
                       points);
}