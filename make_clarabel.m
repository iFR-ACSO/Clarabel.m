clc
clear

% Define the current folder as the destination
destinationFolder = pwd; 

% Define the source path of the file
sourceFolder = '..\..\Clarabel\clarabel\rust_wrapper\target\release'; % Replace with the actual path
fileName = 'clarabel_c.dll';

% Create the full path for the source file and the destination
sourceFile = fullfile(sourceFolder, fileName);
destinationFile = fullfile(destinationFolder, fileName);

% Copy the file
copyfile(sourceFile, destinationFile);

% Display a success message
disp(['File copied to: ', destinationFile]);

% just for display output
pause(1)
disp('');
disp('');

% Set to true to include -DFEATURE_SDP if you want to use the SDP feature
% IMPORTANT: libraries must be compiled with the sdp-feature!
useFeatureSDP = true;     

% Get eigen path from environment variable or change the folder where you
% stored eigen
eigenPath = getenv('EIGEN_PATH'); 
if isempty(eigenPath)
    error('Environment variable EIGEN_PATH is not set.');
else
    disp(['Eigen path is: ', eigenPath]);
end

% just for display output
disp('');
disp('');

pause(1)

% Common paths to clarabel; make sure you first compiled the libraries!
clarabelIncludePath = '..\..\Clarabel\clarabel\include';
clarabelDLLPath = '..\..\Clarabel\clarabel\rust_wrapper\target\release\clarabel_c.dll.lib';
clarabelLibPath = '..\..\Clarabel\clarabel\rust_wrapper\target\release\clarabel_c.lib';

% Construct the mex command
mexCmd = [
    'mex -v clarabel_mex.cpp ' ...
    (useFeatureSDP * '-DFEATURE_SDP ') ... % Conditionally add -DFEATURE_SDP
    '-I"' eigenPath '" ' ... 
    '-I"' clarabelIncludePath '" ' ...
    '"' clarabelDLLPath '" ' ...
    '"' clarabelLibPath '"'
];

%% Execute the mex command
disp('Executing mex command:');
disp(mexCmd);
eval(mexCmd); % Run the constructed mex command

disp('');
disp('');

pause(1)

%% add Clarabel_matlab path and subfolder permanently to matlab path
disp('Adding clarabel_matlab folder and subfolder permanently to your MATLAB path.');
folderToAdd = '../';
addpath(genpath(folderToAdd));
savepath;


disp('Clarabel .mex generation done.');
