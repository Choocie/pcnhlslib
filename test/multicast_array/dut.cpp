#include "dut.h"


void dut(hls::stream<T> inputStream[IN_PAR],
		 hls::stream<T> outputStream[OUT_PAR][IN_PAR]) {
    multicast<T,
              IN_PAR,
              OUT_PAR,
              II>(inputStream,
                  outputStream);
}