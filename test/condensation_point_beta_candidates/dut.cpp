#include "dut.h"

void dut(hls::stream<array<beta_t,F_IN>>				inbetaStream[PAR],
		hls::stream<array<beta_t,N>>					&outbetaStream,
		hls::stream<array<tuple_t<identifier_t>,N>>	    &outtupelStream,
		beta_t 								betaThreshold){
		#pragma HLS dataflow
        #pragma HLS stable variable=betaThreshold
        #pragma HLS interface port=betaThreshold mode=ap_vld
        	condensation_point_beta_candidates<beta_t,identifier_t,F_IN,N,PAR,II> (inbetaStream, outbetaStream, outtupelStream, betaThreshold);
}

