#ifndef DUT_H__
#define DUT_H__

#include "condensation_point_selection.h"
#include "hls_stream.h"



const int N = 4;
const int PAR = 2;
const int II = 2;
const int F_IN = 1;

typedef ap_fixed<16,4> beta_t;
typedef ap_uint<5> identifier_t;


void dut(hls::stream<array<beta_t,F_IN>>				inbetaStream[PAR],
		hls::stream<array<beta_t,N>>					&outbetaStream,
		hls::stream<array<tuple_t<identifier_t>,N>>	&outtupelStream,
		beta_t		betaThreshold);

#endif
