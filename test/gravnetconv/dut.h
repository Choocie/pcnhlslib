
#ifndef DUT_H_
#define DUT_H_

#include <array>
#include "hls_stream.h"
#include "util.h"
#include "gravnetconv.h"

using std::array;

static const int N = 32;

static const int K = 8; 
static const int F_IN = 16; 
static const int F_C = 8; 
static const int F_P = 8;
static const int F_OUT = 32; 

static const int II = 16;
static const int PAR = N / II;
static const int LUT_SIZE = 9;

typedef ap_fixed<16,9>  input_t; 
typedef ap_fixed<16,9>  coordinate_t;
typedef ap_fixed<16,9>  feature_t;
typedef ap_ufixed<10,1> exponential_t;
typedef ap_fixed<16,9>  distance_t; 
typedef ap_fixed<16,9> accum_t;
typedef ap_fixed<16,9>  output_t;

//typedef float input_t; 
//typedef float coordinate_t;
//typedef float feature_t;
//typedef float distance_t; 
//typedef float accum_t;
//typedef float output_t;


void dut(hls::stream<array<coordinate_t,F_C>>       coordinateStream[PAR],
         hls::stream<array<feature_t,F_P>>          featureStream[PAR],
		 hls::stream<array<input_t,F_IN>>           inputStream[PAR],
         hls::stream<array<output_t,F_IN+2*F_P>>    outputStream[PAR],
         hls::stream<int>                           &numStream);

#endif