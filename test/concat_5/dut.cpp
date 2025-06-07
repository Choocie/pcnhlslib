#include "dut.h"


void dut(hls::stream<array<T, INPUT_WIDTH_1>> &in1Stream,
         hls::stream<array<T, INPUT_WIDTH_2>> &in2Stream,
         hls::stream<array<T, INPUT_WIDTH_3>> &in3Stream,
         hls::stream<array<T, INPUT_WIDTH_4>> &in4Stream,
         hls::stream<array<T, INPUT_WIDTH_5>> &in5Stream,
         hls::stream<array<T, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5>> &outStream) {
    concat<T,
           INPUT_WIDTH_1,
           INPUT_WIDTH_2,
           INPUT_WIDTH_3,
           INPUT_WIDTH_4,
           INPUT_WIDTH_5,
           II>(in1Stream,
               in2Stream,
               in3Stream,
               in4Stream,
               in5Stream,
               outStream);
}