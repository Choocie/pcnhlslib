
#ifndef DUT_H_
#define DUT_H_

#include <array>
#include "hls_stream.h"
#include "util.h"
#include "gravnetconv.h"

using std::array;

static const int    N = 32;
static const int    K = 8; 
static const int    LUT_SIZE = 8;
static const int    II = 1;
static const int    PAR = N / II;

typedef ap_fixed<16,1>  input_t; 
typedef ap_ufixed<10,1> output_t;

void dut(hls::stream<array<input_t,K>>      &distancIn,
         hls::stream<array<output_t,K-1>>     &distanecExp);

#endif