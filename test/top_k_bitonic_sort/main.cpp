#include <iostream>
#include "hls_stream.h"
#include "dut.h"

int main() {

    hls::stream<std::array<D,K>> distancesIn[N/K];
    hls::stream<std::array<P,K>> payloadIn[N/K];

    std::array<D,K> distanceStimuli[N/K];
    distanceStimuli[0] = {0.5, 0, -0.25, -0.9875};
    distanceStimuli[1] = {0.75, 0.625, 0.0625, -0.75};
    distancesIn[0] << distanceStimuli[0];
    distancesIn[1] << distanceStimuli[1];

    std::array<D,K> distanceReference = {0.75, 0.625, 0.5, 0.0625};

    std::array<P,K> payloadStimuli[2];
    payloadStimuli[0] = {0.75, 0.975, 0.25, 0.375};
    payloadStimuli[1] = {0.5, -0.5, 0.875, 0};
    payloadIn[0] << payloadStimuli[0];
    payloadIn[1] << payloadStimuli[1];

    std::array<P,K> payloadReference = {0.5, -0.5, 0.75, 0.875};

    hls::stream<std::array<D,K>> distancesOut;
    hls::stream<std::array<P,K>> payloadOut;

    dut(distancesIn,payloadIn,distancesOut,payloadOut);

    std::array<D,K> distance;
    distancesOut >> distance;
    std::array<P,K> payload;
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
