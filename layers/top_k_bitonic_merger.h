#ifndef __TOP_K_BITONIC_MERGE_H__
#define __TOP_K_BITONIC_MERGE_H__

#include "util.h"
#include "hls_stream.h"

using namespace std;

/**
 * @brief Merges two sorted arrays of (key,value) tuples into one sorted array.
 * @param keysAInStream Stream interface of length N/2 containing the keys of the first array
 * @param valuesAInStream Stream interface of length N/2 containing the values of the first array
 * @param keysBInStream  Stream interface of length N/2 containing the keys of the second array
 * @param valuesBInStream Stream interface of length N/2 containing the values of the second array
 * @param keysOutStream Stream interface of length K containing the top K keys of the merged array
 * @param valuesOutStream Stream interface of length K containing the top K values of the merged array
 * @tparam D key type
 * @tparam P value type
 * @tparam N Combined length of both incoming arrays
 * @tparam K Number of tuples to be calculated at the output interface
 * @tparam II Number of iterations
 * @details Latency depends on the pipeline depth. Throughput II is always one.
 */
template <typename D,
          typename P,
          int N,
          int K,
          int II>
void top_k_bitonic_merge(hls::stream<array<D, N / 2>> &keysAInStream, hls::stream<array<P, N / 2>> &valuesAInStream,
                         hls::stream<array<D, N / 2>> &keysBInStream, hls::stream<array<P, N / 2>> &valuesBInStream,
                         hls::stream<array<D, K>> &keysOutStream, hls::stream<array<P, K>> &valuesOutStream)
{
    for(int ii = 0; ii < II; ii++) {
        #pragma HLS pipeline II = 1 rewind
        array<D, N / 2> keysA, keysB;
        keysAInStream >> keysA;
        keysBInStream >> keysB;

        array<D, N> keys;

        for (int n = 0; n < N; n++)
        {
        #pragma HLS unroll
            if (n < N / 2)
                keys[n] = keysA[n];
            else
                keys[n] = keysB[n % (N / 2)];
        }

        array<P, N / 2> valuesA, valuesB;
        valuesAInStream >> valuesA;
        valuesBInStream >> valuesB;

        array<P, N> values;

        for (int n = 0; n < N; n++)
        {
            #pragma HLS unroll
            if (n < N / 2)
                values[n] = valuesA[n];
            else
                values[n] = valuesB[n % (N / 2)];
        }

        for (int p = N / 2; p < N; p *= 2)
            for (int k = p; k > 0; k /= 2)
                #pragma HLS dependence variable = keys type = inter false
                #pragma HLS dependence variable = values type = inter false
                for (int j = (k % p); j <= N - 1 - k; j += 2 * k)
                    for (int i = 0; i <= std::min<int>(k - 1, N - j - k - 1); i++)
                        if ((i + j) / (2 * p) == (i + j + k) / (2 * p))
                            compare_and_swap<D, P>(keys[i + j + k], values[i + j + k], keys[i + j], values[i + j]);

        array<D, K> keysOut;

        for (int k = 0; k < K; k++)
            keysOut[k] = keys[k];

        keysOutStream << keysOut;

        array<P, K> valuesOut;

        for (int k = 0; k < K; k++)
            valuesOut[k] = values[k];

        valuesOutStream << valuesOut;
    }
}

#endif
