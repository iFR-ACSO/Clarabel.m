clc
clear

% generate clarabel mex interface
mex -v clarabel_mex.cpp  '-I./eigen' -I"C:\Users\ac133867\Desktop\clarabel_mex_beta\Clarabel\include" "C:\Users\ac133867\Desktop\clarabel_mex_beta\Clarabel\clarabel_c.dll.lib" "C:\Users\ac133867\Desktop\clarabel_mex_beta\Clarabel\clarabel_c.lib"
