# Build Gordian Placement on Linux

## Description

This project is based on the source code of Gordian Placement ([GitHub Repository](https://github.com/KangliC/Gordian_Placement)) and focuses on building a Linux-compatible version.

## Build Instructions

0. Adjust the paths as mentioned in the [original repository](https://github.com/KangliC/Gordian_Placement).
1. Install LAPACK and BLAS:

   ```sh
   sudo apt update
   sudo apt install -y libblas-dev liblapack-dev
   ```
2. Build the project:

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run and Execution


   ```sh
   ./gordian_linux_build

   ```
    After lauching the process, input the netlist file!
    For example:
    ```sh
    ./build/gordian_linux_build
    input the name of netlist file.(include file name extension)
    ./testcases/custom_netlist2.txt
    ```
