#include "dut.h"

void dut(hls::stream<array<coordinate_t,TPOS_WIDTH>>    coordinateStream[PAR],
         hls::stream<array<beta_t,BETA_WIDTH>>	       inbetaStream[PAR],
         hls::stream<ap_uint<N>>       			&condensationPoints,
         beta_t                                         betaThreshold,
         distance_t                                     isolationThreshold) {
                #pragma HLS dataflow
                #pragma HLS stable variable=betaThreshold
                #pragma HLS interface port=betaThreshold mode=ap_vld
                #pragma HLS stable variable=isolationThreshold
                #pragma HLS interface port=isolationThreshold mode=ap_vld
                condensation_point_selection<
                     coordinate_t,
                     distance_t,
                     beta_t,
                     identifier_t,
                     BETA_WIDTH,
                     TPOS_WIDTH,
                     N,
                     PAR,
                     II>(coordinateStream,
                         inbetaStream,
                         condensationPoints,
                         betaThreshold,
                         isolationThreshold);
}

