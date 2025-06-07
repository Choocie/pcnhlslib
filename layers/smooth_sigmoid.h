/* Author: Valdrin Dajaku - Simplified version of smooth sigmoid*/
#ifndef SMOOTH_SIGMOID_H__
#define SMOOTH_SIGMOID_H__

#include <array>
#include "util.h"
#include "hls_stream.h"

using std::array;

/**
 * @brief 
 * 
 * @tparam input_t 
 * @tparam output_t 
 * @tparam F 
 * @tparam II 
 * @param inputStream 
 * @param outputStream 
 *@details Coefficients from smooth sigmoid taken from QKeras smooth sigmoid function    
 */
template <typename input_t,
          typename output_t,
          int F,
          int II>
void smooth_sigmoid(hls::stream<array<input_t,F>> &inputStream,
          hls::stream<array<output_t,F>> &outputStream) {

    const output_t coeff  = static_cast<output_t>(0.1875);
    const output_t offset = static_cast<output_t>(0.5);
    const output_t max    = static_cast<output_t>(1.);
    const output_t min    = static_cast<output_t>(0.);

    // Reading Inputs out of Input Stream
    for (int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1 rewind
        array<input_t,F> input;
        inputStream >> input; 
        array<output_t,F> output;  
        for (int i = 0; i < F; i++) {
             #pragma HLS UNROLL
            output_t result = (input[i] * coeff + offset);
            if (result >= max) {
                result = max;
            } else if (result <= min) {
                result = min;
            }
            output[i] = result;
        }
        outputStream << output;  
    }
}

#endif
