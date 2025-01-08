# clarabel_mex
MATLAB mex interface to Clarabel, an interior point solver for convex optimization.

# Description
This repository contains a (work in progress) MATLAB mex interface to the recently introduced  interior point solver Clarabel (https://clarabel.org/stable/). The mex interface is interfaced to Clarabel.cpp (https://github.com/oxfordcontrol/Clarabel.cpp/tree/main) , the C/C++ interface to the Rust implementation of Clarabel.
The interface to the C++ implementation via eigen (https://eigen.tuxfamily.org/index.php?title=Main_Page). 

# Installation
A more detail description will follow soon. Also some files are missing. The major step for installation:
1. Clone Clarabel.cpp repository, install according to description (get submodules, get compiler,...) and build the project to get libraries.
2. Put libraries in same folder, where make_clarabel.m lies
3. Execute make_clarabel.m to generate the mex file
4. After mex file is generated, that the clarabel_c.dll lies in the same folder as the mex function
    

