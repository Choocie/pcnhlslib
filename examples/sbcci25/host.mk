BUILD_ROOT ?= build

HIGHFIVE_LIB   := $$HOME/bin/highfive-2_8_0


${BUILD_ROOT}/host.o: src/host.cpp  src/host.h
	mkdir -p ${BUILD_ROOT}
	${CXX} -std=c++17 -I${SDKTARGETSYSROOT}/usr/include/xrt/ -g -Wall -c -fmessage-length=0 -o ${BUILD_ROOT}/host.o  src/host.cpp -I "$(HIGHFIVE_LIB)/include"

$(BUILD_ROOT)/host.exe: $(BUILD_ROOT)/host.o
	mkdir -p ${BUILD_ROOT}
	$(CXX) -lxrt_coreutil -o $(BUILD_ROOT)/host.exe $(BUILD_ROOT)/host.o -lhdf5


host: $(BUILD_ROOT)/host.exe

clean:
	rm -rf ${BUILD_ROOT}

.PHONY: clean host