#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    hls::stream<std::array<T,N/2>> distancesAInStream,distancesBInStream;
    hls::stream<std::array<T,N/2>> payloadAInStream,payloadBInStream;

    std::array<T,N/2> distancesAIn = {0.5, 0, -0.25, -0.9875};
    std::array<T,N/2> distancesBIn = {0.75, 0.625, 0.0625, -0.75};
    std::array<T,K> distanceReference = {0.75, 0.625, 0.5, 0.0625};


    distancesAInStream << distancesAIn;
    distancesBInStream << distancesBIn;

    std::array<T,N/2> payloadAIn = {0.75, 0.975, 0.25, 0.375};
    std::array<T,N/2> payloadBIn ={0.5, -0.5, 0.875, 0};
    std::array<T,K> payloadReference = {0.5, -0.5, 0.75, 0.875};

    payloadAInStream << payloadAIn;
    payloadBInStream << payloadBIn;

    hls::stream<std::array<T,K>> distancesOut;
    hls::stream<std::array<T,K>> payloadOut;

    dut(distancesAInStream,payloadAInStream,distancesBInStream,payloadBInStream ,distancesOut,payloadOut);

    std::array<T,K> distance;
    distancesOut >> distance;
    std::array<T,K> payload;
    payloadOut >> payload;

    bool test_passed = true;
    std::cout << "Distance Array: " << std::endl;
    for (int n = 0; n < K; n++) {
        std::cout << "Got: " << distance[n] << " Expected: " << distanceReference[n] << std::endl;
        if (distance[n] != distanceReference[n]) {
            test_passed = false;
        }
    }

    std::cout << "Payload Array: " << std::endl;
    for (int n = 0; n < K; n++) {
        std::cout << "Got: " << payload[n] << " Expected: " << payloadReference[n] << std::endl;
        if (payload[n] != payload[n]) {
            test_passed = false;
        }
    }
    
    if (test_passed) {
        std::cout << "Test PASSED!" << std::endl;
    } else {
        std::cout << "Test FAILED!" << std::endl;
    }

    return 0;
}
