#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,WIDTH>> aIputStream;
    array<T,WIDTH> a = { 1, 5};
    aIputStream << a;

    hls::stream<array<T,WIDTH>> bIputStream;
    array<T,WIDTH> b = { -5, 0};
    bIputStream << b;

    array<T,2*WIDTH> golden = { 1, 5, -5, 0};

    array<T,2*WIDTH> output;
    hls::stream<array<T,2*WIDTH>> outputStream;

    dut(aIputStream, bIputStream, outputStream);

    outputStream >> output;

    for (int i = 0; i < 2*WIDTH; i++)
    {
        if(golden[i] != output[i]) {
            fail = true;
            std::cout << "TEST FAILED: Got " << output[i] << " but expected " << golden[i] << std::endl;
        }
    }

    if(fail) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }

    return 0;
}