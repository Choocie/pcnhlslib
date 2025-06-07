#include "host.h"

/* Hardware Parameters */

std::string xclbin_path ("kernel.xclbin");
std::string input_path  ("input.h5");
std::string output_path ("output.h5");
static int num_events = 1;
static int verbose    = 0;
static int benchmark  = 0;
static int fused      = 0;
void parse_command_line(int argc, char *argv[]) {

    int c;

    while (1)
    {
      static struct option long_options[] =
        {
          {"verbose", no_argument, &verbose, 1},
          {"benchmark", no_argument, &benchmark, 1},
          {"xclbin_path", required_argument, 0, 'f'},
          {"num_events", required_argument, 0, 'n'},
          {"input_path", required_argument, 0, 'i'},
          {"output_path", required_argument, 0, 'o'},
          {"fused", no_argument, &fused, 1},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "f:n:i:o:", long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
            case 0:
                if (long_options[option_index].flag != 0)
                break;
  
            case 'f':
                xclbin_path = (char*) optarg;
                break;

            case 'i':
                input_path = (char*) optarg;
                break;

            case 'o':
                output_path = (char*) optarg;
                break;

            case 'n':
                num_events = (atoi(optarg) < MAX_EVENTS) ? atoi(optarg) : MAX_EVENTS;
                break;

            default:
                abort();
        }
    }
}

int run_separate() {
    HighFive::File file(input_path, HighFive::File::ReadOnly);
    auto dataset = file.getDataSet("mask_graphs");
    auto num_data = dataset.read<std::vector<int16_t>>();

    int available_events = num_data.size();
    if(benchmark) {
        std::cout << "You requested to benchmark. Input data is randomized." << std::endl;
    } else if(num_events > available_events) {
        std::cout << "You requested to test " << num_events << " but there are only " << available_events << " available inside the provided dataset. Truncating." << std::endl;
        num_events = available_events;
    }

    auto device = xrt::device(0);

    auto device_name = device.get_info<xrt::info::device::name>();

    std::cout << "[USR] " << "INFO: " << "found device " << device_name << std::endl;

    auto uuid = device.load_xclbin(xclbin_path);
    std::cout << "[USR] " << "INFO: " << "succesfully loaded xclbin " << xclbin_path << std::endl;

    auto loadkrnl  = xrt::kernel(device, uuid, "load");
    std::cout << "[USR] " << "INFO: " << "found kernel " << "load" << " on " << xclbin_path << std::endl;
    auto ccnkrnl   = xrt::kernel(device, uuid, "caloclusternet");
    std::cout << "[USR] " << "INFO: " << "found kernel " << "caloclusternet" << " on " << xclbin_path << std::endl;
    auto storekrnl = xrt::kernel(device, uuid, "store");
    std::cout << "[USR] " << "INFO: " << "found kernel " << "store" << " on " << xclbin_path << std::endl;    

    std::cout << "[USR] " << "INFO: " << "allocate Buffers in Global Memory\n";
    auto inputNumList = xrt::bo(device, sizeof(int) * MAX_EVENTS, loadkrnl.group_id(1));
    auto inputFeatureList = xrt::bo(device, sizeof(in_harness_t) * II * num_events, loadkrnl.group_id(2));
    auto outputNumList = xrt::bo(device, sizeof(int) * MAX_EVENTS, storekrnl.group_id(1));
    auto outputFeatureList = xrt::bo(device, sizeof(out_harness_t) * II * num_events, storekrnl.group_id(2));

    std::cout << "[USR] " << "INFO: " << " load number of events\n";
    auto inputNumListMap = inputNumList.map<int*>();
    if(benchmark) {
        for(int e = 0; e < num_events; e++) {        
            inputNumListMap[e] = N;
        }
    } else {
        for(int e = 0; e < num_events; e++) {        
            if(verbose) printf("Event %d consists of %d points.\n",e,num_data[e]);   
            inputNumListMap[e] = num_data[e];
        }
    }

    auto outputNumListMap = outputNumList.map<int*>();
    if(benchmark) {
        for(int e = 0; e < num_events; e++) {     
            outputNumListMap[e] = N;
        }
    } else {
        for(int e = 0; e < num_events; e++) {     
            if(verbose) printf("Event %d consists of %d points.\n",e,num_data[e]);   
            outputNumListMap[e] = num_data[e];
        }
    }

    std::cout << "[USR] " << "INFO: " << " load input features\n";
    auto inputFeatureListMap = inputFeatureList.map<in_harness_t*>();
    if(!benchmark) {
        HighFive::File file(input_path, HighFive::File::ReadOnly);
        auto dataset = file.getDataSet("feat_graphs");
        auto data = dataset.read<std::vector<std::vector<std::vector<int16_t>>>>();
        for(int e = 0; e < num_events*II; e++) {
            if(e % II == 0) if(verbose) printf("Event %d \n",e / II);   
            in_harness_t burst;
            for(int p = 0; p < PAR; p++) {
                for(int f = 0; f < MODEL_INPUT_WIDTH; f++) {
                    if(verbose) printf("%d, ",data[e / II][(e % II)*PAR + p][f]);   
                    burst[p*MODEL_INPUT_WIDTH+f] = data[e / II][(e % II)*PAR + p][f];
                }
            if(verbose) printf("\n");
            }
            inputFeatureListMap[e] = burst;
        }
    }

    std::cout << "[USR] " << "INFO: " << "input transfer from hard drive to system memory complete" << std::endl;;

    auto outputFeatureListMap = outputFeatureList.map<out_harness_t*>();
    for(int i = 0; i < num_events*II; i++) {
        out_harness_t new_node = {0};
        outputFeatureListMap[i] = new_node;
    }

    // Synchronize buffer content with device side
    std::cout << "[USR] " << "INFO: " << "synchronize input buffer data to device global memory\n";
    inputNumList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    inputFeatureList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    outputNumList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    outputFeatureList.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "[USR] " << "INFO: " << "set load arguments\n";
    auto loadrun = xrt::run(loadkrnl);
    loadrun.set_arg(0,num_events);
    loadrun.set_arg(1,inputNumList);
    loadrun.set_arg(2,inputFeatureList);

    std::cout << "[USR] " << "INFO: " << "set caloclusternet arguments\n";
    auto ccnrun = xrt::run(ccnkrnl);
    ccnrun.set_arg(0,num_events);
    ccnrun.set_arg(5, 205); // === 0.1 for <16,5>
    ccnrun.set_arg(6, 77); // === 0.3 for <20,12>

    std::cout << "[USR] " << "INFO: " << "set store arguments\n";
    auto storerun = xrt::run(storekrnl);
    storerun.set_arg(0,num_events);
    storerun.set_arg(1,outputNumList);
    storerun.set_arg(2,outputFeatureList);

    std::cout << "[USR] " << "INFO: " <<"start all kernels" <<  "\n";
    loadrun.start();
    storerun.start();
    ccnrun.start();

    std::cout << "[USR] " << "INFO: " << "wait on completion for load...\n";
    loadrun.wait();
    std::cout << "[USR] " << "INFO: " << "wait on completion for caloclusternet...\n";
    ccnrun.wait();
    std::cout << "[USR] " << "INFO: " << "wait on completion for store...\n";
    storerun.wait();

    std::cout << "[USR] " << "INFO: " << "transfer from device memory to system memory" << std::endl;

    outputFeatureList.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    std::vector<std::vector<std::vector<double>>> output_data(num_events, std::vector<std::vector<double>>(N, std::vector<double>(MODEL_OUTPUT_WIDTH, 0.0)));

    for(int e = 0; e < num_events*II; e++) {
        out_harness_t burst = outputFeatureListMap[e];
        if(e % II == 0) if(verbose) printf("Event %d \n",e / II);   
        for(int p = 0; p < PAR; p++) {
            for(int f = 0; f < MODEL_OUTPUT_WIDTH; f++) {
                if(verbose) printf("%d, ",burst[p*MODEL_OUTPUT_WIDTH+f]);   
                output_data[e / II][(e % II)*PAR + p][f] = static_cast<double>(burst[p*MODEL_OUTPUT_WIDTH+f]) / pow(2,11);
            }
            if(verbose) printf("\n");
        }
    }

    HighFive::File output_file(output_path, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
    std::vector<size_t> dims{static_cast<unsigned long>(num_events), N, MODEL_OUTPUT_WIDTH};
    HighFive::DataSet output_dataset = output_file.createDataSet<double>("simulation", HighFive::DataSpace(dims));
    output_dataset.write(output_data);

    std::cout << "[USR] " << "INFO: " << "SUCCESS\n";
    return 0;
}

int run_fused() {
    HighFive::File file(input_path, HighFive::File::ReadOnly);
    auto dataset = file.getDataSet("mask_graphs");
    auto num_data = dataset.read<std::vector<int16_t>>();

    int available_events = num_data.size();
    if(benchmark) {
        std::cout << "You requested to benchmark. Input data is randomized." << std::endl;
    } else if(num_events > available_events) {
        std::cout << "You requested to test " << num_events << " but there are only " << available_events << " available inside the provided dataset. Truncating." << std::endl;
        num_events = available_events;
    }

    auto device = xrt::device(0);

    auto device_name = device.get_info<xrt::info::device::name>();

    std::cout << "[USR] " << "INFO: " << "found device " << device_name << std::endl;

    auto uuid = device.load_xclbin(xclbin_path);
    std::cout << "[USR] " << "INFO: " << "succesfully loaded xclbin " << xclbin_path << std::endl;

    auto krnl   = xrt::kernel(device, uuid, "harness");
    std::cout << "[USR] " << "INFO: " << "found kernel " << "harness" << " on " << xclbin_path << std::endl;

    std::cout << "[USR] " << "INFO: " << "allocate Buffers in Global Memory\n";
    auto inputNumList = xrt::bo(device, sizeof(int) * MAX_EVENTS, krnl.group_id(3));
    auto inputFeatureList = xrt::bo(device, sizeof(in_harness_t) * II * num_events, krnl.group_id(4));
    auto outputNumList = xrt::bo(device, sizeof(int) * MAX_EVENTS, krnl.group_id(5));
    auto outputFeatureList = xrt::bo(device, sizeof(out_harness_t) * II * num_events, krnl.group_id(6));

    std::cout << "[USR] " << "INFO: " << " load number of events\n";
    auto inputNumListMap = inputNumList.map<int*>();
    if(benchmark) {
        for(int e = 0; e < num_events; e++) {        
            inputNumListMap[e] = N;
        }
    } else {
        for(int e = 0; e < num_events; e++) {        
            if(verbose) printf("Event %d consists of %d points.\n",e,num_data[e]);   
            inputNumListMap[e] = num_data[e];
        }
    }

    auto outputNumListMap = outputNumList.map<int*>();
    if(benchmark) {
        for(int e = 0; e < num_events; e++) {     
            outputNumListMap[e] = N;
        }
    } else {
        for(int e = 0; e < num_events; e++) {     
            if(verbose) printf("Event %d consists of %d points.\n",e,num_data[e]);   
            outputNumListMap[e] = num_data[e];
        }
    }

    std::cout << "[USR] " << "INFO: " << " load input features\n";
    auto inputFeatureListMap = inputFeatureList.map<in_harness_t*>();
    if(!benchmark) {
        HighFive::File file(input_path, HighFive::File::ReadOnly);
        auto dataset = file.getDataSet("feat_graphs");
        auto data = dataset.read<std::vector<std::vector<std::vector<int16_t>>>>();
        for(int e = 0; e < num_events*II; e++) {
            if(e % II == 0) if(verbose) printf("Event %d \n",e / II);   
            in_harness_t burst;
            for(int p = 0; p < PAR; p++) {
                for(int f = 0; f < MODEL_INPUT_WIDTH; f++) {
                    if(verbose) printf("%d, ",data[e / II][(e % II)*PAR + p][f]);   
                    burst[p*MODEL_INPUT_WIDTH+f] = data[e / II][(e % II)*PAR + p][f];
                }
            if(verbose) printf("\n");
            }
            inputFeatureListMap[e] = burst;
        }
    }

    std::cout << "[USR] " << "INFO: " << "input transfer from hard drive to system memory complete" << std::endl;;

    auto outputFeatureListMap = outputFeatureList.map<out_harness_t*>();
    for(int i = 0; i < num_events*II; i++) {
        out_harness_t new_node = {0};
        outputFeatureListMap[i] = new_node;
    }

    // Synchronize buffer content with device side
    std::cout << "[USR] " << "INFO: " << "synchronize input buffer data to device global memory\n";
    inputNumList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    inputFeatureList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    outputNumList.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    outputFeatureList.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "[USR] " << "INFO: " << "set harness arguments\n";
    auto krnlrun = xrt::run(krnl);
    krnlrun.set_arg(0,num_events);
    krnlrun.set_arg(1, 205); // === 0.1 for <16,5>
    krnlrun.set_arg(2, 77); // === 0.3 for <20,12>

    std::cout << "[USR] " << "INFO: " <<"start all kernels" <<  "\n";
    krnlrun.start();

    std::cout << "[USR] " << "INFO: " << "wait on completion for caloclusternet...\n";
    krnlrun.wait(10000);

    std::cout << "[USR] " << "INFO: " << "transfer from device memory to system memory" << std::endl;

    outputFeatureList.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    std::vector<std::vector<std::vector<double>>> output_data(num_events, std::vector<std::vector<double>>(N, std::vector<double>(MODEL_OUTPUT_WIDTH, 0.0)));

    for(int e = 0; e < num_events*II; e++) {
        out_harness_t burst = outputFeatureListMap[e];
        if(e % II == 0) if(verbose) printf("Event %d \n",e / II);   
        for(int p = 0; p < PAR; p++) {
            for(int f = 0; f < MODEL_OUTPUT_WIDTH; f++) {
                if(verbose) printf("%d, ",burst[p*MODEL_OUTPUT_WIDTH+f]);   
                output_data[e / II][(e % II)*PAR + p][f] = static_cast<double>(burst[p*MODEL_OUTPUT_WIDTH+f]) / pow(2,11);
            }
            if(verbose) printf("\n");
        }
    }

    HighFive::File output_file(output_path, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
    std::vector<size_t> dims{static_cast<unsigned long>(num_events), N, MODEL_OUTPUT_WIDTH};
    HighFive::DataSet output_dataset = output_file.createDataSet<double>("simulation", HighFive::DataSpace(dims));
    output_dataset.write(output_data);

    std::cout << "[USR] " << "INFO: " << "SUCCESS\n";
    return 0;
}

int main(int argc, char* argv[]) {

    parse_command_line(argc,argv);

    int result = 0;
    if(fused) {
        result = run_fused();
    } else {
        result = run_separate();
    }

    return result;
}
