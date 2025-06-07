#include "dut.h"


void dut(hls::stream<array<T, INPUT_WIDTH_1>> &in1Stream,
         hls::stream<array<T, INPUT_WIDTH_2>> &in2Stream,
         hls::stream<array<T, INPUT_WIDTH_3>> &in3Stream,
         hls::stream<array<T, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3>> &outStream) {
    concat<T,
           T,
           T,
           T,
           INPUT_WIDTH_1,
           INPUT_WIDTH_2,
           INPUT_WIDTH_3,
           II>(in1Stream,
               in2Stream,
               in3Stream,
               outStream);
}