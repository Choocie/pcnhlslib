#ifndef DUT_H__
#define DUT_H__

#include "util.h"

const int PAR   = 2;
const int II    = 8;

typedef array<int,2>  T; 

void dut(hls::stream<T> inputStream[PAR],
         hls::stream<int> &numStream,
         hls::stream<T> outputStream[PAR],
         hls::stream<bool> lastStream[PAR]);

#endif