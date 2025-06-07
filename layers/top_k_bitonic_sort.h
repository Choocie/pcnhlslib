#ifndef __TOP_K_BITONIC_SORT_H__
#define __TOP_K_BITONIC_SORT_H__

#include "util.h"
#include "hls_stream.h"
#include "top_k_bitonic_merger.h"
#include "bitonic_sort.h"

using namespace std;

/**
 * @brief Sorts top K elements of an array of length N fully pipelined (II=1).
 * @tparam D Type of distance elements
 * @tparam P type of payload elements
 * @tparam N Number of elements in the array that is to be sorted
 * @tparam K Number of output elements
 * @tparam II Number of iterations
 */
template <typename D,
          typename P,
          int N,
          int K,
          int II>
void top_k_bitonic_sort(hls::stream<array<D, K>> keysInStream[N / K], hls::stream<array<P, K>> valuesInStream[N / K],
                        hls::stream<array<D, K>> &keysOutStream, hls::stream<array<P, K>> &valuesOutStream)
{
    #pragma HLS inline
    const int S = ceillog2(N/K);

    hls::stream<array<D, K>> keysStream[S][N / K];
    hls::stream<array<P, K>> valuesStream[S][N / K];

    for (int s = 0; s < S + 1; s++)
    {
        #pragma HLS unroll
        if (s == 0)
        {
            for (int n = 0; n < N / K; n++)
            {
                #pragma HLS unroll
                bitonic_sort<D, P, K, II>(keysInStream[n], valuesInStream[n], keysStream[s][n], valuesStream[s][n]);
            }
        }
        else if (s < S)
        {
            for (int n = 0; n < N / K / static_pow2<int>(s); n++)
            {
                #pragma HLS unroll
                top_k_bitonic_merge<D, P, 2 * K, K, II>(keysStream[s-1][2 * n], valuesStream[s-1][2 * n],
                                                    keysStream[s-1][2 * n + 1], valuesStream[s-1][2 * n + 1],
                                                    keysStream[s][n], valuesStream[s][n]);
            }
        }
        else
        {
            top_k_bitonic_merge<D, P, 2 * K, K, II>(keysStream[s-1][0], valuesStream[s-1][0],
                                                keysStream[s-1][1], valuesStream[s-1][1],
                                                keysOutStream, valuesOutStream);
        }
    }
}

#endif
