#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<input_t,F_IN>> aInputStream;
    array<input_t,F_IN> a = { 0.75, 0.5};
    for(int ii = 0; ii < II; ii++) {
        aInputStream << a;
    }

    hls::stream<array<input_t,F_P>> bInputStream;
    array<input_t,F_P> b = { 0.125, 0.250, 0.375};
    for(int ii = 0; ii < II; ii++) {
        bInputStream << b;
    }

    hls::stream<array<input_t,F_P>> cInputStream;
    array<input_t,F_P> c = { -0.125, -0.250, -0.375};
    for(int ii = 0; ii < II; ii++) {
        cInputStream << c;
    }

    array<output_t,F_IN+2*F_P> golden = {0.75, 0.5, 0.125, 0.250, 0.375, -0.125, -0.250, -0.375};

    hls::stream<array<output_t,F_IN+2*F_P>> outputStream;

    dut(aInputStream, bInputStream, cInputStream, outputStream);


    for(int ii = 0; ii < II; ii++) {
        array<output_t,F_IN+2*F_P> output;
        outputStream >> output;
        for (int i = 0; i < F_IN+2*F_P; i++)
        {
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