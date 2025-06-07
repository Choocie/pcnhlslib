#include "dut.h"


void dut(hls::stream<T> &inputStream,
	   hls::stream<T> outputStream[PAR]) {
    multicast<T,
              PAR,
              II>(inputStream,
                  outputStream);
}