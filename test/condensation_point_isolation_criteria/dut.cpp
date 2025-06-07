#include "dut.h"

void dut(distance_t 								            distanceIn[N][N],
			  hls::stream<array<tuple_t<identifier_t>, N>> &sortedTuplesStream,
       	hls::stream<bool> 						         &outCluster) {
                condensation_point_isolation_criteria<distance_t,identifier_t,N,PAR,II>
                (distanceIn,
                sortedTuplesStream,
                outCluster);
}

