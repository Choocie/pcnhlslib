#ifndef DUT_H__
#define DUT_H__

#include "relu.h"
#include "hls_stream.h"


const int F = 4;
const int II    = 1;

typedef ap_fixed<8,4> input_t;
typedef ap_fixed<8,4> output_t;

void dut(hls::stream<array<input_t,F>> &inputStream,
         hls::stream<array<output_t,F>> &outputStream);

#endif
