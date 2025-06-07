#include "dut.h"

void dut(hls::stream<array<D, N / 2>> &keysAInStream, hls::stream<array<P, N / 2>> &valuesAInStream,
        hls::stream<array<D, N / 2>> &keysBInStream, hls::stream<array<P, N / 2>> &valuesBInStream,
        hls::stream<array<D, K>> &keysOutStream, hls::stream<array<P, K>> &valuesOutStream) {
                top_k_bitonic_merge<D,P,N,K>
                (keysAInStream,
                valuesAInStream,
                keysBInStream,
                valuesBInStream,
                keysOutStream,
                valuesOutStream);
}

