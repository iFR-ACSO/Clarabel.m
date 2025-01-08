clc
clear


% define cost
P = sparse(zeros(3,3));
q = [-1.0, 0.0, 0.0];

% define constraints
A = sparse([ -1.,  0.,  0.;
         0., -1.,  0.;
         0.,  0., -1.;
         0.,  1.,  0.;
         0.,  0.,  1.]);

b = [ 0., 0., 0., 1., exp(5.0)]';

cones = [
         exponentialConeT();
         zeroConeT(2);
         ];

% generate settings
settings = DefaultSettings();


sol = clarabel_mex(P,q,A,b,cones,settings)
