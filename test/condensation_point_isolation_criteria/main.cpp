#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    bool fail = false; 

    distance_t distanceMatrixIn[N][N] = {{  0, 0.2, 0.5,   1,   4,   6}, 
                                         {0.2,   0, 0.4,  10,  13,   1},
                                         {0.5, 0.4,   0,   6,   7,   8},
                                         {  1,  10,   6,   0,   6,  11},
                                         {  4,  13,   7,   6,   0,   1},
                                         {  6,   1,   8,  11,   1,   0}};

    hls::stream<array<tuple_t<identifier_t>,N>> inputTuplesStream;
    std::array<tuple_t<identifier_t>, N> sortedTupleStimuli = {tuple_t<identifier_t>{1,0}, tuple_t<identifier_t>{1,1}, tuple_t<identifier_t>{1,2}, tuple_t<identifier_t>{1,3}, tuple_t<identifier_t>{1,4}, tuple_t<identifier_t>{0,5}};
    inputTuplesStream << sortedTupleStimuli;


    array<bool,N> golden = {1, 0, 1, 1, 1, 0};

    hls::stream<bool> outputStream;

    dut(distanceMatrixIn, inputTuplesStream, outputStream);

    
    array<bool,N> output;
    for (int i = 0; i < N; i++) {
        outputStream >> output[i];
        if(output[i] != golden[i]) {
            fail = true;
            std::cout << "Got " << output[i] << " but expected " << golden[i] << " at "<< i << std::endl;
        }
    }
    
    if(fail) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }

    return 0;
}