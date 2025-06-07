#include "dut.h"


void dut(hls::stream<array<input_t,F_IN>> 	     &inputFeatureStream,
		 hls::stream<array<output_t,F_P>> 		 &inputMaxStream,
		 hls::stream<array<output_t,F_P>> 		 &inputAvgStream,
		 hls::stream<array<output_t,F_IN+2*F_P>> &outputStream) {
    combine<input_t,
            output_t,
            F_P,
            II,
            F_IN>(inputFeatureStream,
                  inputMaxStream,
                  inputAvgStream,
                  outputStream);
}