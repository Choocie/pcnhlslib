#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main() 
{
    
    hls::stream<array<input_t,F>> inputStream;
    hls::stream<array<output_t,F>> outputStream;

    array<input_t,F> stimuli = {-2.0, 3.0, -1.0, 4.0};
    inputStream.write(stimuli); 

    array<output_t,F> golden = {0.0, 3.0, 0.0, 4.0}; 

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

    ap_uint<8> Result = 0;
    ap_fixed<16,1> valdi = -0.00390625;
    std::cout << valdi << std::endl;
    Result = valdi.range(15,7);
    std::cout << Result << std::endl;

    ap_uint<8> Result2 = 0;
    ap_fixed<16,1> valdi2 = -0.5;
    std::cout << valdi2 << std::endl;
    Result2 = valdi2.range(15,7);
    std::cout << Result2 << std::endl;


    return 0;
}
