#include <iostream>
#include "hls_stream.h"
#include "dut.h"

using std::array;

int main()
{
    bool fail = false;

    hls::stream<array<T,INPUT_WIDTH_1>> aInputStream;
    array<T,INPUT_WIDTH_1> a = { 5 };
    for(int ii = 0; ii < II; ii++) {
        aInputStream << a;
    }

    hls::stream<array<T,INPUT_WIDTH_2>> bInputStream;
    array<T,INPUT_WIDTH_2> b = { -2 , -10};
    for(int ii = 0; ii < II; ii++) {
        bInputStream << b;
    }

    hls::stream<array<T,INPUT_WIDTH_3>> cInputStream;
    array<T,INPUT_WIDTH_3> c = {0,1,2};
    for(int ii = 0; ii < II; ii++) {
        cInputStream << c;
    }

    hls::stream<array<T,INPUT_WIDTH_4>> dInputStream;
    array<T,INPUT_WIDTH_4> d = {-1, -2};
    for(int ii = 0; ii < II; ii++) {
        dInputStream << d;
    }

    hls::stream<array<T,INPUT_WIDTH_5>> eInputStream;
    array<T,INPUT_WIDTH_5> e = {0};
    for(int ii = 0; ii < II; ii++) {
        eInputStream << e;
    }


    array<T,INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5> golden = {5, -2, -10, 0, 1, 2,-1,-2,0};

    hls::stream<array<T,INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5>> outputStream;

    dut(aInputStream, bInputStream, cInputStream, dInputStream, eInputStream, outputStream);

    for(int ii = 0; ii < II; ii++) {
        {
            array<T,INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5> output;
            outputStream >> output;
            for(int i = 0; i < INPUT_WIDTH_1+INPUT_WIDTH_2+INPUT_WIDTH_3+INPUT_WIDTH_4+INPUT_WIDTH_5; i++) {
                if(golden[i] != output[i]) {
                    std::cout << "TEST FAILED: Got " << output[i] << " but expected " << golden[i] << std::endl;
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