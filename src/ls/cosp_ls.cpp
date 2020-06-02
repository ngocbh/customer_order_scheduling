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

ostream & operator<<(ostream & out, Assignment a) 
{
	out << a.first << " += " << a.second;
	return out;
}

typedef vector<Assignment> Move;

ostream & operator<<(ostream & out, Move a) 
{
	for (auto e : a) 
		out << e << endl;
	return out;
}


bool compare_assignment(const Assignment & a, const Assignment & b) 
{
	return a.second < b.second;
}

ostream & operator<<(ostream & out, vector<vector<int>> a) 
{
	for (int i = 0; i < a.size(); i++) {
		for (int j = 0; j < a[i].size(); j++)
			out << a[i][j] << " ";
		out << endl;
	}
	return out;
}

ostream & operator<<(ostream & out, vector<vector<vector<int>>> a) 
{
	for (int i = 0; i < a.size(); i++) 
		for (int j = 0; j < a[i].size(); j++) {
			for (int k = 0; k < a[i][j].size(); k++)
				out << a[i][j][k] << " ";
			out << endl;
		}
	return out;
}


class COSPSolution {
public:
	COSPInstance& prob;
	int n, m, p;
	unordered_map<Idx,int, IdxHasher> sparse_load;
	vector<vector<vector<int>>> load;
	vector<vector<int>> tad, demand, remain;
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


		tad.resize(n), demand.resize(n);
		for (int i = 0; i < n; i++) {
			tad[i].resize(p), demand[i].resize(p);
			for (int j = 0; j < p; j++)
				tad[i][j] = prob.allocated_d[i][j], demand[i][j] = prob.d[i][j] - prob.allocated_d[i][j];
		}

		remain.resize(m);
		for (int i = 0; i < m; i++) {
			remain[i].resize(p);
			for (int j = 0; j < p; j++)
				remain[i][j] = prob.s[i][j];
		}
		
		generate_greedy_solution();
	}

	int id(int i, int j,int k) const {
		return i*p*n + j*n + k;
	}

	void generate_greedy_solution() 
	{
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
			auto asg = min(tad[k][j], remain[i][j]);
			if ( !asg ) continue;

			tad[k][j] -= asg;
			remain[i][j] -= asg;
			load[i][j][k] = asg;
			sparse_load[Idx(i, j, k)] = asg;
		}

		total_cost = get_cost();
	}

	void print(ostream& os = cout) 
	{
		os << max_slack << " " << total_cost << endl;
		os << load;
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

void exchange(COSPSolution & sol, Move m)
{
	for (auto a : m) {
		int i(a.first.i), j(a.first.j), k(a.first.k), val(a.second);
		if ( k == -1 ) {
			sol.remain[i][j] += val;
		} else {
			sol.load[i][j][k] += val;
			sol.demand[k][j] -= val;
			sol.total_cost += val * sol.prob.c[i][j][k];
			if (sol.sparse_load.count(Idx(i,j,k)) == 0 ) sol.sparse_load[Idx(i,j,k)] = 0;
			sol.sparse_load[Idx(i,j,k)] += val;
			if (sol.sparse_load.count(Idx(i,j,k)) == 0 ) sol.sparse_load.erase(Idx(i,j,k));
		}
	}
}

bool explore_neighbors(COSPSolution& sol) 
{
	bool moved = false;
	for (auto it = sol.sparse_load.begin(); it != sol.sparse_load.end(); ++it) 
	{
		auto cur_i(it->first.i), cur_j(it->first.j), cur_k(it->first.k), cur_value(it->second);

		auto max_gain = -(numeric_limits<long long>::max)();
		vector<Move> candidates;

		for (int i = 0; i < sol.m; i++)
			for (int j = 0; j < sol.p; j++)
				for (int k = 0; k < sol.n; k++) 
				{
					if ( cur_i == i && cur_j == j && cur_k == k ) continue;
					if ( k == cur_k ) 
					{
						auto max_exchange = (numeric_limits<int>::max)();
						max_exchange = min(max_exchange, sol.demand[k][j]);
						max_exchange = min(max_exchange, cur_value);
						max_exchange = min(max_exchange, sol.remain[i][j]);

						auto gain = max_exchange * (sol.prob.c[cur_i][cur_j][cur_k] - sol.prob.c[i][j][k]);

						if ( max_gain < gain && max_exchange ) 
						{
							max_gain = gain;
							candidates.clear();
							Move move;
							move.push_back(make_pair(Idx(cur_i, cur_j, cur_k), -max_exchange));
							move.push_back(make_pair(Idx(i, j, k), max_exchange));
							move.push_back(make_pair(Idx(cur_i, cur_j, -1), max_exchange));
							move.push_back(make_pair(Idx(i, j, -1), -max_exchange));
							candidates.push_back(move);
						} else if ( max_gain == gain ) {
							Move move;
							move.push_back(make_pair(Idx(cur_i, cur_j, cur_k), -max_exchange));
							move.push_back(make_pair(Idx(i, j, k), max_exchange));
							move.push_back(make_pair(Idx(cur_i, cur_j, -1), max_exchange));
							move.push_back(make_pair(Idx(i, j, -1), -max_exchange));
							candidates.push_back(move);
						}
					} 
					else if ( sol.remain[i][j] == 0 ) 
					{
						for (int ii = 0; ii < sol.m; ii++)
							for (int jj = 0; jj < sol.p; jj++) 
							{
								auto max_exchange = (numeric_limits<int>::max)();
								max_exchange = min(max_exchange, cur_value);
								max_exchange = min(max_exchange, sol.demand[cur_k][j]);
								max_exchange = min(max_exchange, sol.load[i][j][k]);

								auto eviction = min(sol.demand[k][jj], 
									sol.remain[ii][jj] + (cur_i==ii&&cur_j==jj) * cur_value);

								max_exchange = min(max_exchange, eviction);

								auto gain = max_exchange * (sol.prob.c[cur_i][cur_j][cur_k] 
									+ sol.prob.c[i][j][k] - sol.prob.c[i][j][cur_k] - sol.prob.c[ii][jj][k]);

								if ( max_gain < gain && max_exchange ) 
								{
									max_gain = gain;
									candidates.clear();
									Move move;
									move.push_back(make_pair(Idx(cur_i, cur_j, cur_k), -max_exchange));
									move.push_back(make_pair(Idx(i, j, k), -max_exchange));
									move.push_back(make_pair(Idx(i, j, cur_k), max_exchange));
									move.push_back(make_pair(Idx(ii, jj, k), max_exchange));
									move.push_back(make_pair(Idx(cur_i, cur_j, -1), max_exchange));
									move.push_back(make_pair(Idx(ii, jj, -1), -max_exchange));
									candidates.push_back(move);
								} else if ( max_gain == gain ) {
									Move move;
									move.push_back(make_pair(Idx(cur_i, cur_j, cur_k), -max_exchange));
									move.push_back(make_pair(Idx(i, j, k), -max_exchange));
									move.push_back(make_pair(Idx(i, j, cur_k), max_exchange));
									move.push_back(make_pair(Idx(ii, jj, k), max_exchange));
									move.push_back(make_pair(Idx(cur_i, cur_j, -1), max_exchange));
									move.push_back(make_pair(Idx(ii, jj, -1), -max_exchange));
									candidates.push_back(move);
								}
							}
					}
				}

		if (max_gain > 0) {
			Move move = candidates[rand() % candidates.size()];
			exchange(sol, move);
			cout << "TOTAL COST:" << sol.total_cost << endl << move << endl;
			moved = true;
		}
	}
	return moved;
}

string run(COSPInstance& prob) 
{
	stringstream ret;

	int iter = 0, max_iter = 100;

	COSPSolution sol(prob);
	// sol.print(cout);

	while ( iter++ < max_iter ) {
		bool moved = explore_neighbors(sol);
		if (!moved) {
			ret << "Reach Local Optimum" << endl;
			break;
		}

		cout << "Iteration " << iter << " : total_cost = "  << sol.total_cost << endl;  
	}

	sol.print(ret);

	return ret.str();
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
	auto start = high_resolution_clock::now(); 

	int seed = 7;
	srand(seed);
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

	string result = local_search::run(prob);
	cout << result;

	auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);

    stringstream stat;	
    stat << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;

	if (OUTPUT_FILE != "") {
		ofstream fout(OUTPUT_FILE);
		fout << result;
		fout.close();

		ofstream fout_stat(OUTPUT_FILE + "_stat");
		fout_stat << stat.str();
		fout_stat.close();
	}

	// int ret = 0; while ((ret = generator()) != -1) std::cout << "generator: " << ret << std::endl;
	return 0;
}
