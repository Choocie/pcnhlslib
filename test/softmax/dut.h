#ifndef DUT_H__
#define DUT_H__

#include "softmax.h"
#include "hls_stream.h"

const int F         = 6;
const int II        = 1;
const int LUT_SIZE  = 8;

typedef ap_fixed<16,4> input_t;
typedef ap_fixed<16,4> output_t;

void dut(hls::stream<array<input_t,F>> &inputStream,
         hls::stream<array<output_t,F>> &outputStream);

#endif
