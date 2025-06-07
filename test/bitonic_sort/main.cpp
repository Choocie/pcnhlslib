#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    hls::stream<std::array<T,N>> distancesInStream;
    hls::stream<std::array<T,N>> payloadInStream;

    std::array<T,N> distancesIn = {-1, 0, -0.0625, -0.25, 0.5};
    std::array<T,N> distanceReference = {0.5, 0, -0.0625, -0.25, -1};


    distancesInStream << distancesIn;
    
    std::array<T,N> payloadIn = {0, -0.125, 0.25, -0.8, 2};
    std::array<T,N> payloadReference = {2, -0.125, 0.25, -0.8, 0};

    payloadInStream << payloadIn;

    hls::stream<std::array<T,N>> distancesOut;
    hls::stream<std::array<T,N>> payloadOut;

    dut(distancesInStream,payloadInStream,distancesOut,payloadOut);

    std::array<T,N> distance;
    distancesOut >> distance;
    std::array<T,N> payload;
    payloadOut >> payload;

    bool test_passed = true;
    std::cout << "Distance Array: " << std::endl;
    for (int n = 0; n < N; n++) {
        std::cout << "Got: " << distance[n] << " Expected: " << distanceReference[n] << std::endl;
        if (distance[n] != distanceReference[n]) {
            test_passed = false;
        }
    }

    std::cout << "Payload Array: " << std::endl;
    for (int n = 0; n < N; n++) {
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