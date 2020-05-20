#ifndef __COSP_H__
#define __COSP_H__

#include "utils.h"
#include "cosp_instance.h"

class COSP: public Space {
private:
	const COSPInstance& ins;
	IntVarArray load;
	IntVarArray slack;
	IntVar obj1, obj2;
public:
	enum {
	    MODEL_NAIVE,  ///<
	    MODEL_GREEDY ///< Use greedy to compute objective 1
	};

	enum {
		BRANCH_NAIVE,
		BRANCH_OBJ1
	};

	COSP(const COSPInstance& prob, const SizeOptions& opt):
		ins(prob),
		load(*this, ins.m*ins.n*ins.p), 
		slack(*this,ins.p*ins.n),
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


		switch (opt.model()) {
		case MODEL_NAIVE: {
			// objective 1
			// $sum_{j=0}^p slack[i][j] \leq obj1 for all i \in [1,n]$
			for (int i = 0; i < ins.n; i++)
				linear(*this, slack.slice(i*ins.p, 1, ins.p), IRT_LQ, obj1);
			break;
			}
		case MODEL_GREEDY: {
			// objective 1
			rel(*this, obj1 == ins.obj1);

			for (int i = 0; i < ins.n; i++)
				linear(*this, slack.slice(i*ins.p, 1, ins.p), IRT_EQ, ins.slack_n[i]);
			break;
			}
		}

		// objective 2
		// $sum_{i,j,k} c[i][j][k] * load[i][j][k] \leq obj2$
		IntArgs c(ins.m*ins.n*ins.p, ins.flatten_c);
		linear(*this, c, load, IRT_EQ, obj2);

		// branching
		switch (opt.branching()) {
		case BRANCH_NAIVE:
			branch(*this, load, INT_VAL_MIN());
			branch(*this, slack, INT_VAL_MIN());
			branch(*this, obj1, INT_VAL_MIN());
	        branch(*this, obj2, INT_VAL_MIN());
	        break;
	    case BRANCH_OBJ1:
	    	branch(*this, obj1, INT_VAL_MIN());
	    	branch(*this, slack, INT_VAL_MAX());
	    	branch(*this, load, INT_VAL_MIN());
	        branch(*this, obj2, INT_VAL_MIN());
	        break;
	    }
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

	void print(ostream& out) const {
		out << obj1 << " " << obj2 << endl;
		for (int i = 0; i < ins.m; i++)
			for (int j = 0; j < ins.p; j++) {
				for (int k = 0; k < ins.n; k++)
					out << load[ins.id(i,j,k)].val() << " ";
				out << endl;
			}
    }
};

#endif