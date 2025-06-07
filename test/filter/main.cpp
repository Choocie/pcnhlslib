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
            T data =  {(ii + 1) * 8 + p, - (ii + 1) * 8 + p};
            inputStream[p] << data;
        }
    }

    hls::stream<int> numStream;
    numStream << 3;

    array<T,3> dataGolden = {{{{8,-8}},{{9,-7}},{{16,-16}}}};
    array<bool,II> lastGolden = {false,false,false,false,false,false,false,true};

    hls::stream<T>    outputStream[PAR];
    hls::stream<bool> lastStream[PAR];

    dut(inputStream, numStream, outputStream, lastStream);

    for(int p = 0; p < PAR; p++) {
        T output;
        for(int ii = 0; ii < II; ii++) {
            if( ii*PAR + p < 3) {
                outputStream[p] >> output;
                for (int i = 0; i < 2; i++) {
                    if(output[i] != dataGolden[ii*PAR + p][i]) {
                        fail = true;
                        std::cout << "Got " << output[i] << " but expected " << dataGolden[ii*PAR + p][i] << std::endl;
                    }               
                }
            }
        }
    }

    for(int p = 0; p < PAR; p++) {
        bool output;
        for(int ii = 0; ii < II; ii++) {
            lastStream[p] >> output;
            if(output != lastGolden[ii]) {
                fail = true;
                std::cout << "Got " << output << " but expected " << lastGolden[ii] << std::endl;
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