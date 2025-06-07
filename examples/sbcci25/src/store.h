#ifndef STORE_H__
#define STORE_H__

#include "util.h"
#include "hls_stream.h"
#include "global.h"

using namespace std;

void store(int &numEvents,
           int* outputNumList,
           out_harness_t* outFeatureList,
           hls::stream<array<model_output_t,  MODEL_OUTPUT_WIDTH>> outputStream[PAR],
           hls::stream<bool> lastStream[PAR]);

#endif