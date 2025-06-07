#include "dut.h"

void dut(hls::stream<array<T,F>> inputStream[PAR],
            hls::stream<int> &numStream,
            hls::stream<ap_uint<N>> &condensationPointsStream,
            hls::stream<array<T,F>> outputStream[PAR],
            hls::stream<bool> lastStream[PAR]) {
    filter<T,N,PAR,II,F,BETA_OFFSET>(
        inputStream,
        numStream,
        condensationPointsStream,
        outputStream,
        lastStream);
}