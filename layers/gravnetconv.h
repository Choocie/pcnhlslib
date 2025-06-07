#ifndef _GRAVNET_H
#define _GRAVNET_H

#include <ap_fixed.h>
#include <hls_stream.h>
#include <hls_math.h>
#include <ap_int.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "util.h"
#include "top_k_bitonic_sort.h"

using namespace std;

template<typename T,
		 int F_C,
		 int N,
		 int PAR,
		 int II>
void build_point_rows(hls::stream<array<T,F_C>> inCoordinateStream[PAR],
					array<T,F_C> query[N],
					array<T,F_C> points[N]) {

	#pragma HLS array_reshape variable=query type=cyclic factor=PAR dim=1
	#pragma HLS array_reshape variable=points type=cyclic factor=PAR dim=1

	for(int ii=0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind
		for(int p = 0; p < PAR; p++) {
			array<T,F_C> inCoordinates;
			inCoordinateStream[p] >> inCoordinates;
			points[ii*PAR+p] = inCoordinates;
			query[ii*PAR+p]  = inCoordinates;
		}
	}
}


template<typename T,
		 int F,
		 int N,
		 int K,
		 int PAR,
		 int II>
static void store_features(hls::stream<array<T,F>> input_stream[PAR],
						  array<T,F> output_buffer[PAR][N][K]) {
	for(int ii = 0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind
		for(int p = 0;p < PAR; p++) {
			#pragma HLS unroll
			array<T,F> input;
			input_stream[p] >> input;
			for(int b = 0; b < PAR; b++) {
				for(int k = 0; k < K; k++) {
					#pragma HLS unroll
					output_buffer[b][ii*PAR + p][k] = input;            
				}
			}
		}
	}
}

template<typename T,
		 int F,
		 int N,
		 int K,
		 int PAR,
		 int II>
static void fetchFeatures(array<T,F> input_buffer[PAR][N][K],
				          hls::stream<array<ap_uint<ceillog2(N)>, K>> identifier_stream[PAR],
				          hls::stream<array<array<T, F>,K>> output_stream[PAR]) {
	for(int ii = 0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind
		for(int p = 0; p < PAR; p++) {
			array<ap_uint<ceillog2(N)>, K> ids;
			identifier_stream[p] >> ids;
			array<array<T,F>,K> outputs;       
			for(int k = 0; k < K; k++) {
				#pragma HLS unroll
				for(int f = 0; f < F; f++) {
					#pragma HLS unroll
					outputs[k][f] = input_buffer[p][static_cast<int>(ids[k])][k][f];
				}
			}
			output_stream[p] << outputs;
		}
	}
}

template<typename input_t,
		 typename distance_t,
		 int F_C,
		 int N,
		 int K,
		 int PAR,
		 int II>
void calculate_distances(array<input_t,F_C> query[N],
						array<input_t,F_C> points[N],
						hls::stream<array<distance_t,K>> distanceStream[PAR][N / K],
						hls::stream<array<ap_uint<ceillog2(N)>, K>> identifier_stream[PAR][N / K],
						hls::stream<int> &numStream) {
	#pragma HLS ARRAY_PARTITION variable=points dim=0 complete
	#pragma HLS ARRAY_RESHAPE variable=query dim=1 type=complete

	distance_t accumulate[PAR][N][F_C];
	#pragma HLS ARRAY_PARTITION variable=accumulate dim=0 complete

	int num = 0;

	for(int ii = 0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind
		for(int p = 0; p < PAR; p++) {
			int q = PAR*ii + p;

			if(ii == 0 && p == 0)
				numStream >> num;

			for(int n = 0; n < N; n++) {
				for(int f = 0; f < F_C; f++) {
					auto const last = (f == 0) ? ((distance_t) 0) : accumulate[p][n][f-1];
					auto const difference = (query[q][f] >= points[n][f]) ? query[q][f] - points[n][f]: points[n][f] - query[q][f];
					accumulate[p][n][f] = (difference) + last;
				}
			}

			for(int nk = 0; nk < N/K;nk++) {

				array<distance_t,K> distances = init_array<distance_t,K>(-ap_fixed_max<distance_t::width,distance_t::iwidth>());

				for(int k = 0; k < K; k++) {
					int n = nk*K+k;
					if(q < num && n < num)
						distances[k] = (distance_t) -accumulate[p][n][F_C-1];
				}
				distanceStream[p][nk] << distances;

				array<ap_uint<ceillog2(N)>, K> identifier = init_range<ap_uint<ceillog2(N)>, K>(nk*K,1);
				identifier_stream[p][nk] << identifier;
			}
		}
	}
}

template<typename input_t,
		 typename output_t,
		 int K,
		 int II,
		 int LUT_SIZE>
void exponential(hls::stream<array<input_t,K>>    &distanceIn,
		         hls::stream<array<output_t,K-1>> &distanceExp,
				 array<output_t, static_pow2(LUT_SIZE)> &expLUT) {
	array<input_t,K> tempIn;
	#pragma HLS array_partition variable=tempIn type=complete dim=1

	array<output_t,K-1> tempOut;
	#pragma HLS array_partition variable=tempOut type=complete dim=1

	#pragma HLS array_partition variable=expLUT type=complete dim=1

	for(int ii = 0; ii < II;ii++) {
			#pragma HLS pipeline II=1 rewind
			distanceIn >> tempIn;
			for(int i = 0; i < K-1; i++) {
				input_t inverted = -tempIn[i+1];
				ap_uint<LUT_SIZE> index;
				if (inverted >= static_cast<input_t>(0.5))
					index = static_pow2(LUT_SIZE) - 1;
				else if(inverted <= static_cast<input_t>(0))
					index = 0;
				else
					index = inverted.range(input_t::width - input_t::iwidth,input_t::width - input_t::iwidth - LUT_SIZE);
				tempOut[i] = expLUT[index];
		}

		distanceExp << tempOut;
	}
}

template<typename input_t,
		 typename output_t,
		 int K,
		 int II>
void exponential(hls::stream<array<input_t,K>>    &distanceIn,
		         hls::stream<array<output_t,K-1>> &distanceExp) {
	array<input_t,K> tempIn;
	#pragma HLS array_partition variable=tempIn type=complete dim=1

	array<output_t,K-1> tempOut;
	#pragma HLS array_partition variable=tempOut type=complete dim=1

	for(int ii = 0; ii < II;ii++) {
			#pragma HLS pipeline II=1 rewind
			distanceIn >> tempIn;
			for(int i = 0; i < K-1; i++) {
				tempOut[i] = static_cast<output_t>(hls::exp(static_cast<float>(-tempIn[i+1])));
		}
		distanceExp << tempOut;
	}
}


template<typename feature_t,
		 typename distance_t,
		 typename output_t,
		 int K,
		 int F_P,
		 int II>
void drop_and_multiply(hls::stream<array<array<feature_t,F_P>,K>> &featureStream,
			  hls::stream<array<distance_t,K-1>>      	 &distancesStream,
			  hls::stream<array<output_t,(K-1)*F_P>>     &outputStream){
	array<distance_t,K-1> distances;
	#pragma HLS array_partition variable=distances type=complete dim=1

	array<array<feature_t,F_P>,K> features;
	#pragma HLS array_partition variable=features type=complete dim=1

	array<output_t,(K-1)*F_P> output;
	#pragma HLS array_partition variable=output type=complete dim=1

	for(int ii = 0; ii < II;ii++) {
		#pragma HLS pipeline II=1 rewind
		featureStream >> features;
		distancesStream >> distances;

		for(int i = 0; i < K-1; i++){
			for(int j = 0; j < F_P; j++){
				auto result = distances[i] * features[i+1][j];
				output[i*F_P+j] = static_cast<output_t>(result);
			}
		}
		outputStream << output;
	}
}

template<typename data_t,
		 typename accum_t,
		 int K,
		 int F_P,
		 int II>
void aggregate(hls::stream<array<data_t,(K-1)*F_P>> &inputStream,
			   hls::stream<array<data_t,F_P>> &maxStream,
			   hls::stream<array<data_t,F_P>> &avgStream){

	const accum_t inverse = static_cast<accum_t>(1.0 / static_cast<float>(K-1));

	array<data_t,(K-1)*F_P> input;
	#pragma HLS array_partition variable=input type=complete dim=1

	array<data_t,F_P> maxArray;
	#pragma HLS array_partition variable=maxArray type=complete dim=1

	for(int ii = 0; ii < II;ii++) {
		#pragma HLS pipeline II=1 rewind

		array<data_t,F_P> avgArray;
		#pragma HLS array_partition variable=avgArray type=complete dim=1

		inputStream >> input;

		for(int i = 0; i < F_P; i++){
			maxArray[i] = input[i];
		}

		for(int i = 0; i < F_P; i++){
			for(int j = 0; j < K-1; j++){
				maxArray[i] = std::max(maxArray[i], input[j*F_P+i]);
			}
		}
		maxStream << maxArray;

		for(int i =0; i<F_P;i++){
			array<accum_t,F_P> sumArray = {0};
			for(int j=0; j<K-1; j++){
				sumArray[i] += static_cast<accum_t>(input[j*F_P+i]);
			}
			avgArray[i] = static_cast<data_t>(sumArray[i] * inverse);
		}
		avgStream << avgArray;
	}
}


template<typename input_t,
		 typename output_t,
		 int F_P,
		 int II,
		 int F_IN>
void combine(hls::stream<array<input_t,F_IN>> 	    &inputFeatureStream,
			 hls::stream<array<output_t,F_P>> 		&inputMaxStream,
			 hls::stream<array<output_t,F_P>> 		&inputAvgStream,
			 hls::stream<array<output_t,F_IN+2*F_P>>  &outputStream){
	array<input_t,F_IN> features;
	#pragma HLS array_partition variable=features type=complete dim=1

	array<output_t,F_P> maxArray;
	#pragma HLS array_partition variable=maxArray type=complete dim=1

	array<output_t,F_P> avgArray;
	#pragma HLS array_partition variable=avgArray type=complete dim=1

	array<output_t,F_IN+2*F_P> output;
	#pragma HLS array_partition variable=output type=complete dim=1

	for(int ii=0; ii < II; ii++){
		#pragma HLS pipeline II=1 rewind

		inputFeatureStream >> features;
		inputMaxStream >> maxArray;
		inputAvgStream >> avgArray;

		for(int i = 0; i < F_IN; i++){
			output[i] = static_cast<output_t>(features[i]);
		}
		for(int i = 0 ; i < F_P; i++){
			output[F_IN+i] = maxArray[i];
		}
		for(int i=0; i < F_P;i++){
			output[F_IN+F_P+i] = avgArray[i];
		}
		outputStream << output;
	}
}

template<typename T,
	     int WIDTH,
		 int II>
void merge(hls::stream<array<T,WIDTH>> &aInputStream,
		   hls::stream<array<T,WIDTH>> &bInputStream,
		   hls::stream<array<T,2*WIDTH>> &outputStream) {
	array<T,WIDTH> aInput;
	#pragma HLS array_partition variable=aInput complete dim=1

	array<T,WIDTH> bInput;    
	#pragma HLS array_partition variable=bInput complete dim=1

	array<T,2*WIDTH> output;
	#pragma HLS array_partition variable=output complete dim=1

	for (int ii = 0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind

		aInputStream >> aInput;
		for (int i = 0; i < WIDTH; i++) {
			output[i] = aInput[i];
		}

		bInputStream >> bInput;
		for (int i = 0; i < WIDTH; i++) {
			output[WIDTH + i] = bInput[i];
		}

		outputStream << output;
	}
}

template<typename input_t,
         typename coordinate_t,
         typename feature_t,
		 typename distance_t,
		 typename exponential_t,
		 typename accum_t,
		 typename output_t,
		 int F_C,
		 int F_P,
		 int F_IN,
		 int K,
		 int N,
		 int PAR,
		 int II,
		 int LUT_SIZE>
void gravnetconv(hls::stream<array<coordinate_t,F_C>>    coordinateStream[PAR],
                 hls::stream<array<feature_t,F_P>>       featureStream[PAR],
				 hls::stream<array<input_t,F_IN>>        inputStream[PAR],
                 hls::stream<array<output_t,F_IN+2*F_P>> outputStream[PAR],
				 array<exponential_t, static_pow2(LUT_SIZE)>  expLUT,
				 hls::stream<int>                        &numStream) {
	#pragma HLS inline

	array<coordinate_t,F_C> points[N];
	array<coordinate_t,F_C> query[N];
	build_point_rows<coordinate_t,F_C,N,PAR,II>(coordinateStream,query,points);

	array<feature_t,F_P> featureBuffer[PAR][N][K];
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=complete dim=1
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=cyclic factor=PAR dim=2
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=complete dim=3
	#pragma HLS stream variable=featureBuffer type=pipo depth=6

	store_features<feature_t,F_P,N,K,PAR,II>(featureStream,featureBuffer);

	hls::stream<array<distance_t,K>> unsortedDistances[PAR][N/K];
	hls::stream<array<ap_uint<ceillog2(N)>, K>> unsortedPayload[PAR][N / K];
	calculate_distances<coordinate_t,distance_t,F_C,N,K,PAR,II>(query,points,unsortedDistances,unsortedPayload,numStream);

	hls::stream<array<ap_uint<ceillog2(N)>, K>> sortedPayload[PAR];
	hls::stream<array<distance_t,K>> sortedDistances[PAR];

    //FIXME: Yields error when using for loop.
  	top_k_bitonic_sort<distance_t,ap_uint<ceillog2(N)>,N,K,II>(unsortedDistances[0], unsortedPayload[0],sortedDistances[0],sortedPayload[0]);
  	top_k_bitonic_sort<distance_t,ap_uint<ceillog2(N)>,N,K,II>(unsortedDistances[1], unsortedPayload[1],sortedDistances[1],sortedPayload[1]);

	//TODO: Depends on the time for calculating exponential functions
	hls::stream<array<array<feature_t,F_P>,K>,16> sortedFeatures[PAR];
	fetchFeatures<feature_t,F_P,N,K,PAR,II>(featureBuffer,sortedPayload,sortedFeatures);

	hls::stream<array<exponential_t,K-1>> weightedDistances[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		exponential<distance_t,exponential_t,K,II,LUT_SIZE>(sortedDistances[p], weightedDistances[p], expLUT);
	}

	hls::stream<array<output_t,(K-1)*F_P>> messages[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		drop_and_multiply<feature_t,exponential_t,output_t,K,F_P,II>(sortedFeatures[p], weightedDistances[p], messages[p]);
	}

	 hls::stream<array<output_t,F_P>> avgStream[PAR];
	 hls::stream<array<output_t,F_P>> maxStream[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		aggregate<output_t,accum_t,K,F_P,II>(messages[p], maxStream[p],avgStream[p]);
	}

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		combine<input_t,output_t,F_P,II,F_IN>(inputStream[p], avgStream[p], maxStream[p], outputStream[p]);
	}

}


template<typename input_t,
         typename coordinate_t,
         typename feature_t,
		 typename distance_t,
		 typename exponential_t,
		 typename accum_t,
		 typename output_t,
		 int F_C,
		 int F_P,
		 int F_IN,
		 int K,
		 int N,
		 int PAR,
		 int II>
void gravnetconv(hls::stream<array<coordinate_t,F_C>>    coordinateStream[PAR],
                 hls::stream<array<feature_t,F_P>>       featureStream[PAR],
				 hls::stream<array<input_t,F_IN>>        inputStream[PAR],
                 hls::stream<array<output_t,F_IN+2*F_P>> outputStream[PAR],
				 hls::stream<int>                        &numStream) {
	#pragma HLS inline

	array<coordinate_t,F_C> points[N];
	array<coordinate_t,F_C> query[N];
	build_point_rows<coordinate_t,F_C,N,PAR,II>(coordinateStream,query,points);

	array<feature_t,F_P> featureBuffer[PAR][N][K];
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=complete dim=1
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=cyclic factor=PAR dim=2
    #pragma HLS ARRAY_PARTITION variable=featureBuffer type=complete dim=3
	#pragma HLS stream variable=featureBuffer type=pipo depth=8

	store_features<feature_t,F_P,N,K,PAR,II>(featureStream,featureBuffer);

	hls::stream<array<distance_t,K>> unsortedDistances[PAR][N/K];
	hls::stream<array<ap_uint<ceillog2(N)>, K>> unsortedPayload[PAR][N / K];
	calculate_distances<coordinate_t,distance_t,F_C,N,K,PAR,II>(query,points,unsortedDistances,unsortedPayload,numStream);

	hls::stream<array<ap_uint<ceillog2(N)>, K>> sortedPayload[PAR];
	hls::stream<array<distance_t,K>> sortedDistances[PAR];

    //FIXME: Yields error when using for loop.
  	top_k_bitonic_sort<distance_t,ap_uint<ceillog2(N)>,N,K,II>(unsortedDistances[0], unsortedPayload[0],sortedDistances[0],sortedPayload[0]);
  	top_k_bitonic_sort<distance_t,ap_uint<ceillog2(N)>,N,K,II>(unsortedDistances[1], unsortedPayload[1],sortedDistances[1],sortedPayload[1]);

	//TODO: Depends on the time for calculating exponential functions
	hls::stream<array<array<feature_t,F_P>,K>,48> sortedFeatures[PAR];
	fetchFeatures<feature_t,F_P,N,K,PAR,II>(featureBuffer,sortedPayload,sortedFeatures);

	hls::stream<array<exponential_t,K-1>> weightedDistances[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		exponential<distance_t,exponential_t,K,II>(sortedDistances[p], weightedDistances[p]);
	}

	hls::stream<array<output_t,(K-1)*F_P>> messages[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		drop_and_multiply<feature_t,exponential_t,output_t,K,F_P,II>(sortedFeatures[p], weightedDistances[p], messages[p]);
	}

	 hls::stream<array<output_t,F_P>> avgStream[PAR];
	 hls::stream<array<output_t,F_P>> maxStream[PAR];

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		aggregate<output_t,accum_t,K,F_P,II>(messages[p], maxStream[p],avgStream[p]);
	}

	for(int p = 0; p < PAR; p++) {    
		#pragma HLS unroll
		combine<input_t,output_t,F_P,II,F_IN>(inputStream[p], avgStream[p], maxStream[p], outputStream[p]);
	}

}

#endif
