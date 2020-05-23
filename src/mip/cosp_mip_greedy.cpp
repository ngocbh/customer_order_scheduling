/*
 *  Problem : cosp.cc
 *  Description : 
 *  Created by ngocjr7 on [2020-05-23 07:46]	
*/

#include "ortools/linear_solver/linear_solver.h"
#include "utils.h"
#include "cosp_instance.h"

using namespace std;
typedef pair<int,int> ii;

namespace operations_research {

string run(const COSPInstance prob) {
	stringstream ret;
	// Create the linear solver with the GLOP backend.
	MPSolver solver("sportscheduling",
                MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);

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

			MPConstraint* c = solver.MakeRowConstraint(-infinity, prob.s[i][j]);
			for (int k = 0; k < prob.n; k++) 
				c->SetCoefficient(load[i][j][k], 1);
		}

	for (int k = 0; k < prob.n; k++)
		for (int j = 0; j < prob.p; j++) {
			MPConstraint* c = solver.MakeRowConstraint(-infinity, prob.d[k][j]);
			for (int i = 0; i < prob.m; i++)
				c->SetCoefficient(load[i][j][k], 1);
		}

	
	for (int k = 0; k < prob.n; k++) {
		MPConstraint* c = solver.MakeRowConstraint(prob.dn[k] - prob.slack_n[k], prob.dn[k] - prob.slack_n[k]);
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
	const MPSolver::ResultStatus result_status = solver.Solve();
	// Check that the problem has an optimal solution.
	if (result_status != MPSolver::OPTIMAL) {
		LOG(FATAL) << "The problem does not have an optimal solution!";
	}
	// [END solve]

	ret << prob.obj1 << " " << (int)objective->Value() << endl;
	for (int i = 0; i < prob.m; i++)
		for (int j = 0; j < prob.p; j++) {
			for (int k = 0; k < prob.n; k++)
				ret << load[i][j][k]->solution_value() << " ";
			ret << endl;
		}
 
	LOG(INFO) << "\nAdvanced usage:";
	LOG(INFO) << "Problem solved in " << solver.wall_time() << " milliseconds";
	LOG(INFO) << "Problem solved in " << solver.iterations() << " iterations";
	LOG(INFO) << "Problem solved in " << solver.nodes() << " branch-and-bound nodes";
	return ret.str();  
}
    
}
int main() 
{
	COSPInstance prob;
	prob.read_from_file("./data/cos_12_3_1");
	
	cout << operations_research::run(prob);
	return EXIT_SUCCESS;

	return 0;
}
