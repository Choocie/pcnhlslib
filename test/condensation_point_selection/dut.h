#ifndef DUT_H__
#define DUT_H__

#include "condensation_point_selection.h"
#include "hls_stream.h"



const int N = 32;
const int PAR = 2;
const int II = N/PAR;
const int BETA_WIDTH = 1;
const int TPOS_WIDTH = 3;

typedef ap_fixed<16,5> coordinate_t;
typedef ap_fixed<16,5> distance_t;
typedef ap_fixed<16,5> accum_t;
typedef ap_fixed<16,4> beta_t;
typedef ap_uint<5> identifier_t;


void dut(hls::stream<array<coordinate_t,TPOS_WIDTH>>    coordinateStream[PAR],
         hls::stream<array<beta_t,BETA_WIDTH>>          inbetaStream[PAR],
         hls::stream<ap_uint<N>>       				    &condensationPoints,
         beta_t                                         betaThreshold,
         distance_t                                     isolationThreshold);

#endif
