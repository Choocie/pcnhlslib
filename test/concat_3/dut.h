#ifndef DUT_H__
#define DUT_H__


#include "util.h"

typedef int T; 

const int INPUT_WIDTH_1 = 1;
const int INPUT_WIDTH_2 = 2;
const int INPUT_WIDTH_3 = 3;

const int II  = 1;

void dut(hls::stream<array<T, INPUT_WIDTH_1>> &in1Stream,
         hls::stream<array<T, INPUT_WIDTH_2>> &in2Stream,
         hls::stream<array<T, INPUT_WIDTH_3>> &in3Stream,
         hls::stream<array<T, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3>> &outStream);


#endif