clc
clear

% Define the current folder as the destination
destinationFolder = pwd; 

% Define the source path of the file
if ispc
    % Windows paths
    sourceFolder = 'Clarabel\rust_wrapper\target\release';  % Replace with the actual path
    fileName = 'clarabel_c.dll';                            % Windows shared library
elseif isunix
    % Unix-based (Linux/macOS) paths
    sourceFolder = 'Clarabel/rust_wrapper/target/release';  % Unix path with forward slashes
    fileName = 'libclarabel_c.so';                          % Unix shared library (likely .so)
else
    error('Unsupported operating system');
end

% Create the full path for the source file and the destination
sourceFile = fullfile(sourceFolder, fileName);
destinationFile = fullfile(destinationFolder, fileName);

% Copy the file
copyfile(sourceFile, destinationFile);

% Display a success message
disp(['File copied to: ', destinationFile]);

% just for display output
% pause(1)
disp('');
disp('');

% Set to true to include -DFEATURE_SDP if you want to use the SDP feature
% IMPORTANT: libraries must be compiled with the sdp-feature!
useFeatureSDP = true;     

% Get eigen path from environment variable or change the folder where you
% stored eigen
eigenPath = getenv('EIGEN_PATH'); 

% If environment variable is not set, allow the user to manually input the path
if isempty(eigenPath)
    disp('Environment variable EIGEN_PATH is not set.');
    % eigenPath = 'C:/Users/ac133867/Desktop/Clarabel.m/eigen-3.4.0';
    eigenPath = '/home/renato/eigen';
    disp('Use manually inserted path.') 
end

% Check if the path is valid (you could also check if a specific file exists here)
if exist(eigenPath, 'dir') ~= 7
    error(['The specified Eigen path is invalid: ', eigenPath]);
else
    disp(['Eigen path is: ', eigenPath]);
end

% just for display output
disp('');
disp('');

pause(1)

% Common paths to clarabel; make sure you first compiled the libraries!
if ispc
    clarabelIncludePath = 'Clarabel\include';
    clarabelDLLPath = 'Clarabel\rust_wrapper\target\release\clarabel_c.dll.lib';
    clarabelLibPath = 'Clarabel\rust_wrapper\target\release\clarabel_c.lib';


    % Construct the mex command
    mexCmd = [
        'mex -v clarabel_mex.cpp ' ...
        (useFeatureSDP * '-DFEATURE_SDP ') ...          % Conditionally add -DFEATURE_SDP
        (useFeatureSDP * '-DFEATURE_FAER_SPARSE ') ...  % Conditionally add -DFEATURE_SDP
        '-I"' eigenPath '" ' ... 
        '-I"' clarabelIncludePath '" ' ...
        '"' clarabelDLLPath '" ' ...
        '"' clarabelLibPath '"'
    ];

elseif isunix
    clarabelIncludePath = 'Clarabel/include';
    clarabelLibPath = 'Clarabel/rust_wrapper/target/release/libclarabel_c.so';  % Change .dll to .so for Linux
    clarabelRustWrapperPath = 'Clarabel/rust_wrapper/target/release';           % Path to the directory containing the shared library

    % Construct the mex command
    mexCmd = [
        'mex -v clarabel_mex.cpp ' ...
        (useFeatureSDP * '-DFEATURE_SDP ') ...          % Conditionally add -DFEATURE_SDP
        (useFeatureSDP * '-DFEATURE_FAER_SPARSE ') ...  % Conditionally add -DFEATURE_FAER_SPARSE
        '-I"' eigenPath '" ' ... 
        '-I"' clarabelIncludePath '" ' ...
        '-L"' clarabelRustWrapperPath '" ' ...          % Specify library path with -L
        '-lclarabel_c'                                  % Link against the shared library (no "lib" prefix and no ".so" suffix)
    ];
else
    error('Unsupported operating system');
end


%% Execute the mex command
disp('Executing mex command:');
disp(mexCmd);
eval(mexCmd); % Run the constructed mex command

disp('');
disp('');

pause(1)

%% add Clarabel_matlab path and subfolder permanently to matlab path
%disp('Adding clarabel_matlab folder and subfolder permanently to your MATLAB path.');
%folderToAdd = '../';
%addpath(genpath(folderToAdd));
%savepath;


disp('Clarabel .mex generation done.');
