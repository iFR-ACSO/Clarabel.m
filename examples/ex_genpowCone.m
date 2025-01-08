clc
clear


% define cost
P = sparse(zeros(6,6));
q = [0., 0., -1., 0., 0., -1. ];

% define constraints
A = sparse([ -1.,  0.,  0.,  0.,  0.,  0.;
         0., -1.,  0.,  0.,  0.,  0.;
         0.,  0., -1.,  0.,  0.,  0.;
         0.,  0.,  0., -1.,  0.,  0.;
         0.,  0.,  0.,  0., -1.,  0.;
         0.,  0.,  0.,  0.,  0., -1.;
         1.,  2.,  0.,  3.,  0.,  0.;
         0.,  0.,  0.,  0.,  1.,  0.]);

b = [0., 0., 0., 0., 0., 0., 3., 1.]';

alpha1 = [0.6,0.4]';
alpha2 =[0.1,0.9]';

cones = [
         GenPowerConeT(alpha1,1);
         GenPowerConeT(alpha2,1);
         zeroConeT(1);
         zeroConeT(1);
         ];

% generate settings
settings = DefaultSettings();


sol = clarabel_mex(P,q,A,b,cones,settings)
