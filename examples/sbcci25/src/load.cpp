#include "load.h"

void load(int  &numEvents,
          int* inputNumList,
          in_harness_t* inFeatureList,
          hls::stream<array<model_input_t,  MODEL_INPUT_WIDTH>> inputStream[PAR],
          hls::stream<int> &numStream) {
    #pragma HLS stable variable=numEvents
    #pragma HLS interface mode=s_axilite port=numEvents
    #pragma HLS INTERFACE mode=m_axi port=inputNumList offset=slave bundle=gmem0 depth=64 latency=64
    #pragma HLS INTERFACE mode=m_axi port=inFeatureList offset=slave bundle=gmem0 depth=64*II latency=64
    #pragma HLS INTERFACE mode=axis port=inputStream
    #pragma HLS INTERFACE mode=axis port=numStream
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return

    static int nums[MAX_EVENTS];

    assert(numEvents > 0);

    for(int e = 0; e < MAX_EVENTS;e++) {
        #pragma HLS pipeline II=1
        int n = inputNumList[e];
        nums[e] = n;
    }

    for(int i = 0; i < numEvents*II;i++) {
        #pragma HLS pipeline II=1
        int n = nums[i / II]; 
        assert(n > 0);
        if(i % II == 0) 
            numStream << n;
        in_harness_t burst = inFeatureList[i];
        for(int p = 0; p < PAR; p++) {
            if((i % II)*PAR+p < n) {
                array<model_input_t,  MODEL_INPUT_WIDTH> features;
                for(int f = 0; f < MODEL_INPUT_WIDTH; f++) {
                    features[f] = burst[p*MODEL_INPUT_WIDTH+f];
                }
                inputStream[p] << features;
            }
        }
    }
}