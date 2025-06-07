#ifndef LOAD_H__
#define LOAD_H__

#include "util.h"
#include "hls_stream.h"
#include "global.h"

using namespace std;

void load(int  &numEvents,
          int* inputNumList,
          in_harness_t* inFeatureList,
          hls::stream<array<model_input_t,  MODEL_INPUT_WIDTH>> inputStream[PAR],
          hls::stream<int> &numStream);

#endif