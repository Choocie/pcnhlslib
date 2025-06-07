#include "store.h"

void store(int &numEvents,
           int* outputNumList,
           out_harness_t* outFeatureList,
           hls::stream<array<model_output_t,  MODEL_OUTPUT_WIDTH>> outputStream[PAR],
           hls::stream<bool> lastStream[PAR]) {
    #pragma HLS stable variable=numEvents
    #pragma HLS interface mode=s_axilite port=numEvents
    #pragma HLS INTERFACE mode=m_axi port=outputNumList offset=slave bundle=gmem1 depth=64 latency=64
    #pragma HLS INTERFACE mode=m_axi port=outFeatureList offset=slave bundle=gmem1 depth=64*II latency=64
    #pragma HLS INTERFACE mode=axis port=outputStream
    #pragma HLS INTERFACE mode=axis port=lastStream
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return

    assert(numEvents > 0);

    static int nums[MAX_EVENTS];

    for(int e = 0; e < MAX_EVENTS;e++) {
        #pragma HLS pipeline II=1
        int n = outputNumList[e];
        nums[e] = n;
    }

    for(int i = 0; i < numEvents*II;i++) {
        #pragma HLS pipeline II=1
        int n = nums[i / II];
        assert(n > 0);
        
        out_harness_t burst = {0};
        for(int p = 0; p < PAR; p++) {
            if((i % II)*PAR+p < n) {
                array<model_output_t, MODEL_OUTPUT_WIDTH> output;
                outputStream[p] >> output;
                for(int f = 0; f < MODEL_OUTPUT_WIDTH;f++) {
                    burst[p*MODEL_OUTPUT_WIDTH+f] = output[f];
                } 
            } else {
                for(int f = 0; f < MODEL_OUTPUT_WIDTH;f++) {
                    burst[p*MODEL_OUTPUT_WIDTH+f] = 0;
                }   
            }
            bool last;
            lastStream[p] >> last;
        }
        outFeatureList[i] = burst;
    }
}