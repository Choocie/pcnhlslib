#ifndef GLOBALS_H__
#define GLOBALS_H__

#include "util.h"

#ifndef __NUM_POINTS__
    #error "__NUM_POINTS__ not defined or invalid"
#endif

#ifndef __INITIATION_INTERVAL__
    #error "__INITIATION_INTERVAL__ not defined or invalid"
#endif

/* Hardware Parameters */
static const int N = __NUM_POINTS__;
static const int II = __INITIATION_INTERVAL__;
static const int PAR = N / II;

/* Model Parameters ‚*/
static const int MODEL_INPUT_WIDTH = 5;
static const int MODEL_OUTPUT_WIDTH = 9;

static const int DENSE_SCALE_OUTPUT_WIDTH = 5;

static const int DENSE_1_OUTPUT_WIDTH = 16;
static const int DENSE_2_OUTPUT_WIDTH = 6;
static const int DENSE_3_OUTPUT_WIDTH = 8;

static const int GRAVNET_1_OUTPUT_WIDTH = 32;

static const int DENSE_4_OUTPUT_WIDTH = 32;
static const int DENSE_5_OUTPUT_WIDTH = 16;
static const int DENSE_6_OUTPUT_WIDTH = 16;
static const int DENSE_7_OUTPUT_WIDTH = 6;
static const int DENSE_8_OUTPUT_WIDTH = 8;

static const int GRAVNET_2_OUTPUT_WIDTH = 32;

static const int DENSE_9_OUTPUT_WIDTH = 32;
static const int DENSE_10_OUTPUT_WIDTH = 16;
static const int DENSE_11_OUTPUT_WIDTH = 16;

/* HEAD PARAMETERS */
static const int BETA_WIDTH = 1;
static const int SIGNAL_WIDTH = 1;
static const int TPOS_WIDTH = 3;
static const int ENERGY_WIDTH = 1;
static const int CCORDS_WIDTH = 3;

/* GraVNetConv Parameters */
static const int GRAVNET_1_K = 8; 
static const int GRAVNET_2_K = 8; 

/* Types */
typedef ap_uint<ceillog2(N)> cps_identifier_t;
typedef ap_fixed<16,1> model_input_t; 
typedef ap_fixed<16,1> model_output_t; 

#if __RESOLUTION__ == 8
    typedef ap_fixed<8,1>  dense_data_t;
    typedef ap_fixed<8,1>  dense_relu_t;
    typedef ap_fixed<8,1>  dense_weight_t;
    typedef ap_fixed<8,1>  dense_biases_t;
    typedef ap_fixed<20,5> dense_accum_t;
    typedef ap_fixed<8,1>  gravnet_data_t;
    typedef ap_fixed<8,1>  gravnet_relu_t;
    typedef ap_fixed<8,1>  gravnet_coordinate_t;
    typedef ap_fixed<8,1>  gravnet_feature_t;
    typedef ap_fixed<20,5>  gravnet_accum_t;
    typedef ap_fixed<20,5> gravnet_distance_t; 
    typedef ap_ufixed<8,1> gravnet_exponential_t;
    typedef ap_fixed<8,1>  gravnet_weight_t;
    typedef ap_fixed<8,1>  gravnet_biases_t;
    typedef ap_fixed<20,5> cps_distance_t;
    typedef ap_fixed<16,1> cps_beta_t;
#elif __RESOLUTION__ == 16
    typedef ap_fixed<16,1>  dense_data_t;
    typedef ap_fixed<16,1>  dense_relu_t;
    typedef ap_fixed<16,1>  dense_weight_t;
    typedef ap_fixed<16,1>  dense_biases_t;
    typedef ap_fixed<36,5> dense_accum_t;
    typedef ap_fixed<16,1>  gravnet_data_t;
    typedef ap_fixed<16,1>  gravnet_relu_t;
    typedef ap_fixed<16,1>  gravnet_coordinate_t;
    typedef ap_fixed<16,1>  gravnet_feature_t;
    typedef ap_fixed<36,5>  gravnet_accum_t;
    typedef ap_fixed<36,5> gravnet_distance_t; 
    typedef ap_ufixed<16,1> gravnet_exponential_t;
    typedef ap_fixed<16,1>  gravnet_weight_t;
    typedef ap_fixed<16,1>  gravnet_biases_t;
    typedef ap_fixed<32,5> cps_distance_t;
    typedef ap_fixed<16,1> cps_beta_t;
#else
    #error "__RESOLUTION__ unknown or undefined."
#endif

static const int BETA_OFFSET = 0;
static const int LUT_SIZE = 9;

/* Kernel Parameters */
const int MAX_EVENTS = 2*8192;

typedef std::array<model_input_t,16> in_harness_t;
typedef std::array<model_output_t,32> out_harness_t;


#endif