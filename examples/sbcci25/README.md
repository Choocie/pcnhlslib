# SBCCI 2025 PCN HLS Example

## Overview

This folder contains the design used in our SBCCI25 paper. We leverage our library of processing elements suitable for deploying graphg-based point cloud networks on FPGAs.
Currently, we feature a reference implementation of the CaloClusterNet used in the GNN-ETM at Belle II intended for applications in the domain of embedded high-performance, low latency applications.

## C Simulation

### Preqrequisites

This package requires the following dependencies:

- hdf5 >=1.14.0
- HighFive >=2.8.0
- AMD Vitis >=2024.2

We assume that hdf5 and HighFive are installed under `$HOME/bin`. If you choose a different location please adjust the Makefiles or configuration files accordingly.
To perform bit-accurate simulation of the CaloClusterNet, you must first compile the simulation for your system. Please make sure that the AMD Vitis environment is loaded and execute:
```sh
make csim
``` 

Generated binary can be found under `build`

### Run

Our C Simulation provides a suitable interface for data-level verification and testing.
```sh
csim.exe -i <input_path>.h5 -o <output_path>.h5 -n <number_of_events>
```
As an input the simulation requires an hdf5 database with suitable events. 

## Build Flow

The current version of this repository is tested with AMD Vitis 2024.2

### Hardware Synthesis and Implementation Flow

Call `make package` to run the full implementation stack. Call `make csynth`, or `make csynth-fused` respectively, for HLS synthesis only.

There are two versions available:

1. Fused Kernel: Load, Store, and Caloclusternet are fused onto a single kernel for linking process
2. Default Kernel: Load, Store, and Caloclusternet are three kernels for the linking process

**Reason:** To achieve maximum throughput on the VCK190, we must increase the clock frequency of the MAXI Controllers inside the load and store kernels. However, the caloclusternet compute kernel is, depending on the actual configuration, not always routable for frequencies of appreox 300 MHZ.
The easiest solution is thus to seperate the fuction into three kernels. 
**Caveats:** Splitting up the kernel into three individuals blocks makes measuring the end-to-end latency more difficult, as for small batch sizes the scheduling delay of the XRT driver delays computations considerably.

### Building the Host Application for the VCK190

In order to build host applications for VCK190, please source the apropriate SKD beforehand, for example:
`source ~/amd/2024.2/vck190/environment-setup-cortexa72-cortexa53-xilinx-linux`
