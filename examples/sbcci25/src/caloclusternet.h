#ifndef CALOCLUSTERNET_H__
#define CALOCLUSTERNET_H__

#include "caloclusternetv2.h"
#include "global.h"
#include "weights.h"

void caloclusternet(int &numEvents,
                    hls::stream<array<model_input_t, MODEL_INPUT_WIDTH>>  inputStream[PAR],
                    hls::stream<array<model_output_t, MODEL_OUTPUT_WIDTH>> outputStream[PAR],
                    hls::stream<bool> lastStream[PAR],
                    hls::stream<int> &numStream,
                    int &betaThreshold,
                    int &isolationThreshold);

#endif