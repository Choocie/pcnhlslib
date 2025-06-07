#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<T> inputStream;
    T a = 25;
    for(int ii = 0; ii < II; ii++) {
        inputStream << a;
    }

    array<T,2> golden = {25, 25};

    hls::stream<T> outputStream[PAR];

    dut(inputStream, outputStream);


    for(int ii = 0; ii < II; ii++) {
        for (int i = 0; i < PAR; i++)
        {
            int out;
            outputStream[i] >> out;
            if(golden[i] != out) {
                std::cout << "TEST FAILED: Got " << out << " but expected " << golden[i] << std::endl;
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