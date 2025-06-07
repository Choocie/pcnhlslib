#include "dut.h"


void dut(hls::stream<array<T,(K-1)*F_P>> &inputStream,
	   hls::stream<array<T,F_P>> &maxStream,
	   hls::stream<array<T,F_P>> &avgStream) {
    aggregate<T,
              K,
              F_P,
              II>(inputStream,
                  maxStream,
                  avgStream);
}