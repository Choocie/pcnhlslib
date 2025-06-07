#ifndef DUT_H__
#define DUT_H__


#include "gravnetconv.h"
#include "util.h"

const int WIDTH = 2;
const int II    = 1;

typedef int T;

void dut(hls::stream<array<T,WIDTH>> &aInputStream,
         hls::stream<array<T,WIDTH>> &bInputStream,
         hls::stream<array<T,2*WIDTH>> &outputStream);

#endif