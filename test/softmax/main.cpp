#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main() 
{

    hls::stream<array<input_t,F>> inputStream;
    hls::stream<array<output_t,F>> outputStream;

    array<input_t,F> stimuli = {1, 0, -0.5, 0.5, -1, 0.0625};
    inputStream.write(stimuli);  

    array<output_t,F> golden = {0.3125, 0.125, 0.0625, 0.1875, 0, 0.125};

    array<output_t,F> output;

    dut(inputStream, outputStream);

    outputStream.read(output);

    std::cout << "Output Array: " << std::endl;
    bool test_passed = true;
    for (int i = 0; i < F; i++) {
        std::cout << "Got: " << output[i] << " Expected: " << golden[i] << std::endl;
        if (output[i] != golden[i]) {
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
