/**
Description: Top-level test file for standalone caloclusternet test suite.
Author: Marc Neu
Date: 24.01.2025
*/
#include <iostream>
#include "getopt.h"
#include "harness.h"

//#define __HLS_COSIM__

#if !defined(__HLS_COSIM__)
    #include <highfive/highfive.hpp>
#endif

using std::array;

char* input_path = (char *) "build/test.h5";
char* output_path = (char *) "build/output.h5";
int numEvents = 1;

int16_t stimuli[6][16] = {{118, 688, 8091, 1874, -4066, 24, 709, 6051, 5214, 225, 0, 0, 0, 0, 0, 0},
    {0, 5440, 8082, 1368, -3053, -41, 624 ,6051, 5174, -684, 0, 0, 0, 0, 0, 0},
    {24, 1010, 8082, 746, -3261, -62, 731, 680, -4088, 3137, 0, 0, 0, 0, 0, 0},
    {16, 967, -227, -1969, 4754, -103, 752, -2086, -5136, 677, 0, 0, 0, 0, 0, 0},
    {-41, 1656, -227, -4083, 3132, 4, 1633, -227, -1114, 5023, 0, 0, 0, 0, 0, 0},
    {-74, 451, 8082, 1937, -2727, 131, 752, -1141, -1553, -4924, 0, 0, 0, 0, 0, 0}};

void parse_command_line(int argc, char *argv[]) {

    int c;

    while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"input_path", required_argument, 0, 'i'},
		  {"output_path", required_argument, 0, 'o'},
          {"numEvents", required_argument, 0, 'n'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "i:o:n:", long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
            case 'i':
                input_path = (char*) optarg;
                break;

            case 'o':
                output_path = (char*) optarg;
                break;

            case 'n':
                numEvents = (atoi(optarg) < MAX_EVENTS) ? atoi(optarg) : MAX_EVENTS;
                break;

            default:
                abort();
        }
    }
}

int main(int argc, char *argv[])
{

    parse_command_line(argc,argv);

    #if !defined(__HLS_COSIM__)
        HighFive::File file(input_path, HighFive::File::ReadOnly);
        
        auto num_nodes_dataset = file.getDataSet("mask_graphs");
        auto num_nodes_data = num_nodes_dataset.read<std::vector<int16_t>>();

        int available_events = num_nodes_data.size();
        if(numEvents > available_events) {
            std::cout << "You requested to test " << numEvents << " but there are only " << available_events << " available inside the provided dataset. Truncating." << std::endl;
            numEvents = available_events;
        }

        auto feature_dataset = file.getDataSet("feat_graphs");
        auto input_features_data = feature_dataset.read<std::vector<std::vector<std::vector<int16_t>>>>();

        int* inputNumList = (int*) calloc(numEvents, sizeof(int));
        for(int e = 0; e < numEvents; e++) {
            inputNumList[e] = num_nodes_data[e];
        }

        int* outputNumList = (int*) calloc(numEvents, sizeof(int));
        for(int e = 0; e < numEvents; e++) {
            outputNumList[e] = num_nodes_data[e];
        }

        in_harness_t* inFeatureList = (in_harness_t*) calloc(numEvents*II, sizeof(in_harness_t));
        for(int e = 0; e < numEvents*II; e++) {
            in_harness_t burst;
            for(int p = 0; p < PAR; p++) {
                for(int f = 0; f < MODEL_INPUT_WIDTH; f++) {
                    float requant = static_cast<float>(input_features_data[e / II][(e % II)*PAR + p][f]) / pow(2,model_input_t::width-model_input_t::iwidth);
                    burst[p*MODEL_INPUT_WIDTH+f] = static_cast<model_input_t>(requant);
                }
            }
            inFeatureList[e] = burst;
        }
    #else 
        int* inputNumList = (int*) calloc(MAX_EVENTS, sizeof(int));
        inputNumList[0] = 12;

        int* outputNumList = (int*) calloc(MAX_EVENTS, sizeof(int));
        outputNumList[0] = 12;

        in_harness_t* inFeatureList = (in_harness_t*) calloc(MAX_EVENTS*II, sizeof(in_harness_t));
        for(int e = 0; e < 6; e++) {
            in_harness_t burst;
            for(int i = 0; i < 16; i++) {
                burst[i] = static_cast<model_input_t>(static_cast<float>(stimuli[e][i]) / pow(2,model_input_t::width-model_input_t::iwidth));
            }
            inFeatureList[e] = burst;
        }
    #endif


    hls::stream<int> numStream;
    hls::stream<array<model_input_t,MODEL_INPUT_WIDTH>> inputStream[PAR];

    hls::stream<array<model_output_t,MODEL_OUTPUT_WIDTH>> outputStream[PAR];
    hls::stream<bool> lastStream[PAR];

    int  betaThreshold = 205;
    int  isolationThreshold = 77;

    out_harness_t* outFeatureList = (out_harness_t*) calloc(MAX_EVENTS*II, sizeof(out_harness_t));

    harness(numEvents,betaThreshold,isolationThreshold,inputNumList,inFeatureList,outputNumList,outFeatureList);

    #if !defined(__HLS_COSIM__)
        vector<vector<vector<double>>> output(numEvents, vector<vector<double>>(N, vector<double>(MODEL_OUTPUT_WIDTH, 0.0)));

        for(int e = 0; e < numEvents*II; e++) {
            int n = num_nodes_data[e / II];
            out_harness_t burst = outFeatureList[e];
            for(int p = 0; p < PAR; p++) {
                for(int f = 0; f < MODEL_OUTPUT_WIDTH; f++) {
                    if((e % II)*PAR + p < n) {
                        int16_t value = burst[p*MODEL_OUTPUT_WIDTH+f](model_output_t::width-1,0);
                        output[e / II][(e % II)*PAR + p][f] = static_cast<double>(burst[p*MODEL_OUTPUT_WIDTH+f]);
                    }
                }
            }
        }

        HighFive::File output_file(output_path, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
        vector<size_t> dims{static_cast<unsigned long>(numEvents), N, MODEL_OUTPUT_WIDTH};
        HighFive::DataSet output_dataset = output_file.createDataSet<double>("simulation", HighFive::DataSpace(dims));
        output_dataset.write(output);
    #else
        printf("Result: \n");
        for(int e = 0; e < II; e++) {
            out_harness_t burst = outFeatureList[e];
            for(int p = 0; p < PAR; p++) {
                printf("Node %d: ", (e % II)*PAR + p);
                for(int f = 0; f < MODEL_OUTPUT_WIDTH; f++) {
                    if((e % II)*PAR + p < N) {
                        printf(" %1.5f, ", static_cast<double>(burst[p*MODEL_OUTPUT_WIDTH+f]));
                    }
                }
                printf("\n");
            }
        }
    #endif

    return 0;
}