#ifndef DUT_H__
#define DUT_H__

#include "cluster_selection.h"
#include "hls_stream.h"



const int N = 6;
const int PAR = 1;
const int II = 1;

typedef ap_fixed<16,5> distance_t;
typedef ap_uint<5> identifier_t;


void dut(distance_t 								 distanceIn[N][N],
	    hls::stream<array<tuple_t<identifier_t>, N>> &sortedTuplesStream,
       	hls::stream<bool> 						     &outCluster);

#endif
