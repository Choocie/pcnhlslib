#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,(K-1)*F_P>> inputStream;
    array<T,(K-1)*F_P> a = {0.75, 0.5, -0.125, -0.250, -0.375, +0.125};
    for(int ii = 0; ii < II; ii++) {
        inputStream << a;
    }

    array<T,F_P> aGolden = {0.75, 0.5};
    array<T,F_P> bGolden = {0.078125, 0.117188};


    hls::stream<array<T,F_P>> aOutputStream;
    hls::stream<array<T,F_P>> bOutputStream;

    dut(inputStream, aOutputStream, bOutputStream);


    for(int ii = 0; ii < II; ii++) {
        array<T,F_P> aOutput;
        aOutputStream >> aOutput;
        for (int i = 0; i < F_P; i++) {
            if(aGolden[i] != aOutput[i]) {
                std::cout << "TEST FAILED: Got " << aOutput[i] << " but expected " << aOutput[i] << std::endl;
                fail = true;
            } 
        }
        array<T,F_P> bOutput;
        bOutputStream >> bOutput;
        for (int i = 0; i < F_P; i++){
            if(bGolden[i] != bOutput[i]) {
                std::cout << "TEST FAILED: Got " << bOutput[i] << " but expected " << bGolden[i] << std::endl;
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