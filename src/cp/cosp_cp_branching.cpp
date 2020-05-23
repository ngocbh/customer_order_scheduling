/*
 *  Problem : main.cpp
 *  Description : 
 *  Created by ngocjr7 on [2020-05-16 16:01]	
*/

#include "../model/utils.h"
#include "../model/cosp_instance.h"
#include "cosp.h"

void print_stat(ostream& out, Search::Statistics stat, microseconds duration, int num_solutions) {
	out << "Summary" << endl;
	out << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;
	out << "\tsolutions: \t\t" << num_solutions << endl;
	out << "\tfailures: \t\t" << stat.fail << endl;
	out << "\tnodes: \t\t\t" << stat.node << endl;
	out << "\trestarts: \t\t" << stat.restart << endl;
	out << "\tno-goods: \t\t" << stat.nogood << endl;
	out << "\tpeek-depth:\t\t" << stat.depth << endl;
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
		    _b->print(cout);
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
	parseCommandFlags(argc, argv);

	COSPInstance ins;
	if ( INPUT_FILE != "" ) {
		ifstream inp(INPUT_FILE);
		ins.parse_from_stream(inp);
	} else {
		cout << "No file passing, read from stdin\n"; 
		ins.parse_from_stream(cin);
	}


	SizeOptions opt("COSP");
	opt.model(COSP::MODEL_GREEDY);
	opt.model(COSP::MODEL_NAIVE, "naive");
	opt.model(COSP::MODEL_GREEDY, "greedy");
	opt.branching(COSP::BRANCH_OBJ1);
	opt.branching(COSP::BRANCH_NAIVE, "naive");
	opt.branching(COSP::BRANCH_OBJ1, "obj1");
	opt.parse(argc, argv);

	COSP* prob = new COSP(ins, opt);

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
