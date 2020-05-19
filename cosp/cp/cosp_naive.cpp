/*
 *  Problem : main.cpp
 *  Description : 
 *  Created by ngocjr7 on [2020-05-16 16:01]	
*/

#include "utils.h"
#include "cosp_instance.h"
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

int main(int argc, char* argv[]) {
	COSPInstance ins;
	parseCommandFlags(argc, argv);
	ins.read_from_file(INPUT_FILE);

	COSP* prob = new COSP(ins);
	Search::Options o;
    o.stop = Search::Stop::time(20000);

	BAB<COSP> e(prob, o);
    delete prob;

    int num_solutions = 0;
    auto start = high_resolution_clock::now(); 
    COSP* solution;
    while (COSP* s = e.next()) {
        if (INTERMEDIATE) {
        	cout << "Start ---- " << endl;
        	s->print(cout);
        	cout << "End ----" << endl;
        }
        num_solutions += 1;
        solution = (COSP*)s->clone();
        delete s;
    }
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);
    Search::Statistics stat = e.statistics();
    
    if (OUTPUT_FILE != "") {
	    ofstream fout(OUTPUT_FILE);
        if (num_solutions) {
            solution->print(fout);
            solution->print(cout);
        }
	    fout.close();
	    ofstream fstat(OUTPUT_FILE+"_stat");
        if (e.stopped()) 
            fstat << "Model has been stopped by timmer, result wasn't proven\n"; 
	    print_stat(fstat, stat, duration, num_solutions);
	}

    print_stat(cout, stat, duration, num_solutions);
}
