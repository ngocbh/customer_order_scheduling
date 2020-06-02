/*
 *  Problem : cosp_tabu_search.cpp
 *  Description : 
 *  Created by ngocjr7 on [2020-05-24 10:41]	
*/
#include "../model/utils.h"
#include "../model/cosp_instance.h"

using namespace std;

namespace local_search {

struct Idx 
{
	int i, j, k;

	Idx(): i(0), j(0), k(0) {}

	Idx(int _i, int _j, int _k): 
		i(_i), j(_j), k(_k) {}

	Idx(const Idx & other) 
	{
		i = other.i, j = other.j, k = other.k;
	}

	bool operator==(const Idx & other) const
	{
		return (i == other.i && j == other.j && k == other.k);
	}

	friend ostream & operator<<(ostream & os, const Idx & id) {
		return os << "(" << id.i << "," << id.j << "," << id.k << ")";

	}
};

struct IdxHasher 
{
	size_t BASE = 31;
	size_t operator() (const Idx & a) const
	{
		size_t ret = 17;
		ret = ret * BASE + hash<int>()(a.i); 
		ret = ret * BASE + hash<int>()(a.j);
		ret = ret * BASE + hash<int>()(a.k);
		return ret;
	}
};

typedef pair<Idx, int> Assignment;

typedef vector<Assignment> Move;

bool compare_assignment(const Assignment & a, const Assignment & b) 
{
	return a.second < b.second;
}

class COSPSolution {
public:
	COSPInstance& prob;
	int n, m, p;
	unordered_map<Idx,int, IdxHasher> sparse_load;
	vector<vector<vector<int>>> load;
	int **tad;
	long long max_slack;
	long long total_cost;

	COSPSolution(COSPInstance& _prob): prob(_prob), n(_prob.n), m(_prob.m), p(_prob.p) 
	{
		max_slack = prob.obj1;
		total_cost = -1;

		load.resize(m);
		for (int i = 0; i < m; i++) {
			load[i].resize(p);
			for (int j = 0; j < p; j++)
				load[i][j] = vector<int>(n, 0);
		}


		tad = new int*[n];
		for (int i = 0; i < n; i++) {
			tad[i] = new int[p];
			for (int j = 0; j < p; j++)
				tad[i][j] = prob.allocated_d[i][j];
		}
		
		generate_greedy_solution();
	}

	int id(int i, int j,int k) const {
		return i*p*n + j*n + k;
	}

	void generate_greedy_solution() 
	{
		int **rs = new int*[m];
		for (int i = 0; i < m; i++) {
			rs[i] = new int[p];
			for (int j = 0; j < p; j++)
				rs[i][j] = prob.s[i][j];
		}

		vector<Assignment> flatten_c;
		flatten_c.resize(n*m*p);

		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) 
				{
					flatten_c[id(i,j,k)].first.i = i;
					flatten_c[id(i,j,k)].first.j = j;
					flatten_c[id(i,j,k)].first.k = k;
					flatten_c[id(i,j,k)].second = prob.c[i][j][k];
				}
        
		sort(flatten_c.begin(),flatten_c.end(), compare_assignment);
		for (auto e: flatten_c) 
		{
			int i(e.first.i), j(e.first.j), k(e.first.k);
			auto asg = min(tad[k][j], rs[i][j]);
			if ( !asg ) continue;

			tad[k][j] -= asg;
			rs[i][j] -= asg;
			load[i][j][k] = asg;
			sparse_load[Idx(i, j, k)] = asg;
		}

		total_cost = get_cost();
	}

	void print(ostream& os = cout) 
	{
		os << max_slack << " " << total_cost << endl;

		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++) {
				for (int k = 0; k < n; k++)
					os << load[i][j][k] << " ";
				os << endl;
			}
	}

	long long get_cost() {
		if (total_cost != -1) 
			return total_cost;
		total_cost = 0;
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) 
					total_cost += prob.c[i][j][k] * load[i][j][k];
		return total_cost;
	}

};

vector<Move> explore_neighbors(COSPSolution& sol) 
{
	vector<Move> ret;

	for (auto it = sol.sparse_load.begin(); it != sol.sparse_load.end(); ++it) {
		
	}

	return ret;
}

void run(COSPInstance& prob) 
{

	int iter = 0, max_iter = 1000;

	COSPSolution sol(prob);
	sol.print();

	while ( iter++ < max_iter ) {
		explore_neighbors(sol);
		break;
	}
}	
}

std::function<int()> generator = []{
  int i = 0;
  return [=]() mutable {
    return i < 10 ? i++ : -1;
  };
}();

int main(int argc, char* argv[]) 
{
	parseCommandFlags(argc, argv);
	COSPInstance prob;
	if ( INPUT_FILE != "" ) {
		ifstream inp(INPUT_FILE);
		cout << INPUT_FILE << endl;
		prob.parse_from_stream(inp);
	} else {
		cout << "No file passing, read from stdin\n"; 
		prob.parse_from_stream(cin);
	}

	local_search::run(prob);

	// int ret = 0; while ((ret = generator()) != -1) std::cout << "generator: " << ret << std::endl;
	return 0;
}
