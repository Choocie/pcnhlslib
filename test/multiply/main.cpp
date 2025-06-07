#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<array<feature_t,F_P>,K>>  featureStream;
    array<array<feature_t,F_P>,K> f = {{{{0.125,0.0}},{{-0.5,0.875}},{{0.0,0.0}},{{-0.875,0.125}}}};
    for(int ii = 0; ii < II; ii++) {
        featureStream << f;
    }

    hls::stream<array<output_t,K-1>> distanceStream;
    array<output_t,K-1> d = {0.5, 1.0, 0.25};
    for(int ii = 0; ii < II; ii++) {
        distanceStream << d;
    }

    array<output_t,(K-1)*F_P> golden = {-0.25,0.4375,0.0,0.0,-0.21875,0.03125};

    hls::stream<array<output_t,(K-1)*F_P>> outputStream;

    dut(featureStream, distanceStream, outputStream);

    for(int ii = 0; ii < II; ii++) {
        array<output_t,(K-1)*F_P> output;
        outputStream >> output;
        for (int i = 0; i < (K-1)*F_P; i++) {
            if(golden[i] != output[i]) {
                std::cout << "TEST FAILED: Got " << output[i] << " but expected " << golden[i] << std::endl;
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