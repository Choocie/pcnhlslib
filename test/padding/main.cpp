#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<T> inputStream[PAR];

    for(int p = 0; p < PAR; p++) {
        for(int ii = 0; ii < II; ii++) {
            if(ii*PAR+p < 3) {
                T data =  {(ii + 1) * 8 + p, - (ii + 1) * 8 + p};
                inputStream[p] << data;
            }
        }
    }

    hls::stream<int> numStream;
    numStream << 3;

    array<T,16> golden = {{{{8,-8}},{{9,-7}},{{16,-16}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}}}};

    hls::stream<T>    outputStream[PAR];

    dut(inputStream, numStream, outputStream);

    for(int p = 0; p < PAR; p++) {
        T output;
        for(int ii = 0; ii < II; ii++) {
            outputStream[p] >> output;
            for (int i = 0; i < 2; i++) {
                if(output[i] != golden[ii*PAR + p][i]) {
                    fail = true;
                    std::cout << "Got " << output[i] << " but expected " << golden[ii*PAR + p][i] << std::endl;
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