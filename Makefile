export LD_LIBRARY_PATH += :${VITIS_HLS_ROOT}/lib/lnx64.o:${VITIS_HLS_ROOT}/lib/lnx64.o/Rhel/8

VITIS_HLS_ROOT := /tools/xilinx/Vitis_HLS/2024.2

BUILD_ROOT 	     := build
BIN_ROOT         := bin
TEST_ROOT        := $(abspath ./unit_tests)
TEMPLATE_ROOT    := $(abspath ./templates)
TEMPLATE_SRCS    := ${wildcard $(TEMPLATE_ROOT)/*.h}

TARGET_SIM    := $(shell find ./unit_tests -mindepth 1 -type d | sed 's|^./unit_tests/||')
MAIN_OBJ_SIM  := $(foreach target,$(TARGET_SIM),build/$(target)/main.o)
DUT_OBJ_SIM   := $(foreach target,$(TARGET_SIM),build/$(target)/dut.o)

CXX_SYNTH := v++ 

IFLAG_SIM += -g
IFLAG_SIM += -I "${TEMPLATE_ROOT}/layers" -I "${TEMPLATE_ROOT}/models"
IFLAG_SIM += -I "${VITIS_HLS_ROOT}/include"
IFLAG_SIM += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__
IFLAG_SIM += -L"${VITIS_HLS_ROOT}/lnx64/lib/csim" -Wl,-rpath,"${VITIS_HLS_ROOT}/lnx64/lib/csim" 
IFLAG_SIM += -L"${VITIS_HLS_ROOT}/lnx64/tools/fpo_v7_1" -Wl,-rpath,"${VITIS_HLS_ROOT}/lnx64/tools/fpo_v7_1"
IFLAG_SIM += -fuse-ld=lld -lm -lpthread
IFLAG_SIM += -L"$(HDF5_LIB)/lib" -lhdf5 -Wl,-rpath,"$(HDF5_LIB)/lib"

CFLAG_SIM += -g
CFLAG_SIM += -I "${TEMPLATE_ROOT}/layers" -I "${TEMPLATE_ROOT}/models"
CFLAG_SIM += -I "${VITIS_HLS_ROOT}/include"
CFLAG_SIM += -I "$(HDF5_LIB)/include"
CFLAG_SIM += -I "$(HIGHFIVE_LIB)/include"
CFLAG_SIM += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__

CFLAG_SIM += -fPIC -fPIE -Wno-unused-result
CFLAG_SIM += --gcc-toolchain=/tools/xilinx/Vitis_HLS/2024.1/tps/lnx64/gcc-8.3.0

CXX_SIM = ${VITIS_HLS_ROOT}/vcxx/libexec/clang++

.PHONY: clean

clean:
	rm -f ${BIN_ROOT}/*
	rm -f ${BUILD_ROOT}/*

#Unit Tests
$(MAIN_OBJ_SIM): ${BUILD_ROOT}/%/main.o: ${TEST_ROOT}/%/main.cpp $(TEMPLATE_SRCS)
	mkdir -p ${BUILD_ROOT}/$*
	$(CXX_SIM) -c $< -o ${BUILD_ROOT}/$*/main.o $(CFLAG_SIM)

$(DUT_OBJ_SIM): ${BUILD_ROOT}/%/dut.o: ${TEST_ROOT}/%/dut.cpp ${TEST_ROOT}/%/dut.h $(TEMPLATE_SRCS)
	mkdir -p ${BUILD_ROOT}/$*
	$(CXX_SIM) -c $< -o ${BUILD_ROOT}/$*/dut.o $(CFLAG_SIM)

$(TARGET_SIM): %: ${BUILD_ROOT}/%/main.o ${BUILD_ROOT}/%/dut.o
	mkdir -p ${BUILD_ROOT}/bin
	$(CXX_SIM) $^ -o ${BIN_ROOT}/test_$* $(IFLAG_SIM)

all: $(TARGET_SIM)