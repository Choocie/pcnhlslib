#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,F_C>> inputStream[PAR];
    for(int ii = 0; ii < II; ii++) {
        for(int p = 0; p < PAR; p++) {
            array<T,F_C> a = {(ii + 1) * 0.125  + p * -0.5};
            inputStream[p] << a;
        }
    }

    T golden[N * F_C] = {0.125, -0.375, 0.250, -0.250, 0.375, -0.125, 0.500, 0.0};

    T queryOutput[N * F_C];
    T pointsOutput[N * F_C];
    
    dut(inputStream, queryOutput, pointsOutput);

    for(int ii = 0; ii < II; ii++) {
        for (int i = 0; i < N * F_C; i++) {
            if(golden[i] != queryOutput[i]) {
                std::cout << "TEST FAILED: Got " << queryOutput[i] << " but expected " << golden[i] << std::endl;
                fail = true;
            } 
            if(golden[i] != pointsOutput[i]) {
                std::cout << "TEST FAILED: Got " << pointsOutput[i] << " but expected " << golden[i] << std::endl;
                fail = true;
            } 
        }
    }

    if(fail) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }

    return 0;
}