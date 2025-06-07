#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    bool fail = false;
    int repeat = 5;

    hls::stream<std::array<coordinate_t,TPOS_WIDTH>> inputcoordinateStream[PAR];
    std::array<coordinate_t, TPOS_WIDTH> coordinates;
    std::array<coordinate_t,TPOS_WIDTH * N> coordinateStimuli = {1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3,
                                                                 1, 2, 3, 1, 2, 3.5, 4, 5, 6, 5,    5, 5};
    //std::array<coordinate_t,TPOS_WIDTH * N> coordinateStimuli =   {1, 2, 3, 1, 2, 3.5, 4, 5, 6, 1, 2.25, 3};


    hls::stream<array<beta_t,BETA_WIDTH>> inputbetaStream[PAR];
    std::array<beta_t, BETA_WIDTH> beta;
    std::array<beta_t,N> betaStimuli = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                                        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                                        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                                        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    //std::array<beta_t,N> betaStimuli = {0.5, 0.6, 0.8, 0.2};

    array<bool,N> golden = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    //array<bool,N> golden = {1, 1, 1, 0};
    hls::stream<ap_uint<N>> outputStream;
    beta_t betaThreshold = 0.1;
    distance_t isolationThreshold = 0.3;

    for(int r = 0; r < repeat; r++) {

        for (int ii = 0; ii<II; ii++){
            for (int p = 0; p < PAR; p++){
                for (int i = 0; i < TPOS_WIDTH; i++){
                    coordinates[i] = coordinateStimuli[(ii * PAR + p) * TPOS_WIDTH + i];
                }
                inputcoordinateStream[p] << coordinates;
            }
        }


        for (int ii = 0; ii<II; ii++){
            for (int p = 0; p < PAR; p++){
                beta[0] = betaStimuli[p*II+ii];
                inputbetaStream[p] << beta;
            }
        }

        dut(inputcoordinateStream,inputbetaStream,outputStream,betaThreshold,isolationThreshold);

        ap_uint<N> output;
        outputStream >> output;
        for (int i = 0; i < N; i++) {
            if(output.test(i) != golden[i]) {
                fail = true;
                std::cout << "Got " << output.test(i) << " but expected " << golden[i] << " at "<< i << std::endl;
            }
        }
    
    
        if(fail) {
            std::cout << "FAIL" << std::endl;
        } else {
            std::cout << "SUCCESS" << std::endl;
        }

    }

    return 0;
}