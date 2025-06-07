#ifndef BITONIC_SORT_H__
#define BITONIC_SORT_H__

#include "util.h"
#include "hls_stream.h"

using namespace std;

/**
 * @brief Sorts an array of length N of (key,value) tuples.
 * @param keysInStream Stream interface of length N containing the keys of the first array
 * @param valuesInStream Stream interface of length N containing the values of the first array
 * @param keysOutStream Stream interface of length N containing the sorted key array
 * @param valuesOutStream Stream interface of length N containing the sorted value array
 * @tparam D key type
 * @tparam P value type
 * @tparam N Length of incoming array
 * @tparam II Number of iterations
 * @details Latency depends on the pipeline depth. Throughput II is always one.
 */
template <class D, class P, int N,int II>
void bitonic_sort(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<D, N>> &keysOutStream,
                  hls::stream<array<P, N>> &valuesOutStream)
{
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1 rewind

        array<D, N> keys;
        array<P, N> values;

        keysInStream >> keys;
        valuesInStream >> values;

        for (int p = 1; p < N; p *= 2)
            for (int k = p; k > 0; k /= 2)
            #pragma HLS dependence variable = keys type = inter false
            #pragma HLS dependence variable = values type = inter false
                for (int j = (k % p); j <= N - 1 - k; j += 2 * k)
                    for (int i = 0; i <= std::min<int>(k - 1, N - j - k - 1); i++)
                        if ((i + j) / (2 * p) == (i + j + k) / (2 * p))
                            compare_and_swap<D, P>(keys[i + j + k], values[i + j + k], keys[i + j], values[i + j]);

        keysOutStream << keys;
        valuesOutStream << values;
    }
};

/**
 * @brief Sorts an array of length N of (key,value) tuples.
 * @param keysInStream Stream interface of length N containing the keys of the first array
 * @param valuesInStream Stream interface of length N containing the values of the first array
 * @param keysOutStream Stream interface of length N containing the sorted key array
 * @param valuesOutStream Stream interface of length N containing the sorted value array
 * @tparam D key type
 * @tparam P value type
 * @tparam N Length of incoming array
 * @details Latency depends on the pipeline depth. Throughput II is always one.
 */
template <class D, class P, int N>
void bitonic_sort(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<D, N>> &keysOutStream,
                  hls::stream<array<P, N>> &valuesOutStream)
{
    #pragma HLS pipeline II=1 style=frp

    array<D, N> keys;
    array<P, N> values;

    keysInStream >> keys;
    valuesInStream >> values;

    for (int p = 1; p < N; p *= 2)
        for (int k = p; k > 0; k /= 2)
        #pragma HLS dependence variable = keys type = inter false
        #pragma HLS dependence variable = values type = inter false
            for (int j = (k % p); j <= N - 1 - k; j += 2 * k)
                for (int i = 0; i <= std::min<int>(k - 1, N - j - k - 1); i++)
                    if ((i + j) / (2 * p) == (i + j + k) / (2 * p))
                        compare_and_swap<D, P>(keys[i + j + k], values[i + j + k], keys[i + j], values[i + j]);

    keysOutStream << keys;
    valuesOutStream << values;
};



/**
 * @brief Sorts an array of length N of (key,value) tuples.
 * @param keysInStream Stream interface of length N containing the keys of the first array
 * @param valuesInStream Stream interface of length N containing the values of the first array
 * @param valuesOutStream Stream interface of length N containing the sorted value array
 * @tparam D key type
 * @tparam P value type
 * @tparam N Length of incoming array
 * @tparam II Number of iterations
 * @details Latency depends on the pipeline depth. Throughput II is always one.
 */
template <class D, class P, int N, int II>
void bitonic_sort(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<P, N>> &valuesOutStream)
{
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II=1 rewind

        array<D, N> keys;
        array<P, N> values;

        keysInStream >> keys;
        valuesInStream >> values;

        for (int p = 1; p < N; p *= 2)
            for (int k = p; k > 0; k /= 2)
            #pragma HLS dependence variable = keys type = inter false
            #pragma HLS dependence variable = values type = inter false
                for (int j = (k % p); j <= N - 1 - k; j += 2 * k)
                    for (int i = 0; i <= std::min<int>(k - 1, N - j - k - 1); i++)
                        if ((i + j) / (2 * p) == (i + j + k) / (2 * p))
                            compare_and_swap<D, P>(keys[i + j + k], values[i + j + k], keys[i + j], values[i + j]);
                            
        valuesOutStream << values;
    }
};

/**
 * @brief Sorts an array of length N of (key,value) tuples.
 * @param keysInStream Stream interface of length N containing the keys of the first array
 * @param valuesInStream Stream interface of length N containing the values of the first array
 * @param valuesOutStream Stream interface of length N containing the sorted value array
 * @tparam D key type
 * @tparam P value type
 * @tparam N Length of incoming array
 * @tparam II Number of iterations
 * @details Latency depends on the pipeline depth. Throughput II is always one.
 */
template <class D, class P, int N>
void bitonic_sort(hls::stream<array<D, N>> &keysInStream,
                  hls::stream<array<P, N>> &valuesInStream,
                  hls::stream<array<P, N>> &valuesOutStream)
{
    #pragma HLS pipeline II=1 style=frp

    array<D, N> keys;
    array<P, N> values;

    keysInStream >> keys;
    valuesInStream >> values;

    for (int p = 1; p < N; p *= 2)
        for (int k = p; k > 0; k /= 2)
        #pragma HLS dependence variable = keys type = inter false
        #pragma HLS dependence variable = values type = inter false
            for (int j = (k % p); j <= N - 1 - k; j += 2 * k)
                for (int i = 0; i <= std::min<int>(k - 1, N - j - k - 1); i++)
                    if ((i + j) / (2 * p) == (i + j + k) / (2 * p))
                        compare_and_swap<D, P>(keys[i + j + k], values[i + j + k], keys[i + j], values[i + j]);
                        
    valuesOutStream << values;
};


#endif
