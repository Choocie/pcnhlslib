#ifndef HARNESS_H__
#define HARNESS_H__

#include "weights.h"
#include "global.h"

#include "load.h"
#include "caloclusternet.h"
#include "store.h"

void harness(int &numEvents,
             int &betaThreshold,
             int &isolationThreshold,
             int* inputNumList,
             in_harness_t* inFeatureList,
             int* outputNumList,
             out_harness_t* outFeatureList);

#endif