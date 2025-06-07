export LD_LIBRARY_PATH += :${VITIS_HLS_ROOT}/lib/lnx64.o:${VITIS_HLS_ROOT}/lib/lnx64.o/Rhel/8

NUM_POINTS ?= 32
INITIATION_INTERVAL ?= 16
RESOLUTION ?= 8

HDF5_LIB       := $$HOME/bin/hdf5-1_14_0
HIGHFIVE_LIB   := $$HOME/bin/highfive-2_8_0
VITIS_HLS_ROOT := /tools/xilinx/Vitis/2024.2

BUILD_ROOT 	         ?= build
LAYER_TEMPLATE_ROOT  := $(abspath ../../layers)
MODEL_TEMPLATE_ROOT  := $(abspath ../../models)
SRC_ROOT		     := $(abspath ./src)

CXX_SYNTH := v++ 

IFLAG_SIM += -g
IFLAG_SIM += -I "${LAYER_TEMPLATE_ROOT}" -I "${MODEL_TEMPLATE_ROOT}" 
IFLAG_SIM += -I "${VITIS_HLS_ROOT}/include"
IFLAG_SIM += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__
IFLAG_SIM += -L"${VITIS_HLS_ROOT}/lnx64/lib/csim" -Wl,-rpath,"${VITIS_HLS_ROOT}/lnx64/lib/csim" -lhlsmc++-CLANG39
IFLAG_SIM += -L"${VITIS_HLS_ROOT}/lnx64/tools/fpo_v7_1" -Wl,-rpath,"${VITIS_HLS_ROOT}/lnx64/tools/fpo_v7_1" -lIp_floating_point_v7_1_bitacc_cmodel -lgmp -lmpfr
IFLAG_SIM += -fuse-ld=lld -lm -lpthread
IFLAG_SIM += -L"$(HDF5_LIB)/lib" -lhdf5 -Wl,-rpath,"$(HDF5_LIB)/lib"

CFLAG_SIM += -g
CFLAG_SIM += -I "${LAYER_TEMPLATE_ROOT}" -I "${MODEL_TEMPLATE_ROOT}" 
CFLAG_SIM += -I "${VITIS_HLS_ROOT}/include"
CFLAG_SIM += -I "$(HDF5_LIB)/include"
CFLAG_SIM += -I "$(HIGHFIVE_LIB)/include"
CFLAG_SIM += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__
CFLAG_SIM += -D__NUM_POINTS__=$(NUM_POINTS) -D__INITIATION_INTERVAL__=$(INITIATION_INTERVAL) -D__RESOLUTION__=$(RESOLUTION)

CFLAG_SIM += -fPIC -fPIE -Wno-unused-result
CFLAG_SIM += --gcc-toolchain=/tools/xilinx/Vitis/2024.2/tps/lnx64/gcc-8.3.0

CXX_SIM = ${VITIS_HLS_ROOT}/vcxx/libexec/clang++

SYNTH_HZ ?= 350000000
IMPL_HZ  ?= 127000000

LOAD_HLS_CFG           = ../config/load.hls.cfg
CALOCLUSTERNET_HLS_CFG = ../config/caloclusternet.hls.cfg
STORE_HLS_CFG          = ../config/store.hls.cfg
HARNESS_HLS_CFG        = ../config/harness.hls.cfg
LINK_CFG               = ../config/link.cfg
FUSED_LINK_CFG         = ../config/fused.link.cfg
PACKAGE_CFG            = ../config/package.cfg 

TARGET = --platform xilinx_vck190_base_202420_1

.PHONY: clean csim csynth link package all

clean:
	rm -f ${BUILD_ROOT}/*

${BUILD_ROOT}/csim.exe: ${SRC_ROOT}/csim.cpp ${SRC_ROOT}/harness.cpp ${SRC_ROOT}/load.cpp ${SRC_ROOT}/caloclusternet.cpp ${SRC_ROOT}/store.cpp ${SRC_ROOT}/harness.cpp ${MODEL_TEMPLATE_ROOT}/caloclusternetv2.h ${SRC_ROOT}/global.h ${SRC_ROOT}/weights.h
	mkdir -p ${BUILD_ROOT}/csim
	$(CXX_SIM) -c ${SRC_ROOT}/load.cpp -o ${BUILD_ROOT}/csim/load.o $(CFLAG_SIM)
	$(CXX_SIM) -c ${SRC_ROOT}/caloclusternet.cpp -o ${BUILD_ROOT}/csim/caloclusternet.o $(CFLAG_SIM)
	$(CXX_SIM) -c ${SRC_ROOT}/store.cpp -o ${BUILD_ROOT}/csim/store.o $(CFLAG_SIM)
	$(CXX_SIM) -c ${SRC_ROOT}/harness.cpp -o ${BUILD_ROOT}/csim/harness.o $(CFLAG_SIM)
	$(CXX_SIM) -c ${SRC_ROOT}/csim.cpp -o ${BUILD_ROOT}/csim/csim.o $(CFLAG_SIM)
	$(CXX_SIM) ${BUILD_ROOT}/csim/*.o -o ${BUILD_ROOT}/csim.exe $(IFLAG_SIM)

$(BUILD_ROOT)/harness/harness.xo: ${SRC_ROOT}/harness.cpp ${SRC_ROOT}/load.cpp ${SRC_ROOT}/store.cpp ${SRC_ROOT}/caloclusternet.cpp ${SRC_ROOT}/global.h ${SRC_ROOT}/weights.h
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -c --mode hls $(TARGET) --freqhz=${SYNTH_HZ} --config $(HARNESS_HLS_CFG)

$(BUILD_ROOT)/load/load.xo: ${SRC_ROOT}/load.cpp ${SRC_ROOT}/global.h ${SRC_ROOT}/weights.h
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -c --mode hls $(TARGET) --freqhz=${SYNTH_HZ} --config $(LOAD_HLS_CFG)

$(BUILD_ROOT)/store/store.xo: ${SRC_ROOT}/store.cpp ${SRC_ROOT}/global.h ${SRC_ROOT}/weights.h
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -c --mode hls $(TARGET) --freqhz=${SYNTH_HZ} --config $(STORE_HLS_CFG)

$(BUILD_ROOT)/caloclusternet/caloclusternet.xo: ${SRC_ROOT}/caloclusternet.cpp ${SRC_ROOT}/global.h ${SRC_ROOT}/weights.h
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -c --mode hls $(TARGET) --freqhz=${SYNTH_HZ} --config $(CALOCLUSTERNET_HLS_CFG)

$(BUILD_ROOT)/harness.xsa: $(BUILD_ROOT)/load/load.xo $(BUILD_ROOT)/store/store.xo $(BUILD_ROOT)/caloclusternet/caloclusternet.xo
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -l $(TARGET) -t hw load/load.xo store/store.xo caloclusternet/caloclusternet.xo --freqhz=300000000:load_1.ap_clk --freqhz=${IMPL_HZ}:caloclusternet_1.ap_clk --freqhz=300000000:store_1.ap_clk --config $(LINK_CFG) -o harness.xsa

$(BUILD_ROOT)/fused.xsa: $(BUILD_ROOT)/harness/harness.xo
	mkdir -p $(BUILD_ROOT)
	cd $(BUILD_ROOT) && $(CXX_SYNTH) -l $(TARGET) -t hw harness/harness.xo --freqhz=${IMPL_HZ}:harness_1.ap_clk --config $(FUSED_LINK_CFG) -o fused.xsa

$(BUILD_ROOT)/harness.xclbin: $(BUILD_ROOT)/harness.xsa
	cd $(BUILD_ROOT) && v++ --package -t hw $(TARGET) --config $(PACKAGE_CFG) harness.xsa -o harness.xclbin

csim:  	  		$(BUILD_ROOT)/csim.exe
csynth:   		$(BUILD_ROOT)/load/load.xo $(BUILD_ROOT)/store/store.xo $(BUILD_ROOT)/caloclusternet/caloclusternet.xo
csynth-fused:   $(BUILD_ROOT)/harness/harness.xo
link:     		$(BUILD_ROOT)/harness.xsa
link-fused: 	$(BUILD_ROOT)/fused.xsa
package:  		$(BUILD_ROOT)/harness.xclbin

all: package
