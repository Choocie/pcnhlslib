#ifndef HOST_H__
#define HOST_H__

#include <cstring>
#include <iostream>
#include "getopt.h"
#include "xrt/xrt_kernel.h"

#include <highfive/highfive.hpp>

/* Harness global parameters */
static const int MAX_EVENTS = 2*8196;
/* Derived from underlying model */
static const int N = 64;
static const int MODEL_INPUT_WIDTH = 5;
static const int MODEL_OUTPUT_WIDTH = 9;

static const int II = 32;
static const int PAR = N / II;

typedef  int16_t model_input_t;
typedef  int16_t model_output_t;

typedef std::array<model_input_t,16> in_harness_t;
typedef std::array<model_output_t,32> out_harness_t;

#endif