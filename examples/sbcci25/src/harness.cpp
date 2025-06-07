#include "harness.h"

void harness(int &numEvents,
             int &betaThreshold,
             int &isolationThreshold,
             int* inputNumList,
             in_harness_t* inFeatureList,
             int* outputNumList,
             out_harness_t* outFeatureList) {
    #pragma HLS stable variable=numEvents
    #pragma HLS INTERFACE mode=s_axilite port=numEvents
    #pragma HLS stable variable=betaThreshold
    #pragma HLS INTERFACE mode=s_axilite port=betaThreshold
    #pragma HLS stable variable=isolationThreshold
    #pragma HLS INTERFACE mode=s_axilite port=isolationThreshold
    #pragma HLS INTERFACE mode=m_axi port=inputNumList offset=slave bundle=gmem0 depth=64 latency=64
    #pragma HLS INTERFACE mode=m_axi port=inFeatureList offset=slave bundle=gmem0 depth=64*II latency=64
    #pragma HLS INTERFACE mode=m_axi port=outputNumList offset=slave bundle=gmem1 depth=64 latency=64
    #pragma HLS INTERFACE mode=m_axi port=outFeatureList offset=slave bundle=gmem1 depth=64*II latency=64
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return
    #pragma HLS dataflow

    hls::stream<array<model_input_t,  MODEL_INPUT_WIDTH>,4*II> inputStream[PAR];
    hls::stream<int,4*II> numStream;
    hls::stream<array<model_output_t,  MODEL_OUTPUT_WIDTH>,4*II> outputStream[PAR];
    hls::stream<bool,4*II> lastStream[PAR];
    
    load(numEvents,inputNumList,inFeatureList,inputStream,numStream);
    caloclusternet(numEvents,inputStream,outputStream, lastStream, numStream, betaThreshold, isolationThreshold);    
    store(numEvents,outputNumList,outFeatureList,outputStream,lastStream);
}