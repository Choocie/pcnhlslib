#include "dut.h"

void dut(hls::stream<array<T,F_P>> inPayloadStream[PAR],
		 hls::stream<array<array<T,F_P>,N>> outPayloadStream[PAR]) {
    buildPayloadRows<T,
                     F_P,
                     N,
                     PAR,
                     II>(inPayloadStream,
                         outPayloadStream);
}