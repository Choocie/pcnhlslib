/* Author: Valdrin Dajaku - Simplified Version of hls4ml latency Softmax function */
#ifndef SOFTMAX_H__
#define SOFTMAX_H__

#include <array>
#include <iostream>
#include "util.h"
#include "hls_stream.h"
#include <cmath>

using std::array;

template <typename input_t,
          typename output_t,
          int F,
          int II,
          int LUT_SIZE>
void softmax(hls::stream<array<input_t, F>> &inputStream,
             hls::stream<array<output_t, F>> &outputStream,
             array<output_t, static_pow2(LUT_SIZE)> &expLUT) {
	
    
    ap_uint<LUT_SIZE> index = 0;
    #pragma HLS array_partition variable=expLUT type=complete dim=1
    
    for (int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1
        array<input_t, F> input;
        inputStream >> input;


        // Calc Softmax-Values
        array<output_t, F> result;
        output_t exp_sum = 0;

        for (int i = 0; i < F; i++) {
            #pragma HLS UNROLL
            index = input[i].range(input_t::width - 1,input_t::width - LUT_SIZE);
            result[i] = expLUT[index]; //LUT
            exp_sum += result[i];//sum for division

        }
        // output values
        for (int i = 0; i < F; i++) {
            #pragma HLS UNROLL
            result[i] /= exp_sum;
        }
        outputStream << result;
    }
    
}

#endif
