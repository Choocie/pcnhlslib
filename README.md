# Real-Time Graph-based Point Cloud Networks on FPGAs via Stall-Free Deep Pipelining

This library contains architecture templates for deploying point cloud networks on FPGAs via ADM Vitis HLS.
Templates in this version are tested against AMD Vitis 2024.2

## Structure 

We provide HLS architecture templates for common graph-based Point Cloud Network operators. You can find all avaailable operators in the folder `layers`. We also provide some example models that utilize these layers under `models`.

## Examples

### SBCCI 2025 

To make use of our library, we currently provide an example application for point cloud inference on FPGA. You may find a detailed a more detailed explanation in the respective 
In our paper, we provide an example application for point cloud inference, demonstrating how to leverage our library.

## Unit Tests

Unit tests for developed kernels are available under `unit_tests`.
You may build all unit test through `make all`.

## Citation

If you find this repository useful, please cite our work:

```
TBD
```