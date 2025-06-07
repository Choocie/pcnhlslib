#include "dut.h"

void dut(hls::stream<array<array<feature_t,F_P>,K>> &featureStream,
         hls::stream<array<distance_t,K-1>>      	&distancesStream,
		 hls::stream<array<output_t,(K-1)*F_P>>    	&outputStream) {
    multiply<feature_t,
             distance_t,
             output_t,
             K,
             F_P,
             II>(featureStream,
                 distancesStream,
                 outputStream);
}