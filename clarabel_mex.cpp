
#include "mex.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Clarabel>
#include <cstdint>
#include <cmath>
#include <vector>
#include <iostream>


using namespace clarabel;
using namespace std;
using namespace Eigen;
typedef std::uintptr_t uintptr_t;


// Function to convert mxArray to Eigen::SparseMatrix
Eigen::SparseMatrix<double> mxArrayToSparseMatrix(const mxArray *array) {

    mwIndex *ir = mxGetIr(array);
    mwIndex *jc = mxGetJc(array);
    double *pr  = mxGetPr(array);
    mwSize rows = mxGetM(array);
    mwSize cols = mxGetN(array);
    mwSize nnz  = jc[cols];

    Eigen::SparseMatrix<double> mat(rows, cols);
    mat.reserve(nnz);

    for (mwSize col = 0; col < cols; ++col) {
        for (mwSize idx = jc[col]; idx < jc[col + 1]; ++idx) {
            mat.insert(ir[idx], col) = pr[idx];
        }
    }

    mat.makeCompressed();
    return mat;
}

// The actual mex function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    // Check for proper number of arguments (dimensions of the inputs )
    if (nrhs != 6) {
        mexErrMsgIdAndTxt("MATLAB:eigen_test:invalidNumInputs", "Four input matrices required.");
    }

    if (nlhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:eigen_test:invalidNumOutputs", "Four outputs required.");
    }

    // Get the input matrices/vectors
    Eigen::SparseMatrix<double> P;
    Eigen::SparseMatrix<double> A;
    Eigen::VectorXd b;
    Eigen::VectorXd q;

    // check if the input matrices are sparse or dense; in the latter case, convert them to sparse
    if (mxIsSparse(prhs[0])) {
        P = mxArrayToSparseMatrix(prhs[0]);
    } else {
        Eigen::Map<Eigen::MatrixXd> P_dense(mxGetPr(prhs[0]), mxGetM(prhs[0]), mxGetN(prhs[0]));
        P = P_dense.sparseView();
        P.makeCompressed();
    }

     q = Eigen::Map<Eigen::VectorXd>(mxGetPr(prhs[1]), mxGetM(prhs[1]) * mxGetN(prhs[1]), 1);

    if (mxIsSparse(prhs[2])) {
        A = mxArrayToSparseMatrix(prhs[2]);
    } else {
        Eigen::Map<Eigen::MatrixXd> A_dense(mxGetPr(prhs[2]), mxGetM(prhs[2]), mxGetN(prhs[2]));
        A = A_dense.sparseView();
        A.makeCompressed();
    }

    // get the vectors
    b = Eigen::Map<Eigen::VectorXd>(mxGetPr(prhs[3]), mxGetM(prhs[3]) * mxGetN(prhs[3]), 1);


	// Get the cones struct
	const mxArray *conesStruct = prhs[4];
	
	// Check if the cones input is a struct
	if (!mxIsStruct(conesStruct)) {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Fifth input must be a struct.");
	}
	
	// Get the number of entries in the cones struct
	mwSize numCones = mxGetNumberOfElements(conesStruct);
	
	// Initialize the cones for clarabel
	std::vector<SupportedConeT<double>> cones;
	
	// Loop over the entries in the cones struct
	for (mwSize i = 0; i < numCones; ++i) {
		// Get the cone name
		mxArray *coneNameArray = mxGetField(conesStruct, i, "coneName");
		if (!coneNameArray) {
			mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Each entry in the cones struct must have a 'coneName' field.");
		}
	
		char *coneName = mxArrayToString(coneNameArray);
	
		// Get the cone dimension
		mxArray *dimArray = mxGetField(conesStruct, i, "dim");
		if (!dimArray) {
			mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Each entry in the cones struct must have a 'dim' field.");
		}
		
		// Add the cone to the vector based on the cone name
		if (strcmp(coneName, "ZeroCone") == 0) {
			uintptr_t dim = static_cast<uintptr_t>(mxGetScalar(dimArray));
			if (dim > 0) {
				cones.push_back(ZeroConeT<double>(dim));
			}
		} else if (strcmp(coneName, "NonnegativeCone") == 0) {
			uintptr_t dim = static_cast<uintptr_t>(mxGetScalar(dimArray));
			if (dim > 0) {
				cones.push_back(NonnegativeConeT<double>(dim));
			}
		} else if (strcmp(coneName, "SecondOrderCone") == 0) {
			uintptr_t dim = static_cast<uintptr_t>(mxGetScalar(dimArray));
			if (dim > 0) {
				cones.push_back(SecondOrderConeT<double>(dim));
			}
		}  else if (strcmp(coneName, "ExponentialCone") == 0) {
			
				cones.push_back(ExponentialConeT<double>());
		
		} else if (strcmp(coneName, "PowerCone") == 0) {
			double power = mxGetScalar(dimArray);
				cones.push_back(PowerConeT<double>(power ));
		
		} 
		else if (strcmp(coneName, "GenPowerCone") == 0) {
	
			uintptr_t dim = static_cast<uintptr_t>(mxGetScalar(dimArray));
	
			mxArray *alphaArray = mxGetField(conesStruct, i, "alpha");
			if (!alphaArray) {
				mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Each entry in the GenPowerCone struct must have an 'alpha' field.");
			}
			Eigen::Map<Eigen::VectorXd> alpha(mxGetPr(alphaArray), mxGetM(alphaArray) * mxGetN(alphaArray));
	
			cones.push_back(GenPowerConeT<double>(alpha,dim ));
		
		} 
		#ifdef FEATURE_SDP
		    else if (strcmp(coneName, "PSDTriangleCone") == 0) {
			
				uintptr_t dim = static_cast<uintptr_t>(mxGetScalar(dimArray));
				if (dim > 0) {
					cones.push_back(PSDTriangleConeT<double>(dim));
				} 

		}
		#else
		else if (strcmp(coneName, "PSDTriangleCone") == 0) {
			mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "SDP cones are not supported in this build. Activate SDP-feature in CMakeLists.txt and rebuild.");
		}
		#endif
		else {

			mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Unknown cone name: %s", coneName);
		}
	
		// Free the allocated memory for the cone name
		mxFree(coneName);
	}
	
	// Get the user settings struct
	const mxArray *userSettingsStruct = prhs[5];
	
	// Check if the user settings input is a struct
	if (!mxIsStruct(userSettingsStruct)) {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Sixth input must be a struct.");
	}
	
	// Extract the fields from the user settings struct (for readability)
	bool verbose 								= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "verbose")));
	int max_iter 								= static_cast<int>(mxGetScalar(mxGetField(userSettingsStruct, 0, "max_iter")));
	int equilibrate_max_iter					= static_cast<int>(mxGetScalar(mxGetField(userSettingsStruct, 0, "equilibrate_max_iter")));
	int iterative_refinement_max_iter 			= static_cast<int>(mxGetScalar(mxGetField(userSettingsStruct, 0, "iterative_refinement_max_iter")));
	
	bool equilibrate_enable 					= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "equilibrate_enable")));
	bool direct_kkt_solver 						= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "direct_kkt_solver")));
	bool static_regularization_enable 			= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "static_regularization_enable")));
	bool dynamic_regularization_enable 			= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "dynamic_regularization_enable")));
	bool iterative_refinement_enable 			= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "iterative_refinement_enable")));
	bool presolve_enable 						= static_cast<bool>(mxGetScalar(mxGetField(userSettingsStruct, 0, "presolve_enable")));
	ClarabelDirectSolveMethods direct_solve_method = static_cast<ClarabelDirectSolveMethods>(mxGetScalar(mxGetField(userSettingsStruct, 0, "direct_solve_method")));

	double time_limit 							= mxGetScalar(mxGetField(userSettingsStruct, 0, "time_limit"));
	double max_step_fraction 					= mxGetScalar(mxGetField(userSettingsStruct, 0, "max_step_fraction"));
	double tol_gap_abs 							= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_gap_abs"));
	double tol_gap_rel 							= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_gap_rel"));
	double tol_feas 							= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_feas"));
	double tol_infeas_abs 						= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_infeas_abs"));
	double tol_infeas_rel 						= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_infeas_rel"));
	double tol_ktratio 							= mxGetScalar(mxGetField(userSettingsStruct, 0, "tol_ktratio"));
	double reduced_tol_gap_abs 					= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_gap_abs"));
	double reduced_tol_gap_rel 					= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_gap_rel"));
	double reduced_tol_feas 					= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_feas"));
	double reduced_tol_infeas_abs 				= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_infeas_abs"));
	double reduced_tol_infeas_rel 				= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_infeas_rel"));
	double reduced_tol_ktratio 					= mxGetScalar(mxGetField(userSettingsStruct, 0, "reduced_tol_ktratio"));
	double equilibrate_min_scaling 				= mxGetScalar(mxGetField(userSettingsStruct, 0, "equilibrate_min_scaling"));
	double equilibrate_max_scaling 				= mxGetScalar(mxGetField(userSettingsStruct, 0, "equilibrate_max_scaling"));
	double linesearch_backtrack_step 			= mxGetScalar(mxGetField(userSettingsStruct, 0, "linesearch_backtrack_step"));
	double min_switch_step_length 				= mxGetScalar(mxGetField(userSettingsStruct, 0, "min_switch_step_length"));
	double min_terminate_step_length 			= mxGetScalar(mxGetField(userSettingsStruct, 0, "min_terminate_step_length"));
	double static_regularization_constant 		= mxGetScalar(mxGetField(userSettingsStruct, 0, "static_regularization_constant"));
	double static_regularization_proportional 	= mxGetScalar(mxGetField(userSettingsStruct, 0, "static_regularization_proportional"));
	double dynamic_regularization_eps 			= mxGetScalar(mxGetField(userSettingsStruct, 0, "dynamic_regularization_eps"));
	double dynamic_regularization_delta 		= mxGetScalar(mxGetField(userSettingsStruct, 0, "dynamic_regularization_delta"));
	double iterative_refinement_reltol 			= mxGetScalar(mxGetField(userSettingsStruct, 0, "iterative_refinement_reltol"));
	double iterative_refinement_abstol 			= mxGetScalar(mxGetField(userSettingsStruct, 0, "iterative_refinement_abstol"));
	double iterative_refinement_stop_ratio 		= mxGetScalar(mxGetField(userSettingsStruct, 0, "iterative_refinement_stop_ratio"));
		// Get the direct solve method as a string
	mxArray *solveMethodArray = mxGetField(userSettingsStruct, 0, "direct_solve_method");
	if (!solveMethodArray) {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "User settings struct must have a 'direct_solve_method' field.");
	}
	// Get the clique merge method as a string
	mxArray *mergeMethodArray = mxGetField(userSettingsStruct, 0, "chordal_decomposition_merge_method");
	if (!mergeMethodArray) {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "User settings struct must have a 'chordal_decomposition_merge_method' field.");
	}

	// Settings; overwrite the default settings with user input, in case user provides the field; otherwise, the same default value as in default settings "overwrites" the default value
	DefaultSettings<double> settings = DefaultSettingsBuilder<double>::default_settings()
																	.verbose(verbose)
																	.max_iter(max_iter)
																	.equilibrate_max_iter(equilibrate_max_iter)
																	.iterative_refinement_max_iter(iterative_refinement_max_iter)
																	.equilibrate_enable(equilibrate_enable)
																	.direct_kkt_solver(direct_kkt_solver)
																	.static_regularization_enable(static_regularization_enable)
																	.dynamic_regularization_enable(dynamic_regularization_enable)
																	.iterative_refinement_enable(iterative_refinement_enable)
																	.presolve_enable(presolve_enable)
																	.time_limit(time_limit)
																	.max_step_fraction(max_step_fraction)
																	.tol_gap_abs(tol_gap_abs)
																	.tol_gap_rel(tol_gap_rel)
																	.tol_feas(tol_feas)
																	.tol_infeas_abs(tol_infeas_abs)
																	.tol_infeas_rel(tol_infeas_rel)
																	.tol_ktratio(tol_ktratio)
																	.reduced_tol_gap_abs(reduced_tol_gap_abs)
																	.reduced_tol_gap_rel(reduced_tol_gap_rel)
																	.reduced_tol_feas(reduced_tol_feas)
																	.reduced_tol_infeas_abs(reduced_tol_infeas_abs)
																	.reduced_tol_infeas_rel(reduced_tol_infeas_rel)
																	.reduced_tol_ktratio(reduced_tol_ktratio)
																	.equilibrate_min_scaling(equilibrate_min_scaling)
																	.equilibrate_max_scaling(equilibrate_max_scaling)
																	.linesearch_backtrack_step(linesearch_backtrack_step)
																	.min_switch_step_length(min_switch_step_length)
																	.min_terminate_step_length(min_terminate_step_length)
																	.static_regularization_constant(static_regularization_constant)
																	.static_regularization_proportional(static_regularization_proportional)
																	.dynamic_regularization_eps(dynamic_regularization_eps)
																	.dynamic_regularization_delta(dynamic_regularization_delta)
																	.iterative_refinement_reltol(iterative_refinement_reltol)
																	.iterative_refinement_abstol(iterative_refinement_abstol)
																	.iterative_refinement_stop_ratio(iterative_refinement_stop_ratio)
																	.build();



	char *solveMethodStr = mxArrayToString(solveMethodArray);
	if (strcmp(solveMethodStr, "qdldl") == 0) {
		settings.direct_solve_method = ClarabelDirectSolveMethods::QDLDL;
	} else if (strcmp(solveMethodStr, "FAER") == 0) {
		settings.direct_solve_method = ClarabelDirectSolveMethods::FAER;
	} else {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Unknown direct solve method: %s", solveMethodStr);
	}



	char *mergeMethodStr = mxArrayToString(mergeMethodArray);
	if (strcmp(mergeMethodStr, "CLIQUE_GRAPH") == 0) {
		settings.chordal_decomposition_merge_method = ClarabelCliqueMergeMethods::CLIQUE_GRAPH;
	} else if (strcmp(mergeMethodStr, "PARENT_CHILD") == 0) {
		settings.chordal_decomposition_merge_method = ClarabelCliqueMergeMethods::PARENT_CHILD;
	} else if (strcmp(mergeMethodStr, "NONE") == 0) {
		settings.chordal_decomposition_merge_method = ClarabelCliqueMergeMethods::NONE;
	} else {
		mexErrMsgIdAndTxt("MATLAB:clarabel_mex:invalidInput", "Unknown clique merge method: %s", mergeMethodStr);
	}

	// Free the allocated memory for the merge method string
	mxFree(mergeMethodStr);

	// Free the allocated memory for the solve method string
	mxFree(solveMethodStr);
    
    // Build solver
    DefaultSolver<double> solver(P, q, A, b, cones, settings);

    // Solve
    //solver.print_to_buffer();
    solver.solve();
	//if (verbose) {
	//	std::string bufstr = solver.get_print_buffer();
		//std::cout << bufstr << std::endl;
	//}
    

    // Get solution
    DefaultSolution<double> solution = solver.solution();
    
    // =============== Output ===============
    // Create the fifth output as a struct for solution
    const char *field_names[] = {"x", "z", "s", "status", "obj_val", "obj_val_dual", "solve_time", "iterations", "r_prim", "r_dual"};
    plhs[0] = mxCreateStructMatrix(1, 1, 10, field_names);

    // Set the fields in the struct
    mxArray *x = mxCreateDoubleMatrix(solution.x.size(), 1, mxREAL);
    Eigen::Map<Eigen::VectorXd>(mxGetPr(x), solution.x.size()) = solution.x;
    mxSetField(plhs[0], 0, "x", x);

    mxArray *z = mxCreateDoubleMatrix(solution.z.size(), 1, mxREAL);
    Eigen::Map<Eigen::VectorXd>(mxGetPr(z), solution.z.size()) = solution.z;
    mxSetField(plhs[0], 0, "z", z);

    mxArray *s = mxCreateDoubleMatrix(solution.s.size(), 1, mxREAL);
    Eigen::Map<Eigen::VectorXd>(mxGetPr(s), solution.s.size()) = solution.s;
    mxSetField(plhs[0], 0, "s", s);

    // solution status (enumeration) to string for output
    const char *status_str;
    switch (solution.status) {
        case SolverStatus::Unsolved:
            status_str = "Unsolved";
            break;
        case SolverStatus::Solved:
            status_str = "Solved";
            break;
        case SolverStatus::PrimalInfeasible:
            status_str = "PrimalInfeasible";
            break;
        case SolverStatus::DualInfeasible:
            status_str = "DualInfeasible";
            break;
        case SolverStatus::AlmostSolved:
            status_str = "AlmostSolved";
            break;
        case SolverStatus::AlmostPrimalInfeasible:
            status_str = "AlmostPrimalInfeasible";
            break;
        case SolverStatus::AlmostDualInfeasible:
            status_str = "AlmostDualInfeasible";
            break;
        case SolverStatus::MaxIterations:
            status_str = "MaxIterations";
            break;
        case SolverStatus::MaxTime:
            status_str = "MaxTime";
            break;
        case SolverStatus::NumericalError:
            status_str = "NumericalError";
            break;
        case SolverStatus::InsufficientProgress:
            status_str = "InsufficientProgress";
            break;
        default:
            status_str = "Unknown";
            break;
    }

    mxArray *status = mxCreateString(status_str);
    mxSetField(plhs[0], 0, "status", status);

    mxArray *obj_val = mxCreateDoubleScalar(solution.obj_val);
    mxSetField(plhs[0], 0, "obj_val", obj_val);

    mxArray *obj_val_dual = mxCreateDoubleScalar(solution.obj_val_dual);
    mxSetField(plhs[0], 0, "obj_val_dual", obj_val_dual);

    mxArray *solve_time = mxCreateDoubleScalar(solution.solve_time);
    mxSetField(plhs[0], 0, "solve_time", solve_time);

    mxArray *iterations = mxCreateDoubleScalar(solution.iterations);
    mxSetField(plhs[0], 0, "iterations", iterations);

    mxArray *r_prim = mxCreateDoubleScalar(solution.r_prim);
    mxSetField(plhs[0], 0, "r_prim", r_prim);

    mxArray *r_dual = mxCreateDoubleScalar(solution.r_dual);
    mxSetField(plhs[0], 0, "r_dual", r_dual);
}
