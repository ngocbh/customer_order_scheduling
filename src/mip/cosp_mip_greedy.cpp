/*
 *  Problem : cosp.cc
 *  Description : 
 *  Created by ngocjr7 on [2020-05-23 07:46]	
*/

#include "ortools/linear_solver/linear_solver.h"
#include "../model/utils.h"
#include "../model/cosp_instance.h"

using namespace std;
typedef pair<int,int> ii;

namespace operations_research {

string run(const COSPInstance prob, stringstream& stat) {
	stringstream ret;
	// Create the linear solver with the GLOP backend.
	MPSolver solver("sportscheduling",
                MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);
	solver.set_time_limit(50000); // 50s

	// Create variables
	vector<vector<vector<MPVariable*> >  > load;
 
	load.resize(prob.m);
	for (int i = 0; i < prob.m; i++) {
		load[i].resize(prob.p);
		for (int j = 0; j < prob.p; j++) {
			load[i][j].resize(prob.n);
			for (int k = 0; k < prob.n; k++) 
				load[i][j][k] = solver.MakeIntVar(0, prob.max_load[i][j][k], 
					"load|" + to_string(i) + "|" + to_string(j) + "|" + to_string(k));
		}
	}

	const double infinity = solver.infinity();

	// [START constraints]
	for (int i = 0; i < prob.m; i++)
		for (int j = 0; j < prob.p; j++) {
			MPConstraint* c = solver.MakeRowConstraint(-infinity, 1LL*prob.s[i][j]);
			for (int k = 0; k < prob.n; k++) 
				c->SetCoefficient(load[i][j][k], 1);
		}

	for (int k = 0; k < prob.n; k++)
		for (int j = 0; j < prob.p; j++) {
			MPConstraint* c = solver.MakeRowConstraint(-infinity, 1LL*prob.d[k][j]);
			for (int i = 0; i < prob.m; i++)
				c->SetCoefficient(load[i][j][k], 1);
		}

	
	for (int k = 0; k < prob.n; k++) {
		MPConstraint* c = solver.MakeRowConstraint(1LL*(prob.dn[k] - prob.slack_n[k]), 1LL*(prob.dn[k] - prob.slack_n[k]));
		for (int i = 0; i < prob.m; i++)
			for (int j = 0; j < prob.p; j++)
				c->SetCoefficient(load[i][j][k], 1);
	}	
	
	// [END constraints]
	// [START objective]
	// Minimize sum_{i,j,k,t} f[i][j][k][t] * d[j][k];
	MPObjective* const objective = solver.MutableObjective();
	for (int i = 0; i < prob.m; i++)
		for (int j = 0; j < prob.p; j++)
			for (int k = 0; k < prob.n; k++) 
				objective->SetCoefficient(load[i][j][k], prob.c[i][j][k]);
	objective->SetMinimization();
	// [END objective]

	// [START solve]
	MPSolverParameters params;
	// params.SetIntegerParam(MPSolverParameters::LP_ALGORITHM, MPSolverParameters::DUAL);
	const MPSolver::ResultStatus result_status = solver.Solve(params);
	// Check that the problem has an optimal solution.
	if (result_status != MPSolver::OPTIMAL and result_status != MPSolver::FEASIBLE ) {
		stat << "The problem does not have an optimal solution!";
		return to_string(prob.obj1) + " -1";
	}
	// [END solve]

	ret << prob.obj1 << " " << (long long)objective->Value() << endl;
	for (int i = 0; i < prob.m; i++)
		for (int j = 0; j < prob.p; j++) {
			for (int k = 0; k < prob.n; k++)
				ret << load[i][j][k]->solution_value() << " ";
			ret << endl;
		}
 
	stat << "\nAdvanced usage:";
	if (result_status == MPSolver::FEASIBLE )
		stat << "Problem has been stopped by timmer\n";
	stat << "Problem solved in " << solver.wall_time() << " milliseconds\n";
	stat << "Problem solved in " << solver.iterations() << " iterations\n";
	stat << "Problem solved in " << solver.nodes() << " branch-and-bound nodes\n";
	return ret.str();  
}
    
}
int main(int argc, char* argv[]) 
{
	parseCommandFlags(argc, argv);
	COSPInstance prob;
	if ( INPUT_FILE != "" ) {
		ifstream inp(INPUT_FILE);
		prob.parse_from_stream(inp);
	} else {
		cout << "No file passing, read from stdin\n"; 
		prob.parse_from_stream(cin);
	}

	stringstream stat;
	string results = operations_research::run(prob, stat);
	if (OUTPUT_FILE != "") {
		ofstream fout(OUTPUT_FILE);
		fout << results;
		fout.close();

		ofstream fout_stat(OUTPUT_FILE + "_stat");
		fout_stat << stat.str();
		fout_stat.close();
	}

	cout << results << endl;
	cout << stat.str() << endl;

	return EXIT_SUCCESS;
}
