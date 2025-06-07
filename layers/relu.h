/* Author: Valdrin Dajaku - Simplified version of hls4ml latency ReLU function. */
#ifndef RELU_H__
#define RELU_H__

#include <array>
#include "util.h"
#include "hls_stream.h"

using std::array;

template <typename input_t,
          typename output_t,
          int F,
          int II>
void relu(hls::stream<array<input_t,F>> &inputStream,
          hls::stream<array<output_t,F>> &outputStream) {


    // Reading Inputs out of Input Stream
    for (int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1
        array<input_t,F> input;
        inputStream >> input; 

        // Array for Output
        array<output_t,F> result;  

         // ReLU-Calculation
        for (int i = 0; i < F; i++) {
             #pragma HLS UNROLL
             result[i] = (input[i] > 0) ? input[i] : static_cast<output_t>(0);

            //result[i] = (input[i] > 0) ? input[i] : 0; 
        }

        // Writing results in Output Stream
        outputStream << result;  
    }
}

#endif
