#ifndef __UTIL_H__
#define __UTIL_H__

#include <array>
#include "hls_math.h"
#include "hls_stream.h"

using std::array;

template <typename T>
static constexpr T ceildiv(T dividend, T divisor)
{
#pragma HLS INLINE
    return (dividend + divisor - 1) / divisor;
}

template <typename T>
static constexpr T roundup(T dividend, T divisor)
{
#pragma HLS INLINE
    return ceildiv(dividend, divisor) * divisor;
}

template <class key_t, class val_t>
void compare_and_swap(key_t &keyA, val_t &valA, key_t &keyB, val_t &valB)
{
    #pragma HLS inline
    if (keyA > keyB)
    {
        key_t t;
        t = keyA;
        keyA = keyB;
        keyB = t;
        val_t s;
        s = valA;
        valA = valB;
        valB = s;
    }
};

template <typename T>
static constexpr T static_pow2(T exponent)
{
    T result = 1;
    for (int i = 0; i < exponent; i++)
    {
        result *= 2;
    }
    return result;
}

template <int WIDTH,
          int IWIDTH>
static constexpr float ap_fixed_max() {
    float max = static_pow2<float>(static_cast<int>(IWIDTH - 1)) - 1.0 + (1.0 - 1.0 / static_cast<float>(static_pow2<int>(WIDTH - IWIDTH)));
    return max;
}

constexpr int ceillog2(int x) {
    int result = 0;
    while (x > 1) {
        x = (x + 1) / 2;
        ++result;
    }
    return result;
}

constexpr int floorlog2(int x) {
    int result = 0;
    while (x >= 2) {
        x /= 2;
        ++result;
    }
    return result;
}

template <typename T, int I>
void repeat(hls::stream<T> &in, hls::stream<T> &out)
{
    T data;
    for (int i = 0; i < I; i++)
    {
        #pragma HLS pipeline II = 1 rewind
        if (i == 0)
        {
            in >> data;
        }
        out << data;
    }
}

template <typename T, int II, int N>
void repeat(hls::stream<T> in[N], hls::stream<T> out[N]) {
    T data[N];
    for (int i = 0; i < II; i++) {
        #pragma HLS pipeline II = 1 rewind
        for(int n = 0; n < N; n++) {
            if (i == 0) {
                in[n] >> data[n];
            }
            out[n] << data[n];
        }
    }
}

template <typename T, int II, int N, int M>
void repeat(hls::stream<T> in[N][M], hls::stream<T> out[N][M]) {
    T data[N][M];
    for (int i = 0; i < II; i++) {
        #pragma HLS pipeline II = 1 rewind
        for(int n = 0; n < N; n++) {
            for(int m = 0; m < M; m++) {
                if (i == 0) {
                    in[n][m] >> data[n][m];
                }
                out[n][m] << data[n][m];
            }
        }
    }
}

template<typename TC>
auto exp2(TC const &c) -> decltype(c*c) {
    #pragma HLS inline
    decltype(c*c) const  res = c*c;
    #pragma HLS BIND_OP variable=res op=mul impl=dsp
    return  res;
}

template<typename T1,
         typename T2,
         typename T3,
         typename TO,
         int INPUT_WIDTH_1,
         int INPUT_WIDTH_2,
         int INPUT_WIDTH_3,
         int II>
void concat(hls::stream<array<T1, INPUT_WIDTH_1>> &in1Stream,
            hls::stream<array<T2, INPUT_WIDTH_2>> &in2Stream,
            hls::stream<array<T3, INPUT_WIDTH_3>> &in3Stream,
            hls::stream<array<TO, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3>> &outStream) {
    array<T1, INPUT_WIDTH_1> in1;
    array<T2, INPUT_WIDTH_2> in2;
    array<T3, INPUT_WIDTH_3> in3;
    array<TO, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3> out;

    #pragma HLS array_partition variable=in1 complete dim=1
    #pragma HLS array_partition variable=in2 complete dim=1 
    #pragma HLS array_partition variable=in3 complete dim=1
    #pragma HLS array_partition variable=out complete dim=1

    for (int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1 rewind

        in1Stream >> in1;
        in2Stream >> in2;
        in3Stream >> in3;

        for (int i = 0; i < INPUT_WIDTH_1; i++) {
            #pragma HLS UNROLL
            out[i] = static_cast<TO>(in1[i]);
        }

        for (int i = 0; i < INPUT_WIDTH_2; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + i] =  static_cast<TO>(in2[i]);
        }

        for (int i = 0; i < INPUT_WIDTH_3; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + INPUT_WIDTH_2 + i] =  static_cast<TO>(in3[i]);
        }

        outStream << out;
    }
}

template<typename T,
         int INPUT_WIDTH_1,
         int INPUT_WIDTH_2,
         int INPUT_WIDTH_3,
         int INPUT_WIDTH_4,
         int INPUT_WIDTH_5,
         int II>
void concat(hls::stream<array<T, INPUT_WIDTH_1>> &in1Stream,
            hls::stream<array<T, INPUT_WIDTH_2>>  &in2Stream,
            hls::stream<array<T, INPUT_WIDTH_3>>  &in3Stream,
            hls::stream<array<T, INPUT_WIDTH_4>>  &in4Stream,
            hls::stream<array<T, INPUT_WIDTH_5>>  &in5Stream,
            hls::stream<array<T, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5>> &outStream) {
    array<T, INPUT_WIDTH_1> in1;
    array<T, INPUT_WIDTH_2> in2;
    array<T, INPUT_WIDTH_3> in3;
    array<T, INPUT_WIDTH_4> in4;
    array<T, INPUT_WIDTH_5> in5;
    array<T, INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5> out;

    #pragma HLS array_partition variable=in1 complete dim=1
    #pragma HLS array_partition variable=in2 complete dim=1
    #pragma HLS array_partition variable=in3 complete dim=1
    #pragma HLS array_partition variable=in4 complete dim=1
    #pragma HLS array_partition variable=in5 complete dim=1
    #pragma HLS array_partition variable=out complete dim=1

    for (int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1 rewind

        in1Stream >> in1;
        in2Stream >> in2;
        in3Stream >> in3;
        in4Stream >> in4;  
        in5Stream >> in5;

        for (int i = 0; i < INPUT_WIDTH_1; i++) {
            #pragma HLS UNROLL
            out[i] = in1[i];
        }

        for (int i = 0; i < INPUT_WIDTH_2; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + i] = in2[i];
        }

        for (int i = 0; i < INPUT_WIDTH_3; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + INPUT_WIDTH_2 + i] = in3[i];
        }

        for (int i = 0; i < INPUT_WIDTH_4; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + INPUT_WIDTH_2 + INPUT_WIDTH_3 + i] = in4[i];
        }

        for (int i = 0; i < INPUT_WIDTH_5; i++) {
            #pragma HLS UNROLL
            out[INPUT_WIDTH_1 + INPUT_WIDTH_2 + INPUT_WIDTH_3 + INPUT_WIDTH_4 + i] = in5[i];
        }

        outStream << out;
    }
}

template <typename T, int N>
static constexpr std::array<T, N> init_array(const T value)
{
    #pragma HLS inline
    std::array<T, N> a{};
    for (auto i = 0; i < a.size(); i++)
        #pragma HLS unroll
        a[i] = value;
    return a;
}

template <typename T, int N>
static constexpr std::array<T, N> init_range(int offset,int step)
{
    #pragma HLS inline
    std::array<T, N> a{};
    for (auto i = 0; i < a.size(); i+=step)
        #pragma HLS unroll
        a[i] = static_cast<T>(i+offset);
    return a;
}

template<typename inputa_t,
         typename inputb_t,
         typename output_t,
         int II,
         int F>
void multiply(hls::stream<array<inputa_t,F>> &inputAStream,
              hls::stream<array<inputb_t,F>> &inputBStream,
              hls::stream<array<output_t,F>> &outputStream) {
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1
            array<inputa_t,F> a;
            inputAStream >> a;
            array<inputb_t,F> b;
            inputBStream >> b;
            array<output_t,F> result;
            for(int f= 0; f < F; f++) {
                #pragma HLS unroll
                auto c = a[f] * b[f];
                result[f] = static_cast<output_t>(c);
            }
            outputStream << result;
    }
}

template<typename inputa_t,
         typename inputb_t,
         typename output_t,
         int PAR,
         int II,
         int F>
void multiply(hls::stream<array<inputa_t,F>> inputAStream[PAR],
              hls::stream<array<inputb_t,F>> inputBStream[PAR],
              hls::stream<array<output_t,F>> outputStream[PAR]) {
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1
        for(int p = 0; p < PAR; p++) {
            #pragma HLS unroll
            array<inputa_t,F> a;
            inputAStream[p] >> a;
            array<inputb_t,F> b;
            inputBStream[p] >> b;
            array<output_t,F> result;
            for(int f= 0; f < F; f++) {
                #pragma HLS unroll
                auto c = a[f] * b[f];
                result[f] = static_cast<output_t>(c);
            }
            outputStream[p] << result;
        }
    }
}

template<typename T,
         int PAR,
         int II>
void filter(hls::stream<T> inputStream[PAR],
            hls::stream<int> &numStream,
            hls::stream<T> outputStream[PAR],
            hls::stream<bool> lastStream[PAR]) {
    #pragma HLS pipeline II=II

    int num;

    for(int ii = 0; ii < II; ii++) {
        if(ii == 0)
            numStream >> num;

        for(int p = 0; p < PAR; p++) {
            #pragma HLS unroll
                T data;
                inputStream[p] >> data;
                if(ii*PAR + p  <  num)
                    outputStream[p] << data;

                if(ii == II - 1)
                    lastStream[p] << true;
                else
                    lastStream[p] << false;
            }
	}
}

template<typename T,
         int PAR,
         int II>
void padding(hls::stream<T> inputStream[PAR],
             hls::stream<int> &numStream,
			 hls::stream<T> outputStream[PAR]) {
    int num;
    #pragma HLS pipeline II=II

    for(int ii = 0; ii < II; ii++) {
        if(ii == 0) {
            numStream >> num;
        }

        for(int p = 0; p < PAR; p++) {
            #pragma HLS unroll
            T data;                
            if(ii*PAR + p  <  num) {
                inputStream[p] >> data;
                outputStream[p] << data;
            } else {
                T zeros = {0};
				outputStream[p] << zeros;
            }
		}
	}
}

template<typename T,
         int PAR,
         int II,
         int F,
         int ENERGY_OFFSET>
void padding(hls::stream<array<T,F>> inputStream[PAR],
             hls::stream<int> &numStream,
			 hls::stream<array<T,F>> outputStream[PAR],
             hls::stream<array<T,1>> energyStream[PAR]) {
    int num;
    #pragma HLS pipeline II=II

    for(int ii = 0; ii < II; ii++) {
        if(ii == 0) {
            numStream >> num;
        }

        for(int p = 0; p < PAR; p++) {
            #pragma HLS unroll
            array<T,F> data;      
            array<T,1> energy;          
            if(ii*PAR + p  <  num) {
                inputStream[p] >> data;
                outputStream[p] << data;
                energy[0] = data[ENERGY_OFFSET];
                energyStream[p] << energy;
            } else {
                array<T,F> zeros = {0};
                array<T,1> zero = {0};
				outputStream[p] << zeros;
                energyStream[p] << zero;
            }
		}
	}
}

template<typename T,
         int N,
         int PAR,
         int II,
         int F,
         int BETA_OFFSET>
void filter(hls::stream<array<T,F>> inputStream[PAR],
            hls::stream<int> &numStream,
            hls::stream<ap_uint<N>> &condensationPointsStream,
            hls::stream<array<T,F>> outputStream[PAR],
            hls::stream<bool> lastStream[PAR]) {
    #pragma HLS pipeline II=II

    int num;
    ap_uint<N> condensationPoints;

    for(int ii = 0; ii < II; ii++) {

        if(ii == 0) {
            numStream >> num;
            condensationPointsStream >> condensationPoints;
        }

        for(int p = 0; p < PAR; p++) {
            #pragma HLS unroll
                array<T,F> data;
                inputStream[p] >> data;
                if(ii*PAR + p  <  num){
                    if (condensationPoints.test(ii*PAR + p)){
                        data[BETA_OFFSET] = -data[BETA_OFFSET];
                    }
                    outputStream[p] << data;
                }

                if(ii == II - 1)
                    lastStream[p] << true;
                else
                    lastStream[p] << false;
            }
	}
}


template <typename T, int N, int K, int II>
static void stream_splitter(hls::stream<array<T, N>> &in, hls::stream<array<T, K>> out[N / K])
{
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II = 1
        array<T, N> dataIn;
        in >> dataIn;

        array<T, K> dataOut[ceildiv<int>(N, K)];

        for (int n = 0; n < N; n++)
        {
            #pragma HLS unroll
            dataOut[n / K][n % K] = dataIn[n];
        }

        for (int i = 0; i < N / K; i++)
        {
            #pragma HLS unroll
            out[i] << dataOut[i];
        }
    }
}


#endif
