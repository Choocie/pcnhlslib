#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,F_P>> inputStream[PAR];
    for(int ii = 0; ii < II; ii++) {
        for(int p = 0; p < PAR; p++) {
            array<T,F_P> a = {(ii + 1) * 0.125  + p * -0.5};
            inputStream[p] << a;
        }
    }

    T golden[N][F_P] = {{0.125},{-0.375},{0.250},{-0.250},{0.375},{-0.125},{0.500},{0.0}};

    hls::stream<array<array<T,F_P>,N>> outputStream[PAR];

    dut(inputStream, outputStream);

    for(int p = 0; p < PAR; p++) {
        array<array<T,F_P>,N> output;
        outputStream[p] >> output;
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < F_P; j++) {
                if(golden[i][j] != output[i][j]) {
                    std::cout << "TEST FAILED: Got " << output[i][j] << " but expected " << golden[i][j]  << std::endl;
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