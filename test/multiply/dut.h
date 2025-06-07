#ifndef DUT_H__
#define DUT_H__

#include "gravnetconv.h"
#include "util.h"

typedef ap_fixed<16,1> feature_t;
typedef ap_fixed<16,1> distance_t;
typedef ap_fixed<16,1> output_t;

const int K     = 4;
const int F_P   = 2;
const int II    = 1;

void dut(hls::stream<array<array<feature_t,F_P>,K>>  &featureStream,
         hls::stream<array<distance_t,K-1>>      	&distancesStream,
		 hls::stream<array<output_t,(K-1)*F_P>>    	&outputStream);

#endif