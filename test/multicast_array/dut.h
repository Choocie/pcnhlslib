#ifndef DUT_H__
#define DUT_H__


#include "multicast.h"
#include "util.h"

const int IN_PAR  = 2;
const int OUT_PAR = 2;
const int II      = 1;

typedef int T; 

void dut(hls::stream<T> inputStream[IN_PAR],
		 hls::stream<T> outputStream[OUT_PAR][IN_PAR]);


#endif