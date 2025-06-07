#include "dut.h"

void dut(hls::stream<array<T,WIDTH>> &aInputStream,
         hls::stream<array<T,WIDTH>> &bInputStream,
         hls::stream<array<T,2*WIDTH>> &outputStream) {
    merge<T,WIDTH,II>(aInputStream,bInputStream,outputStream);
}