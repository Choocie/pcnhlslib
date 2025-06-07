/* Author: Marc Neu - Simplified version of hls4ml latency dense layer. */
#ifndef DENSE_H__
#define DENSE_H__

#include <array>
#include "util.h"
#include "hls_stream.h"

using std::array;

template <typename input_t,
          typename output_t,
          typename weight_t,
          typename bias_t,
          typename accum_t,
          int F_IN,
          int F_OUT,
          int II>
void dense(hls::stream<array<input_t,F_IN>> &inputStream,
           hls::stream<array<output_t,F_OUT>> &outputStream,
           weight_t weights[F_IN * F_OUT],
           bias_t biases[F_OUT]) {
    #pragma HLS ARRAY_PARTITION variable=biases complete
    accum_t mult[F_IN * F_OUT];
    #pragma HLS ARRAY_PARTITION variable=mult complete
    accum_t acc[F_OUT];
    #pragma HLS ARRAY_PARTITION variable=acc complete

    for(int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1 rewind

        array<input_t,F_IN> input;
        inputStream >> input;

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                mult[index] = input[ii] * weights[index];
            }
        }

        for (int iacc = 0; iacc < F_OUT; iacc++) {
            acc[iacc] = static_cast<accum_t>(biases[iacc]);
        }

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                acc[jj] += mult[index];
            }
        }

        array<output_t,F_OUT> result;

       for (int i = 0; i < F_OUT; i++) {
            result[i] = static_cast<output_t>(acc[i]);
        }

        outputStream << result;
    }
}

template <typename input_t,
          typename output_t,
          typename weight_t,
          typename bias_t,
          typename accum_t,
          int F_IN,
          int F_OUT,
          int II>
void dense_saturate(hls::stream<array<input_t,F_IN>> &inputStream,
           hls::stream<array<output_t,F_OUT>> &outputStream,
           weight_t weights[F_IN * F_OUT],
           bias_t biases[F_OUT]) {
    #pragma HLS ARRAY_PARTITION variable=biases complete
    accum_t mult[F_IN * F_OUT];
    #pragma HLS ARRAY_PARTITION variable=mult complete
    accum_t acc[F_OUT];
    #pragma HLS ARRAY_PARTITION variable=acc complete

    for(int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1 rewind

        array<input_t,F_IN> input;
        inputStream >> input;

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                mult[index] = input[ii] * weights[index];
            }
        }

        for (int iacc = 0; iacc < F_OUT; iacc++) {
            acc[iacc] = static_cast<accum_t>(biases[iacc]);
        }

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                acc[jj] += mult[index];
            }
        }

        array<output_t,F_OUT> result;

         for (int i = 0; i < F_OUT; i++) {
            if(acc[i] >= static_cast<accum_t>(ap_fixed_max<output_t::width,output_t::iwidth>()))
                result[i] = static_cast<output_t>(ap_fixed_max<output_t::width,output_t::iwidth>());
            else if(acc[i] <= static_cast<accum_t>(-ap_fixed_max<output_t::width,output_t::iwidth>()))
                result[i] = static_cast<output_t>(-ap_fixed_max<output_t::width,output_t::iwidth>());
            else
                result[i] = static_cast<output_t>(acc[i]);
        }


        outputStream << result;
    }
}


template <typename input_t,
          typename output_t,
          typename weight_t,
          typename bias_t,
          typename accum_t,
          int F_IN,
          int F_OUT,
          int II>
void dense_relu(hls::stream<array<input_t,F_IN>> &inputStream,
                hls::stream<array<output_t,F_OUT>> &outputStream,
                weight_t weights[F_IN * F_OUT],
                bias_t biases[F_OUT]) {
    #pragma HLS ARRAY_PARTITION variable=biases complete
    accum_t mult[F_IN * F_OUT];
    #pragma HLS ARRAY_PARTITION variable=mult complete
    accum_t acc[F_OUT];
    #pragma HLS ARRAY_PARTITION variable=acc complete

    for(int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1 rewind

        array<input_t,F_IN> input;
        inputStream >> input;

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                mult[index] = input[ii] * weights[index];
            }
        }

        for (int iacc = 0; iacc < F_OUT; iacc++) {
            acc[iacc] = static_cast<accum_t>(biases[iacc]);
        }

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                acc[jj] += mult[index];
            }
        }

        array<output_t,F_OUT> result;

        for (int i = 0; i < F_OUT; i++) {
            result[i] = (acc[i] > 0) ? static_cast<output_t>(acc[i]) : static_cast<output_t>(0.0);
        }

        outputStream << result;
    }
}

template <typename input_t,
          typename output_t,
          typename weight_t,
          typename bias_t,
          typename accum_t,
          int F_IN,
          int F_OUT,
          int II>
void dense_relu_saturate(hls::stream<array<input_t,F_IN>> &inputStream,
                         hls::stream<array<output_t,F_OUT>> &outputStream,
                         weight_t weights[F_IN * F_OUT],
                         bias_t biases[F_OUT]) {
    #pragma HLS ARRAY_PARTITION variable=biases complete
    accum_t mult[F_IN * F_OUT];
    #pragma HLS ARRAY_PARTITION variable=mult complete
    accum_t acc[F_OUT];
    #pragma HLS ARRAY_PARTITION variable=acc complete

    for(int ii = 0; ii < II; ii++) {
        #pragma HLS PIPELINE II=1 rewind

        array<input_t,F_IN> input;
        inputStream >> input;

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                mult[index] = input[ii] * weights[index];
            }
        }

        for (int iacc = 0; iacc < F_OUT; iacc++) {
            acc[iacc] = static_cast<accum_t>(biases[iacc]);
         }

        for (int ii = 0; ii < F_IN; ii++) {
            for (int jj = 0; jj < F_OUT; jj++) {
                int index = ii * F_OUT + jj;
                acc[jj] += mult[index];
            }
        }

        array<output_t,F_OUT> result;

        for (int i = 0; i < F_OUT; i++) {
            if(acc[i] >= static_cast<accum_t>(ap_fixed_max<output_t::width,output_t::iwidth>()))
                result[i] = static_cast<output_t>(ap_fixed_max<output_t::width,output_t::iwidth>());
            else if(acc[i] > 0)
                result[i] = static_cast<output_t>(acc[i]);
            else
                result[i] = static_cast<output_t>(0.0);        
            }

        outputStream << result;
    }
}

#endif