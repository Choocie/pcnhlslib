#ifndef DUT_H__
#define DUT_H__

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>

#include "ap_fixed.h"
#include "hls_math.h"
#include "util.h"


#include "gravnetconv.h"
#include "hls_stream.h"

const int N = 16;
const int F_C = 6;
const int PAR = 2;
const int II = 8;

typedef ap_fixed<16,1> input_t;
typedef ap_fixed<16,4> distance_t;
typedef ap_fixed<16,4> accum_t;

template<int N, int F, int DISTANCE_WIDTH, int DISTANCE_IWIDTH>
void distance_golden_model(float input[N*F], std::array<std::array<float,N>,N> &result, int num) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            if(j > i)
                continue;
            float accumulate = 0;
            for(int f = 0; f < F; f++)
                accumulate += abs(input[i*F+f] - input[j*F+f]);
            if(i < num && j < num) {
                result[i][j] = -accumulate;
                result[j][i] = -accumulate;
            } else {
                result[i][j] = -ap_fixed_max<DISTANCE_WIDTH,DISTANCE_IWIDTH>();
                result[j][i] = -ap_fixed_max<DISTANCE_WIDTH,DISTANCE_IWIDTH>();
            }

        }
    }
    //Self loop distances must be 0.
    for(int i = 0; i < num; i++)
        assert(result[i][i] == -0.);
}

void dut(input_t queryRow[N*F_C],
		 input_t pointRow[N*F_C],
		 hls::stream<array<distance_t,N>> distanceStream[PAR],
         hls::stream<int>                 &numStream);

#endif
