/*
 *  Problem : main.cpp
 *  Description : 
 *  Created by ngocjr7 on [2020-05-16 16:01]	
*/
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> 

using namespace std;
using namespace std::chrono; 
using namespace Gecode;

class COSPInstance{
public:
	int n, m, p;
	int** s;
	int* sp; // $sp[*] = sum_{i=0}^m s[i][*]$
	int** d;
	int* dp; // $dp[*] = sum_{i=0}^n d[i][*]$
	int*** c;
	int* flatten_c; 
	int*** load;
	int*** max_load; // $max_load[i][j][k] = max(s[i][j], d[k][j])$
	int obj1, obj2;
	int max_obj1; // $max_obj1 = max_i^n sum_j^p d[i][j]$
	int max_obj2; // $max_obj2 = sum_{i,j,k} c[i][j][k]*max_load[i][j][k]

	COSPInstance() {}

	COSPInstance(COSPInstance& org) {
		n = org.n, p = org.p, m = org.m;

		// allocation and // initialization
		s = new int*[m];
		for (int i = 0; i < m; i++) {
			s[i] = new int[p];
		}

		d = new int*[n];
		for (int i = 0; i < n; i++)
			d[i] = new int[p];

		c = new int**[m], load = new int**[m], max_load = new int**[m];
		for (int i = 0; i < m; i++) {
			c[i] = new int*[p], load[i] = new int*[p], max_load[i] = new int*[p];
			for (int j = 0; j <= n; j++)
				c[i][j] = new int[n], load[i][j] = new int[n], max_load[i][j] = new int[n];
		}

		flatten_c = new int[m*p*n];
		sp = new int[p];
		dp = new int[p];

		// read and compute upper bound
		// read s and compute sum sp
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				s[i][j] = org.s[i][j], sp[j] += s[i][j];

		// read c and compute sum dp and upper bound of obj1
		for (int i = 0; i < n; i++) {
			int temp_max_obj1 = 0;
			for (int j = 0; j < p; j++)
				d[i][j] = org.d[i][j], temp_max_obj1 += d[i][j], dp[j] += d[i][j];
			max_obj1 = max(max_obj1,temp_max_obj1);
		}

		// read c and compute upper bound of load and obj2
		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) {
					c[i][j][k] = flatten_c[id(i,j,k)] = org.c[i][j][k];
					max_load[i][j][k] = max(s[i][j], d[k][j]);
					max_obj2 += c[i][j][k] * max_load[i][j][k];
				}
	}

	void read_from_file(string filename) {
		ifstream inp(filename);
		inp >> n >> p >> m;

		// initalization
		s = new int*[m];
		for (int i = 0; i < m; i++)
			s[i] = new int[p];

		d = new int*[n];
		for (int i = 0; i < n; i++)
			d[i] = new int[p];

		c = new int**[m], load = new int**[m], max_load = new int**[m];
		for (int i = 0; i < m; i++) {
			c[i] = new int*[p], load[i] = new int*[p], max_load[i] = new int*[p];
			for (int j = 0; j <= n; j++)
				c[i][j] = new int[n], load[i][j] = new int[n], max_load[i][j] = new int[n];
		}

		flatten_c = new int[m*p*n];
		sp = new int[p];
		dp = new int[p];

		// read s and compute bound
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				inp >> s[i][j], sp[j] += s[i][j];

		// read d
		for (int i = 0; i < n; i++) {
			int temp_max_obj1 = 0;
			for (int j = 0; j < p; j++)
				inp >> d[i][j], temp_max_obj1 += d[i][j], dp[j] += d[i][j];
			max_obj1 = max(max_obj1,temp_max_obj1);
		}

		// read c
		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) {
					inp >> c[i][j][k], flatten_c[id(i,j,k)] = c[i][j][k];
					max_load[i][j][k] = max(s[i][j], d[k][j]);
					max_obj2 += c[i][j][k] * max_load[i][j][k];
				}
	}

	int id(int i, int j,int k) const {
		return i*p*n + j*n + k;
	}

};

class COSP: public Space {
private:
	const COSPInstance& ins;
	IntVarArray load;
	IntVarArray slack;
	IntVar obj1, obj2;
public:
	COSP(const COSPInstance& prob):
		ins(prob),
		load(*this, ins.m*ins.n*ins.p), 
		slack(*this, ins.p*ins.n),
		obj1(*this, 0, ins.max_obj1),
		obj2(*this, 0, ins.max_obj2) {

		// initialize decision variables
		for (int i = 0; i < ins.m; i++)
			for (int j = 0; j < ins.p; j++)
				for (int k = 0; k < ins.n; k++) {
					load[ins.id(i,j,k)] = IntVar(*this, 0, ins.max_load[i][j][k]);
				}

		for (int i = 0; i < ins.n; i++)
			for (int j = 0; j < ins.p; j++) 
				slack[i*ins.p+j] = IntVar(*this, 0, ins.d[i][j]);

		// main constraints
		// $slack[i][j] = d[i][j] - sum_k^m load[k][j][i]$
		for (int i = 0; i < ins.n; i++)
			for (int j = 0; j < ins.p; j++)
				rel(*this, slack[i*ins.p+j] == (ins.d[i][j] - sum(load.slice(ins.id(0,j,i), ins.p*ins.n, ins.m)) ));

		// $slack[i][j] \geq 0$
		rel(*this, slack, IRT_GQ, 0);

		// $sum_k^n load[i][j][k] \leq s[i][j]$
		for (int i = 0; i < ins.m; i++)
			for (int j = 0; j < ins.p; j++) 
				linear(*this, load.slice(ins.id(i,j,0), 1, ins.n) , IRT_LQ, ins.s[i][j]);

		// Redundant constraints
		for (int i = 0; i < ins.p; i++) 
			linear(*this, slack.slice(i, ins.p, ins.n) , IRT_LQ, ins.sp[i]);

		// $sum_{i=0}^n slack[i][*] = dp[*] - sp[*] = sum_{i=0}^n d[i][*] - sum_{i=0}^m s[i][*]$
		for (int i = 0; i < ins.p; i++)
			linear(*this, slack.slice(i, ins.p, ins.n), IRT_EQ, max(0,ins.dp[i] - ins.sp[i]));
		

		// objectives
		// $sum_{j=0}^p slack[i][j] \leq obj1 for all i \in [1,n]$
		for (int i = 0; i < ins.n; i++)
			linear(*this, slack.slice(i*ins.p, 1, ins.p), IRT_LQ, obj1);

		// $sum_{i,j,k} c[i][j][k] * load[i][j][k] \leq obj2$
		IntArgs c(ins.m*ins.n*ins.p, ins.flatten_c);
		linear(*this, c, load, IRT_EQ, obj2);

		// branching
		branch(*this, obj1, INT_VAL_MIN());
		branch(*this, slack, INT_VAL_MAX());
		branch(*this, load, INT_VAL_MIN());
        branch(*this, obj2, INT_VAL_MIN());

		cout << "initalized"<<  endl;
	}

	COSP(COSP& s): ins(s.ins), Space(s) {
		this->load.update(*this, s.load);
		this->slack.update(*this, s.slack);
		this->obj1.update(*this, s.obj1);
		this->obj2.update(*this, s.obj2);
	}

	virtual Space* copy() {
		return new COSP(*this);
	}

	virtual void constrain(const Space& _b) {
        const COSP& b = static_cast<const COSP&> (_b);
        rel(*this, obj1 <= b.obj1.val());
        rel(*this, (obj1 == b.obj1.val()) >> (obj2 < b.obj2.val()));
    }

	void print() const {
		cout << "Start ---- " << endl;
        cout << load << endl;
        cout << slack << endl;
        cout << obj1 << endl;
        cout << obj2 << endl;
        cout << "End ----" << endl;
    }
};

string format_duration(microseconds ms ) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<microseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    stringstream ss;
    if (hour.count() != 0)
    	ss << hour.count() << ":";
    if (mins.count() != 0)
		ss << mins.count() << ":" ;
	if (secs.count() != 0) 
		ss << secs.count() << ":" ;
	ss << float(ms.count())/1000;
    return ss.str();
}

void mybab(Space* s, unsigned int& n, Space*& b) {
  	switch (s->status()) {
  		case SS_FAILED:
	    	delete s;
	    	break;
	  	case SS_SOLVED: {
	    	// solved
		    n++;
		    delete b; 
		    (void) s->choice(); b = s->clone(); delete s;
		    COSP* _b = (COSP*) b;
		    _b->print();
		}
	    break;
	  	case SS_BRANCH: 
	    {
			const Choice* ch = s->choice();
			// remember number of solutions
			unsigned int m=n;
			for (int i = 0; i < ch->alternatives(); i++) {
				Space* c = s->clone();
				// constrain clone
			 	if (n > m)
					c->constrain(*b);
				// explore second alternative
				c->commit(*ch,i);
				mybab(c,n,b);
			}
			delete ch;
	    }
	    break;
  	}
}

Space* mybab(Space* s) {
  unsigned int n = 0; Space* b = NULL;
  mybab(s,n,b);
  return b;
}

int main(int argc, char* argv[]) {
	COSPInstance ins;
	ins.read_from_file("./data/cosp_n_2_p_2_m_3.txt");

	COSP* prob = new COSP(ins);
	Search::Options o;
    o.stop = Search::Stop::time(300000);

    int num_solutions = 0;
    auto start = high_resolution_clock::now(); 

    COSP* solution = (COSP*)mybab(prob);
    // solution->print();

    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);

    // Search::Statistics stat = solution->statistics();
    cout << "Summary" << endl;
    cout << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;
    // cout << "\tsolutions: \t\t" << num_solutions << endl;
    // cout << "\tfailures: \t\t" << stat.fail << endl;
    // cout << "\tnodes: \t\t\t" << stat.node << endl;
    // cout << "\trestarts: \t\t" << stat.restart << endl;
    // cout << "\tno-goods: \t\t" << stat.nogood << endl;
    // cout << "\tpeek-depth:\t\t" << stat.depth << endl;
}
