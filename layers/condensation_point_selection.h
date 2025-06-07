#ifndef CLUSTER_SELECTION_H__
#define CLUSTER_SELECTION_H__

#include "util.h"
#include "hls_stream.h"
#include "bitonic_sort.h"
#include "gravnetconv.h"

using namespace std;

template <typename id_t> 
struct tuple_t { 
	bool cluster_candidate; 
	id_t id;
};

template<typename input_t,
		 typename distance_t,
		 int F_C,
		 int N,
		 int PAR,
		 int II>
void calculate_isolation(array<input_t,F_C> query[N],
						 array<input_t,F_C> points[N],
						 hls::stream<array<ap_uint<N>,N>> &isolationStream,
						 distance_t isolationThreshold) {
	
	#pragma HLS stable variable=isolationThreshold

	#pragma HLS ARRAY_PARTITION variable=points dim=0 complete
	#pragma HLS ARRAY_RESHAPE variable=query type=complete  dim=1

	distance_t accumulate[PAR][N][F_C];
	#pragma HLS ARRAY_PARTITION variable=accumulate dim=0 complete

	array<ap_uint<N>,N> distances;

	for(int ii = 0; ii < II; ii++) {
		#pragma HLS pipeline II=1 rewind style=frp
		for(int p = 0; p < PAR; p++) {
			int q = PAR*ii + p;		

			for(int n = 0; n < N; n++) {
				for(int f = 0; f < F_C; f++) {
					auto const last = (f == 0) ? ((distance_t) 0) : accumulate[p][n][f-1];
					auto const difference = (query[q][f] >= points[n][f]) ? query[q][f] - points[n][f]: points[n][f] - query[q][f];
					accumulate[p][n][f] = (difference) + last;
				}
			}

			for(int n = 0; n < N; n++) {
					distances[PAR*ii+p].set(n,((distance_t) accumulate[p][n][F_C-1] >= isolationThreshold));
			}
		}

		if(ii == II - 1) {
			isolationStream << distances;
		}
	}
}


template<typename distance_t,
		 typename id_t,
         int N,
         int PAR,
         int II>
void condensation_point_isolation_criteria(hls::stream<array<ap_uint<N>,N>>	    &isolationStream,
	                                       hls::stream<ap_uint<N>>				&inFlagStream,
				   						   hls::stream<array<id_t, N>> 			&sortedIdentifierStream,
                   						   hls::stream<ap_uint<N>>              &outCluster) {
    ap_uint<N> flags;
    id_t ids[N];
    #pragma HLS ARRAY_RESHAPE variable=ids type=complete dim=1

    ap_uint<N> out;

	int nonzeros;
	int select;

	array<ap_uint<N>,N> isolations;

    for (int i = 0; i < II; i++) {
        #pragma HLS pipeline II=1 rewind
        if(i == 0) {
            out = 0;
            isolationStream >> isolations;
            array<id_t, N> buf;
            sortedIdentifierStream >> buf;
			inFlagStream >> flags;
            for(int n = 0; n < N; n++) {
                ids[n] = buf[n];
            }
        }
		for(int p = 0; p < PAR; p++) {
			select = ids[i*PAR+p];
			out.set(select,flags.test(select));
			flags = flags & isolations[select];
		}
        if(i == II - 1)
            outCluster << out;
    }
}


template<typename beta_t,
		typename id_t,
		int BETA_WIDTH,
		int N,
		int PAR,
		int II>
void condensation_point_beta_candidates(hls::stream<array<beta_t,BETA_WIDTH>> inbetaStream[PAR],
								 	 hls::stream<array<beta_t,N>>		  	  &outBetaStream,
								 	 hls::stream<array<id_t,N>>				  &outIdentifierStream,
									 hls::stream<ap_uint<N>>				  &outFlagStream,
                				 	 beta_t 								  betaThreshold) {
    #pragma HLS stable variable=betaThreshold
	array<beta_t, N> beta_buffer;
	array<id_t, N> id_buffer;
	ap_uint<N> flag;

	for (int ii = 0; ii < II; ii++) {
		#pragma HLS PIPELINE II=1 rewind
		for (int p = 0; p < PAR; p++) {
			#pragma HLS UNROLL
            if(ii*PAR+p < N) {
                array<beta_t,BETA_WIDTH> beta;
			    inbetaStream[p] >> beta;
                
                beta_buffer[ii*PAR+p] = beta[0];
                id_buffer[ii*PAR+p] = ii*PAR+p;
				flag.set(ii*PAR+p,(beta[0] < betaThreshold) ? false : true);
            }
		}

        if(ii == II-1) {
	        outIdentifierStream << id_buffer;
	        outBetaStream << beta_buffer;
			outFlagStream << flag;
        }
    }
}

template<typename input_t,
        typename distance_t,
		typename beta_t,
		typename identifier_t,
		int BETA_WIDTH,
        int TPOS_WIDTH,
        int N,
        int PAR,
        int II>
void condensation_point_selection(hls::stream<array<input_t,TPOS_WIDTH>> 	coordinateStream[PAR],
                      			hls::stream<array<beta_t,BETA_WIDTH>>	    inbetaStream[PAR],
                      			hls::stream<ap_uint<N>>                     &condensationPoints,
                      			beta_t 								        betaThreshold,
					  			distance_t 						 	        isolationThreshold){
	#pragma HLS inline

    array<input_t,TPOS_WIDTH> points[N];
	array<input_t,TPOS_WIDTH> query[N];
	build_point_rows<input_t,TPOS_WIDTH,N,PAR,II>(coordinateStream,query,points);

	hls::stream<array<identifier_t, N>> unsortedIdentifierStream;
	hls::stream<array<beta_t, N>> betaConcat;
	hls::stream<ap_uint<N>,II> flagStream;
	condensation_point_beta_candidates<beta_t,identifier_t,BETA_WIDTH,N,PAR,II>(inbetaStream,betaConcat,unsortedIdentifierStream,flagStream,betaThreshold);

	hls::stream<array<identifier_t, N>> sortedIdentifierStream;
	bitonic_sort<beta_t,identifier_t,N>(betaConcat,unsortedIdentifierStream,sortedIdentifierStream);

	hls::stream<array<ap_uint<N>,N>> isolationStream;
	calculate_isolation<input_t,distance_t,TPOS_WIDTH,N,PAR,II>(query,points,isolationStream,isolationThreshold);

    condensation_point_isolation_criteria<distance_t,identifier_t,N,PAR,II>(isolationStream,flagStream,sortedIdentifierStream,condensationPoints);

}


#endif
