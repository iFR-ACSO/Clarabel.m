clc
clear

% Define the source path of the file
sourceFolder = 'C:\path\to\source\folder'; % Replace with the actual path
fileName = 'clarabel_c.lib';

% Define the current folder as the destination
destinationFolder = pwd; % 'pwd' gives the current working directory

% Create the full path for the source file and the destination
sourceFile = fullfile(sourceFolder, fileName);
destinationFile = fullfile(destinationFolder, fileName);

% Copy the file
copyfile(sourceFile, destinationFile);

% Display a success message
disp(['File copied to: ', destinationFile]);


% generate clarabel mex interface
mex -v clarabel_mex.cpp ...
        -DFEATURE_SDP ...       % activate SDP feature
      '-I./eigen' ...
       -I"C:\Users\ac133867\Desktop\clarabel_mex_beta\clarabel_mex\clarabel\include" ...
         "C:\Users\ac133867\Desktop\clarabel_mex_beta\clarabel_mex\clarabel\clarabel_c.dll.lib" ...
         "C:\Users\ac133867\Desktop\clarabel_mex_beta\clarabel_mex\clarabel\clarabel_c.lib"
