#ifndef DUT_H__
#define DUT_H__


#include "gravnetconv.h"
#include "util.h"

const int F_IN  = 2;
const int F_P   = 3;
const int II    = 2;

typedef ap_fixed<8,1>  input_t; 
typedef ap_fixed<8,1>  output_t; 

void dut(hls::stream<array<input_t,F_IN>> 	     &inputFeatureStream,
		 hls::stream<array<output_t,F_P>> 		 &inputMaxStream,
		 hls::stream<array<output_t,F_P>> 		 &inputAvgStream,
		 hls::stream<array<output_t,F_IN+2*F_P>> &outputStream);
#endif