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

int minimize_maximum_slack(const COSPInstance prob) {
	// Create the linear solver with the GLOP backend.
	MPSolver solver("cosp_1",
                MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);
	solver.set_time_limit(50000); // 50s

	// Create variables
	vector<vector<vector<MPVariable*> >  > load;
	MPVariable *obj1;

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

	obj1 = solver.MakeIntVar(0, prob.max_obj1, "objective1");

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
		MPConstraint* c = solver.MakeRowConstraint(prob.dn[k], infinity);
		c->SetCoefficient(obj1, 1);
		for (int i = 0; i < prob.m; i++)
			for (int j = 0; j < prob.p; j++) 
				c->SetCoefficient(load[i][j][k], 1);
	}	
	// [END constraints]

	// [START objective]
	// Minimize sum_{i,j,k,t} f[i][j][k][t] * d[j][k];
	MPObjective* const objective = solver.MutableObjective();
	objective->SetCoefficient(obj1, 1);
	objective->SetMinimization();
	// [END objective]

	// [START solve]
	const MPSolver::ResultStatus result_status = solver.Solve();
	// Check that the problem has an optimal solution.
	if (result_status != MPSolver::OPTIMAL) {
		return -1;
	}
	// [END solve]

	return (long long)objective->Value();
}

string minimize_cost(const COSPInstance prob, long long objective1) {
	stringstream ret;
	// Create the linear solver with the GLOP backend.
	MPSolver solver("cosp_2",
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
		long long slackn = min(prob.dn[k], objective1);
		MPConstraint* c = solver.MakeRowConstraint(1LL*(prob.dn[k] - slackn), 1LL*(prob.dn[k] - slackn));
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
		return to_string(objective1) + " -1";
	}
	// [END solve]

	ret << objective1 << " " << (int)objective->Value() << endl;
	for (int i = 0; i < prob.m; i++)
		for (int j = 0; j < prob.p; j++) {
			for (int k = 0; k < prob.n; k++)
				ret << load[i][j][k]->solution_value() << " ";
			ret << endl;
		}

	return ret.str();  
}
    
}
int main(int argc, char* argv[]) 
{
	parseCommandFlags(argc, argv);


	auto start = high_resolution_clock::now(); 

	COSPInstance prob;
	if ( INPUT_FILE != "" ) {
		ifstream inp(INPUT_FILE);
		prob.parse_from_stream(inp);
	} else {
		cout << "No file passing, read from stdin\n"; 
		prob.parse_from_stream(cin);
	}

	// phase 1: minimize maximum slack
	long long objective1 = operations_research::minimize_maximum_slack(prob);

	// phase 2: minimize cost

	string results;
	if (objective1 == -1) 
		results = "-1";
	else {
		results = operations_research::minimize_cost(prob, objective1);
	}

	auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);

    stringstream stat;	
    stat << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;
	// save results
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
