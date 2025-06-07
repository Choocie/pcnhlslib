#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    bool fail = false; 

    hls::stream<array<beta_t,F_IN>> inputbetaStream[PAR];
    std::array<beta_t, F_IN> beta;
    std::array<beta_t,N> betaStimuli = {0.05, 0.15, 0.08, 0.2};
    for (int ii = 0; ii<II; ii++){
        for (int p = 0; p < PAR; p++){
            beta[0] = betaStimuli[ii*PAR+p];
            inputbetaStream[p] << beta;
        }
    }
    
    std::array<tuple_t<identifier_t>,N> golden = {tuple_t<identifier_t>{0,0}, tuple_t<identifier_t>{1,1}, tuple_t<identifier_t>{0,2}, tuple_t<identifier_t>{1,3}};


    hls::stream<array<tuple_t<identifier_t>,N>> outputFilterStream;
    hls::stream<array<beta_t,N>> outputbetaStream;
    beta_t betaThreshold = 0.1;

    dut(inputbetaStream, outputbetaStream, outputFilterStream, betaThreshold);
    
    std::array<tuple_t<identifier_t>,N> outputFilter;
    outputFilterStream >> outputFilter;
    std::array<beta_t,N> outputBetas;
    outputbetaStream >> outputBetas;

    for (int i = 0; i < N; i++) {
        if(outputFilter[i].isCluster != golden[i].isCluster) {
            fail = true;
            std::cout << "Got " << outputFilter[i].isCluster << " but expected " << golden[i].isCluster << " at "<< i << std::endl;
        }
    }
    
    if(fail) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }

    return 0;
}