#include "dut.h"

void dut(hls::stream<array<input_t,F>> &inputStream,
         hls::stream<array<output_t,F>> &outputStream) {
    // ReLU Funktion aufrufen
    smooth_sigmoid<input_t,
    output_t,
    F,
    II>(
  inputStream,
  outputStream);
}
