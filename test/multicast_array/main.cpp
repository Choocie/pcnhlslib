#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<T> inputStream[IN_PAR];
    for(int ii = 0; ii < II; ii++) {
        for(int i = 0; i < IN_PAR; i++) {
            inputStream[i] << (i+1) * 30;
        }
    }

    array<T,2> golden = {30, 60};

    hls::stream<T> outputStream[OUT_PAR][IN_PAR];

    dut(inputStream, outputStream);


    for(int ii = 0; ii < II; ii++) {
        for(int i = 0; i < IN_PAR; i++) {
            for (int j = 0; j < OUT_PAR; j++)
            {
                int out;
                outputStream[j][i] >> out;
                if(golden[i] != out) {
                    std::cout << "TEST FAILED: Got " << out << " but expected " << golden[i] << std::endl;
                    fail = true;
                } 
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