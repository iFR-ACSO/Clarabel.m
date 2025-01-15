% MATLAB script to generate and populate settings struct for Clarabel
function settings = DefaultSettings()
% Initialize settings structure
settings = struct();

% Integer fields
settings.max_iter = uint32(200); 
settings.equilibrate_max_iter = uint32(10); 
settings.iterative_refinement_max_iter = uint32(10); 

% Boolean fields
settings.verbose = true; 
settings.equilibrate_enable = true; 
settings.direct_kkt_solver = true; 
settings.static_regularization_enable = true; 
settings.dynamic_regularization_enable = true; 
settings.iterative_refinement_enable = true; 
settings.presolve_enable = true; 

% #ifdef FEATURE_SDP
settings.chordal_decomposition_enable = true; 
settings.chordal_decomposition_compact = true; 
settings.chordal_decomposition_complete_dual = true; 
% #endif

% Double fields
settings.time_limit = Inf; 
settings.max_step_fraction = 0.99; 
settings.tol_gap_abs = 1e-8; 
settings.tol_gap_rel = 1e-8; 
settings.tol_feas = 1e-8; 
settings.tol_infeas_abs = 1e-8; 
settings.tol_infeas_rel = 1e-8; 
settings.tol_ktratio = 1e-6; 
settings.reduced_tol_gap_abs = 5e-5; 
settings.reduced_tol_gap_rel = 5e-5; 
settings.reduced_tol_feas = 1e-4; 
settings.reduced_tol_infeas_abs = 5e-5; 
settings.reduced_tol_infeas_rel = 5e-5; 
settings.reduced_tol_ktratio = 1e-4; 
settings.equilibrate_min_scaling = 1e-4; 
settings.equilibrate_max_scaling = 1e4; 
settings.linesearch_backtrack_step = 0.8; 
settings.min_switch_step_length = 1e-1; 
settings.min_terminate_step_length = 1e-4; 
settings.static_regularization_constant = 1e-8; 
settings.static_regularization_proportional = eps^2; 
settings.dynamic_regularization_eps = 1e-13; 
settings.dynamic_regularization_delta = 2e-7; 
settings.iterative_refinement_reltol = 1e-13; 
settings.iterative_refinement_abstol = 1e-12; 
settings.iterative_refinement_stop_ratio = 5.0; 

% Enum fields
settings.direct_solve_method = 'qldl'; % Default enum value
% #ifdef FEATURE_SDP
settings.chordal_decomposition_merge_method = 'clique_graph'; 
% #endif

end