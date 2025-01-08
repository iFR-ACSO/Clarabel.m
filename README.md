# clarabel.m

MATLAB mex interface to Clarabel, an interior point solver for convex optimization.

# Description

This repository contains a (work in progress) MATLAB mex interface to the recently introduced interior point solver Clarabel ([https://clarabel.org/stable/](https://clarabel.org/stable/)). The mex interface is interfaced to Clarabel.cpp ([https://github.com/oxfordcontrol/Clarabel.cpp/tree/main](https://github.com/oxfordcontrol/Clarabel.cpp/tree/main)), the C/C++ interface to the Rust implementation of Clarabel. The interface to the C++ implementation via eigen ([https://eigen.tuxfamily.org/index.php?title=Main\_Page](https://eigen.tuxfamily.org/index.php?title=Main_Page)).

# Installation

A more detailed description will follow soon. Also, some files are missing. The major steps for installation:

1. Clone Clarabel.cpp repository, install according to the description (get submodules, get compiler, etc.), and build the project to get libraries.
2. Put libraries in the same folder where `make_clarabel.m` lies.
3. Execute `make_clarabel.m` to generate the mex file.
4. After the mex file is generated, the `clarabel_c.dll` must lie in the same folder as the mex function.


Ensure you have the necessary mex file and libraries set up correctly before running the example. This example demonstrates basic usage and can be extended for more complex scenarios.

# Usage Example

Below is an usage example to showcase solving a more detailed optimization problem.
The following example simply implements the getting started example from 
https://clarabel.org/stable/python/getting_started_py/
```matlab

% define cost: 1/2 x^T P x + q^T x
P = sparse(triu([ 3., 1., -1.;
                  1., 4.,  2.;
                 -1., 2.,  5.]));


q = [1., 2., -3.];

% define constraints
Aeq = sparse([1 1 -1]);
beq = 1;

Aineq = sparse([0 1 0; 0 0 1]);
bineq = [2;2];


Asoc = -speye(3);
bsoc = zeros(3,1);

% stack constraints
A = [Aeq;Aineq;Asoc];

b = [beq;bineq;bsoc];

% stack cones
cones = [zeroConeT(1);
         NonnegativeConeT(2);
         SecondOrderConeT(3)];

% generate settings
settings = DefaultSettings();

settings.verbose = false;
settings.time_limit = 5.;

% call interface
sol = clarabel_mex(P,q,A,b,cones,settings)

```

The order of the cones can be arbitrary similar to the Python interface.
