#ifndef DUT_H__
#define DUT_H__


#include "multicast.h"
#include "util.h"

const int PAR = 2;
const int II  = 1;

typedef int T; 

void dut(hls::stream<T> &inputStream,
		 hls::stream<T> outputStream[PAR]);


#endif