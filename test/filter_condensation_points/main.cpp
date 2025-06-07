#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,F>> inputStream[PAR];

    for(int p = 0; p < PAR; p++) {
        for(int ii = 0; ii < II; ii++) {
            array<T,F> data =  {(ii + 1) * 8 + p, - (ii + 1) * 8 + p};
            inputStream[p] << data;
        }
    }

    ap_uint<N> condensationPoints = 0;
    condensationPoints.set(0); 
    condensationPoints.set(2); 
    hls::stream<ap_uint<N>> condensationPointsStream;
    condensationPointsStream << condensationPoints;
    
    hls::stream<int> numStream;
    numStream << 3;

    array<array<T,F>,3> dataGolden = {{{{8,8}},{{9,-7}},{{16,16}}}};
    array<bool,II> lastGolden = {false,false,false,false,false,false,false,true};

    hls::stream<array<T,F>>    outputStream[PAR];
    hls::stream<bool> lastStream[PAR];

    dut(inputStream, numStream, condensationPointsStream, outputStream, lastStream);

    for(int p = 0; p < PAR; p++) {
        array<T,F> output;
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