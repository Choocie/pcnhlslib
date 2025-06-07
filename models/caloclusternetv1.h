#ifndef CALOCLUSTERNETV1_H__
#define CALOCLUSTERNETV1_H__

#include <array>

#include "hls_stream.h"

#include "gravnetconv.h"
#include "util.h"
#include "dense.h"
#include "multicast.h"
#include "condensation_point_selection.h"
#include "relu.h"
#include "smooth_sigmoid.h"

using std::array;

template<
	typename model_input_t,
    typename dense_relu_t,
	typename dense_weight_t,
	typename dense_biases_t,
	typename dense_accum_t,
	typename gravnet_relu_t,
	typename gravnet_weight_t,
	typename gravnet_biases_t,
	typename gravnet_accum_t,
	typename gravnet_coordinate_t,
	typename gravnet_feature_t,
	typename gravnet_distance_t,
	typename gravnet_exponential_t,
	typename gravnet_data_t,
	typename  model_output_t,
    typename cps_beta_t,
	typename cps_distance_t,
	typename cps_identifier_t,
	int MODEL_INPUT_WIDTH,
	int DENSE_1_OUTPUT_WIDTH,
	int DENSE_2_OUTPUT_WIDTH,
	int DENSE_3_OUTPUT_WIDTH,
	int GRAVNET_1_OUTPUT_WIDTH,
	int DENSE_4_OUTPUT_WIDTH,
	int DENSE_5_OUTPUT_WIDTH,
	int DENSE_6_OUTPUT_WIDTH,
	int DENSE_7_OUTPUT_WIDTH,
	int DENSE_8_OUTPUT_WIDTH,
	int GRAVNET_2_OUTPUT_WIDTH,
	int DENSE_9_OUTPUT_WIDTH,
	int DENSE_10_OUTPUT_WIDTH,
	int DENSE_11_OUTPUT_WIDTH,
	int DENSE_SCALE_OUTPUT_WIDTH,
	int MODEL_OUTPUT_WIDTH,
	int GRAVNET_1_K,
	int GRAVNET_2_K,
	int N,
	int PAR,
	int II,
	int BETA_WIDTH,
	int ENERGY_WIDTH,
	int TPOS_WIDTH,
	int SIGNAL_WIDTH,
	int CCORDS_WIDTH,
	int BETA_OFFSET,
	int LUT_SIZE>
void caloclusternetv1(hls::stream<array<model_input_t,  MODEL_INPUT_WIDTH>>  inputStream[PAR],
					  hls::stream<array<model_output_t,  MODEL_OUTPUT_WIDTH>> outputStream[PAR],
					  hls::stream<bool> lastStream[PAR],
					  hls::stream<int> &numStream,
					  cps_beta_t betaThreshold,
					  cps_distance_t isolationThreshold,
					  dense_weight_t dense1Weights[MODEL_INPUT_WIDTH * DENSE_1_OUTPUT_WIDTH],
					  dense_biases_t dense1Biases[DENSE_1_OUTPUT_WIDTH],
					  gravnet_weight_t dense2Weights[DENSE_1_OUTPUT_WIDTH * DENSE_2_OUTPUT_WIDTH],
					  gravnet_biases_t dense2Biases[DENSE_2_OUTPUT_WIDTH],
					  gravnet_weight_t dense3Weights[DENSE_1_OUTPUT_WIDTH * DENSE_3_OUTPUT_WIDTH],
					  gravnet_biases_t dense3Biases[DENSE_3_OUTPUT_WIDTH],
					  gravnet_weight_t dense4Weights[GRAVNET_1_OUTPUT_WIDTH * DENSE_4_OUTPUT_WIDTH],
					  gravnet_biases_t dense4Biases[DENSE_4_OUTPUT_WIDTH],
					  dense_weight_t dense5Weights[DENSE_4_OUTPUT_WIDTH * DENSE_5_OUTPUT_WIDTH],
					  dense_biases_t dense5Biases[DENSE_5_OUTPUT_WIDTH],
					  dense_weight_t dense6Weights[DENSE_5_OUTPUT_WIDTH * DENSE_6_OUTPUT_WIDTH],
					  dense_biases_t dense6Biases[DENSE_6_OUTPUT_WIDTH],
					  gravnet_weight_t dense7Weights[DENSE_6_OUTPUT_WIDTH * DENSE_7_OUTPUT_WIDTH],
					  gravnet_biases_t dense7Biases[DENSE_7_OUTPUT_WIDTH],
					  gravnet_weight_t dense8Weights[DENSE_7_OUTPUT_WIDTH * DENSE_8_OUTPUT_WIDTH],
					  gravnet_biases_t dense8Biases[DENSE_8_OUTPUT_WIDTH],
					  gravnet_weight_t dense9Weights[DENSE_8_OUTPUT_WIDTH * DENSE_9_OUTPUT_WIDTH],
					  gravnet_biases_t dense9Biases[DENSE_9_OUTPUT_WIDTH],
					  dense_weight_t dense10Weights[DENSE_9_OUTPUT_WIDTH * DENSE_10_OUTPUT_WIDTH],
					  dense_biases_t dense10Biases[DENSE_10_OUTPUT_WIDTH],
					  dense_weight_t dense11Weights[(MODEL_INPUT_WIDTH+DENSE_5_OUTPUT_WIDTH+DENSE_10_OUTPUT_WIDTH) * DENSE_11_OUTPUT_WIDTH],
					  dense_biases_t dense11Biases[DENSE_11_OUTPUT_WIDTH],
					  gravnet_weight_t denseScaleWeights[DENSE_SCALE_OUTPUT_WIDTH * MODEL_INPUT_WIDTH],
					  gravnet_biases_t denseScaleBiases[DENSE_SCALE_OUTPUT_WIDTH],
					  gravnet_weight_t tposWeights[DENSE_11_OUTPUT_WIDTH * TPOS_WIDTH],
					  gravnet_biases_t tposBiases[TPOS_WIDTH],
					  gravnet_weight_t ccordsWeights[DENSE_11_OUTPUT_WIDTH * CCORDS_WIDTH],
					  gravnet_biases_t ccordsBiases[CCORDS_WIDTH],
					  gravnet_weight_t betaWeights[DENSE_11_OUTPUT_WIDTH * BETA_WIDTH],
					  gravnet_biases_t betaBiases[BETA_WIDTH],
					  gravnet_weight_t energyWeights[DENSE_11_OUTPUT_WIDTH * ENERGY_WIDTH],
					  gravnet_biases_t energyBiases[ENERGY_WIDTH],
					  gravnet_weight_t signalWeights[DENSE_11_OUTPUT_WIDTH * SIGNAL_WIDTH],
					  gravnet_biases_t signalBiases[SIGNAL_WIDTH],
					  array<gravnet_exponential_t, static_pow2(LUT_SIZE)>  expLUT) {
	#pragma HLS inline

	hls::stream<int,II*2> multicastNumStream[4];
	multicast<int,4>(numStream,multicastNumStream);

	hls::stream<array<model_input_t,  MODEL_INPUT_WIDTH>> paddingStream[PAR];
	padding<array<model_input_t,  MODEL_INPUT_WIDTH>,PAR,II>(inputStream,multicastNumStream[0],paddingStream);

	hls::stream<array<model_input_t, MODEL_INPUT_WIDTH>,II*16> multicast0Stream[2][PAR];
	multicast<array<model_input_t, MODEL_INPUT_WIDTH>,PAR,2,II>(paddingStream,multicast0Stream);

	hls::stream<array<dense_relu_t, DENSE_1_OUTPUT_WIDTH>> dense1Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<model_input_t,
							dense_relu_t,
							dense_weight_t,
							dense_biases_t,
							dense_accum_t,
							MODEL_INPUT_WIDTH,
							DENSE_1_OUTPUT_WIDTH,
							II>(multicast0Stream[0][p], 
								dense1Stream[p],
								dense1Weights,
								dense1Biases);
	}

	hls::stream<array<dense_relu_t,DENSE_1_OUTPUT_WIDTH>,II*16> multicast1Stream[3][PAR];
	multicast<array<dense_relu_t, DENSE_1_OUTPUT_WIDTH>,PAR,3,II>(dense1Stream,multicast1Stream);

	hls::stream<array<gravnet_coordinate_t,DENSE_2_OUTPUT_WIDTH>> dense2Stream[PAR];
	for (int p = 0; p < PAR; p++) {
	#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   gravnet_coordinate_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   dense_accum_t,
					   DENSE_1_OUTPUT_WIDTH,
					   DENSE_2_OUTPUT_WIDTH,
					   II>(multicast1Stream[0][p], 
					   	   dense2Stream[p],
					       dense2Weights,
					       dense2Biases);
	}

	hls::stream<array<gravnet_feature_t,DENSE_3_OUTPUT_WIDTH>> dense3Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   gravnet_feature_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   dense_accum_t,
					   DENSE_1_OUTPUT_WIDTH,
					   DENSE_3_OUTPUT_WIDTH,
					   II>(multicast1Stream[1][p],
					   	   dense3Stream[p],
					       dense3Weights,
					       dense3Biases);
	}

	hls::stream<array<gravnet_data_t, GRAVNET_1_OUTPUT_WIDTH>> gravnet1Stream[PAR];
	gravnetconv<dense_relu_t,
				gravnet_coordinate_t,
				gravnet_feature_t,
				gravnet_distance_t,
				gravnet_exponential_t,
				gravnet_accum_t,
				gravnet_data_t,
				DENSE_2_OUTPUT_WIDTH,
				DENSE_3_OUTPUT_WIDTH,
				DENSE_1_OUTPUT_WIDTH,
				GRAVNET_1_K,
				N,
				PAR,
				II,
				LUT_SIZE>(dense2Stream,
						  dense3Stream,
						  multicast1Stream[2],
						  gravnet1Stream,
						  expLUT,
						  multicastNumStream[1]);

	hls::stream<array<gravnet_relu_t, DENSE_4_OUTPUT_WIDTH>> dense4Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<gravnet_data_t,
		gravnet_relu_t,
		gravnet_weight_t,
		gravnet_biases_t,
		gravnet_accum_t,
		GRAVNET_1_OUTPUT_WIDTH,
		DENSE_4_OUTPUT_WIDTH,
		II>(gravnet1Stream[p],
			dense4Stream[p],
		    dense4Weights,
		    dense4Biases);
	}

	hls::stream<array<dense_relu_t, DENSE_5_OUTPUT_WIDTH>> dense5Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<gravnet_relu_t,
		dense_relu_t,
		dense_weight_t,
		dense_biases_t,
		dense_accum_t,
		DENSE_4_OUTPUT_WIDTH,
		DENSE_5_OUTPUT_WIDTH,
		II>(dense4Stream[p],
			dense5Stream[p],
		    dense5Weights,
		    dense5Biases);
	}

	hls::stream<array<dense_relu_t, DENSE_5_OUTPUT_WIDTH>,II*8> multicast2Stream[2][PAR];
	multicast<array<dense_relu_t, DENSE_5_OUTPUT_WIDTH>, PAR, 2, II>(dense5Stream, multicast2Stream);

	hls::stream<array<dense_relu_t, DENSE_6_OUTPUT_WIDTH>> dense6Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<dense_relu_t,
		dense_relu_t,
		dense_weight_t,
		dense_biases_t,
		dense_accum_t,
		DENSE_5_OUTPUT_WIDTH,
		DENSE_6_OUTPUT_WIDTH,
		II>(multicast2Stream[0][p],
			dense6Stream[p],
		    dense6Weights,
		    dense6Biases);
	}

	hls::stream<array<dense_relu_t,DENSE_6_OUTPUT_WIDTH>,II*8> multicast3Stream[3][PAR];
	multicast<array<dense_relu_t, DENSE_6_OUTPUT_WIDTH>,PAR,3,II>(dense6Stream,multicast3Stream);

	hls::stream<array<gravnet_coordinate_t,DENSE_7_OUTPUT_WIDTH>> dense7Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
		gravnet_coordinate_t,
		gravnet_weight_t,
		gravnet_biases_t,
		gravnet_accum_t,
		DENSE_6_OUTPUT_WIDTH,
		DENSE_7_OUTPUT_WIDTH,
		II>(multicast3Stream[0][p], 
			dense7Stream[p],
		    dense7Weights,
		    dense7Biases);
	}

	hls::stream<array<gravnet_feature_t,DENSE_8_OUTPUT_WIDTH>> dense8Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
			gravnet_feature_t,
			gravnet_weight_t,
			gravnet_biases_t,
			gravnet_accum_t,
			DENSE_6_OUTPUT_WIDTH,
			DENSE_8_OUTPUT_WIDTH,
			II>(multicast3Stream[1][p],
				dense8Stream[p],
				dense8Weights,
				dense8Biases);
	}

	hls::stream<array<gravnet_data_t,GRAVNET_2_OUTPUT_WIDTH>> gravnet2Stream[PAR];
	gravnetconv<dense_relu_t,
				gravnet_coordinate_t,
				gravnet_feature_t,
				gravnet_distance_t,
				gravnet_exponential_t,
				gravnet_accum_t,
				gravnet_data_t,
				DENSE_7_OUTPUT_WIDTH,
				DENSE_8_OUTPUT_WIDTH,
				DENSE_6_OUTPUT_WIDTH,
				GRAVNET_2_K,
				N,
				PAR,
				II,
				LUT_SIZE>(dense7Stream,
						  dense8Stream,
						  multicast3Stream[2],
						  gravnet2Stream,
						  expLUT,
						  multicastNumStream[2]);

	hls::stream<array<gravnet_relu_t,DENSE_9_OUTPUT_WIDTH>> dense9Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<gravnet_data_t,
							gravnet_relu_t,
							gravnet_weight_t,
							gravnet_biases_t,
							gravnet_accum_t,
							GRAVNET_2_OUTPUT_WIDTH,
							DENSE_9_OUTPUT_WIDTH,
							II>(gravnet2Stream[p],
								dense9Stream[p],
								dense9Weights,
								dense9Biases);
	}

	hls::stream<array<dense_relu_t,DENSE_10_OUTPUT_WIDTH>> dense10Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<gravnet_relu_t,
							dense_relu_t,
							dense_weight_t,
							dense_biases_t,
							dense_accum_t,
							DENSE_9_OUTPUT_WIDTH,
							DENSE_10_OUTPUT_WIDTH,
							II>(dense9Stream[p],
								dense10Stream[p],
								dense10Weights,
								dense10Biases);
	}

	hls::stream<array<dense_relu_t, DENSE_SCALE_OUTPUT_WIDTH>> denseScaleStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<model_input_t,
							dense_relu_t,
							gravnet_weight_t,
							gravnet_biases_t,
							gravnet_accum_t,
							MODEL_INPUT_WIDTH,
							DENSE_SCALE_OUTPUT_WIDTH,
							II>(multicast0Stream[1][p], 
								denseScaleStream[p],
								denseScaleWeights,
								denseScaleBiases);
	}

	hls::stream<array<dense_relu_t,DENSE_SCALE_OUTPUT_WIDTH+DENSE_5_OUTPUT_WIDTH+DENSE_10_OUTPUT_WIDTH>> concat1Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		concat<dense_relu_t,
			   dense_relu_t,
			   dense_relu_t,
			   dense_relu_t,
			   DENSE_SCALE_OUTPUT_WIDTH,
			   DENSE_5_OUTPUT_WIDTH,
			   DENSE_10_OUTPUT_WIDTH,
			   II>(denseScaleStream[p],
			       multicast2Stream[1][p],
			   	   dense10Stream[p],
			   	   concat1Stream[p]);
	}

	hls::stream<array<dense_relu_t,DENSE_10_OUTPUT_WIDTH>> dense11Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_relu_saturate<dense_relu_t,
			dense_relu_t,
			dense_weight_t,
			dense_biases_t,
			dense_accum_t,
			DENSE_SCALE_OUTPUT_WIDTH+DENSE_5_OUTPUT_WIDTH+DENSE_10_OUTPUT_WIDTH,
			DENSE_11_OUTPUT_WIDTH,
			II>(concat1Stream[p],
				dense11Stream[p],
				dense11Weights,
				dense11Biases);
	}

	hls::stream<array<dense_relu_t,DENSE_11_OUTPUT_WIDTH>> multicast4Stream[5][PAR];
	multicast<array<dense_relu_t,DENSE_11_OUTPUT_WIDTH>,PAR,5,II>(dense11Stream,multicast4Stream);

	hls::stream<array<model_output_t,ENERGY_WIDTH>,12> energyStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   model_output_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   gravnet_accum_t,
					   DENSE_11_OUTPUT_WIDTH,
					   ENERGY_WIDTH,
					   II>(multicast4Stream[4][p],
					   	   energyStream[p],
					       energyWeights,
					       energyBiases);
	}

	hls::stream<array<model_output_t,TPOS_WIDTH>> tposStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   model_output_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   gravnet_accum_t,
					   DENSE_11_OUTPUT_WIDTH,
					   TPOS_WIDTH,
					   II>(multicast4Stream[0][p],
					   	   tposStream[p],
					       tposWeights,
					       tposBiases);
	}


	hls::stream<array<model_output_t,TPOS_WIDTH>,8> multicast5Stream[2][PAR];
	multicast<array<model_output_t,TPOS_WIDTH>,PAR,2,II>(tposStream,multicast5Stream);

	hls::stream<array<model_output_t,CCORDS_WIDTH>,12> ccordsStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   model_output_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   gravnet_accum_t,
					   DENSE_11_OUTPUT_WIDTH,
					   CCORDS_WIDTH,
					   II>(multicast4Stream[1][p],
					   	   ccordsStream[p],
					       ccordsWeights,
					       ccordsBiases);
	}

	hls::stream<array<model_output_t,BETA_WIDTH>> betaStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   model_output_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   gravnet_accum_t,
					   DENSE_11_OUTPUT_WIDTH,
					   BETA_WIDTH,
					   II>(multicast4Stream[2][p],
					   	   betaStream[p],
					       betaWeights,
					       betaBiases);
	}

	hls::stream<array<model_output_t, BETA_WIDTH>> smoothsigmoid1Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		smooth_sigmoid<model_output_t,
					   model_output_t,
					   BETA_WIDTH,
					   II>(betaStream[p], 
					       smoothsigmoid1Stream[p]);
	}


	hls::stream<array<model_output_t,BETA_WIDTH>,8> multicast6Stream[2][PAR];
	multicast<array<model_output_t,BETA_WIDTH>,PAR,2,II>(smoothsigmoid1Stream,multicast6Stream);


	hls::stream<array<model_output_t,SIGNAL_WIDTH>> signalStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		dense_saturate<dense_relu_t,
					   model_output_t,
					   gravnet_weight_t,
					   gravnet_biases_t,
					   gravnet_accum_t,
					   DENSE_11_OUTPUT_WIDTH,
					   SIGNAL_WIDTH,
					   II>(multicast4Stream[3][p],
					   	   signalStream[p],
					       signalWeights,
					       signalBiases);
	}

	hls::stream<array<model_output_t, SIGNAL_WIDTH>> smoothsigmoid2Stream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		smooth_sigmoid<model_output_t,
					   model_output_t,
					   SIGNAL_WIDTH,
					   II>(signalStream[p], 
					       smoothsigmoid2Stream[p]);
	}

	hls::stream<ap_uint<N>> condensationPoints;
	condensation_point_selection<model_output_t,
								 cps_distance_t,
								 cps_beta_t,
								 cps_identifier_t,
								 BETA_WIDTH,
								 TPOS_WIDTH,
								 N,
								 PAR,
								 II>(multicast5Stream[1],
								     multicast6Stream[1],
								     condensationPoints,
								     betaThreshold,
								     isolationThreshold);
 
	hls::stream<array<model_output_t,MODEL_OUTPUT_WIDTH>, II*16> concatStream[PAR];
	for (int p = 0; p < PAR; p++) {
		#pragma HLS unroll
		concat<model_output_t,
			   BETA_WIDTH,
			   ENERGY_WIDTH,
			   TPOS_WIDTH,
			   SIGNAL_WIDTH,
			   CCORDS_WIDTH,
			   II>(multicast6Stream[0][p],
			       energyStream[p],
			       multicast5Stream[0][p],
			       smoothsigmoid2Stream[p],
			       ccordsStream[p],
			       concatStream[p]);
	}

	filter<model_output_t,
		   N,
		   PAR,
		   II,
		   MODEL_OUTPUT_WIDTH,
		   BETA_OFFSET>(concatStream,
						multicastNumStream[3],
						condensationPoints,
						outputStream,
						lastStream);
}

#endif