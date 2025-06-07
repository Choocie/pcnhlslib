#include "dut.h"

void dut(hls::stream<T> inputStream[PAR],
         hls::stream<int> &numStream,
         hls::stream<T> outputStream[PAR]) {
    padding<T,PAR,II>(
        inputStream,
        numStream,
        outputStream);
}