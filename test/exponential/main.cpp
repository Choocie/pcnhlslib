/**
Description: Top-level test file for standalone exponential test suite.
Author: Valrin Dajaku
Date: 04.11.2024
*/
#include <iostream>
#include <array>
#include "hls_stream.h"
#include "dut.h"

using std::array;

float reference_model(float x) {
    if(x > 0)
        return 1;
    else if(x <= -0.25)
        return 0;
    else
        return exp(10*x);
}

int main()
{

    hls::stream<array<input_t,K>> distanceIn;
    hls::stream<array<output_t,K-1>> outputStream;

    array<input_t,K> inputStimuli = {-0.00390625 , -0.125, -0.2475, -0.75, -0.0134, 0.125, 0};
    distanceIn.write(inputStimuli);

    array<output_t,K-1> output;

    dut(distanceIn,outputStream);

    outputStream >> output;

    std::cout << "Output Array: " << std::endl;
    bool test_passed = true;
    for (int i = 0; i < K-1; i++) {
        float test = static_cast<float>( output[i]);
        float reference = reference_model(inputStimuli[i]);
        float error = abs(test - reference);
        std::cout << "Got: " << test << " Expected: " << reference << " Abs. Error " << error << std::endl;
        if (error > 1e-2) {
            test_passed = false;
        }
    }

    if (test_passed) {
        std::cout << "Test PASSED!" << std::endl;
    } else {
        std::cout << "Test FAILED!" << std::endl;
    }
    
    return 0;
}