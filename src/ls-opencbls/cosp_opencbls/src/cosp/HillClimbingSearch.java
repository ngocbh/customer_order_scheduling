package cosp;

import java.util.ArrayList;
import java.util.Random;

import cosp.HillClimbingSearchConstraintObjectiveFunction.Move;
import localsearch.model.IConstraint;
import localsearch.model.IFunction;
import localsearch.model.VarIntLS;

public class HillClimbingSearch {
	class Move{
		int i;
		int v;
		public Move(int i, int v){
			this.i = i; this.v  = v;
		}		
	}
	
	private IFunction f1;
	private IFunction f2;
	private IConstraint c;
	private VarIntLS[] X;
	private Random R = new Random();
	
	public HillClimbingSearch(IConstraint c, IFunction f1, IFunction f2, VarIntLS[] X) {
		this.c = c; 
		this.f1 = f1; 
		this.f2 = f2;
		this.X = X;
		
	}

	public void exploreNeighborhood(ArrayList<Move> cand){
		cand.clear();
		int minDeltaC = Integer.MAX_VALUE;
		int minDeltaF1 = Integer.MAX_VALUE;
		int minDeltaF2 = Integer.MAX_VALUE;
		for(int i = 0; i < X.length; i++){
			for(int v = X[i].getMinValue(); v <= X[i].getMaxValue(); v++){
				int deltaC = c.getAssignDelta(X[i], v);
				int deltaF1 = f1.getAssignDelta(X[i], v);
				int deltaF2 = f2.getAssignDelta(X[i], v);
				
//				System.out.println("deltaC(" + i + "," + v + ") = " + deltaC);
//				System.out.println("deltaF1(" + i + "," + v + ") = " + deltaF1);
//				System.out.println("deltaF2(" + i + "," + v + ") = " + deltaF2);
				if(deltaC < 0 || 
				   deltaC == 0 && deltaF1 < 0 ||
				   deltaC == 0 && deltaF1 == 0 && deltaF2 < 0){
					if(deltaC < minDeltaC || 
					   deltaC == minDeltaC && deltaF1 < minDeltaF1 ||
					   deltaC == minDeltaC && deltaF1 == minDeltaF1 && deltaF2 < minDeltaF2){
						cand.clear();
						cand.add(new Move(i,v));
						minDeltaC = deltaC; 
						minDeltaF1 = deltaF1;
						minDeltaF2 = deltaF2;
					}else if(deltaC == minDeltaC && deltaF1 == minDeltaF1 && deltaF2 == minDeltaF2){
						cand.add(new Move(i,v));
					}
				}
			}
		}
	}
	public void printSol(){
		for(int i = 0; i < X.length; i++) System.out.println("X[" + i + "] = " + X[i].getValue());
		System.out.println("f1 = " + f1.getValue());
		System.out.println("f2 = " + f2.getValue());
	}
	
	public void search(int maxIter){
		int it = 0;
		ArrayList<Move> cand = new ArrayList<>();
//		printSol();
		System.out.println("Init f1 = " + f1.getValue());
		System.out.println("Init f2 = " + f2.getValue());
		while(it < maxIter){
			exploreNeighborhood(cand);
			if(cand.size() == 0){
				System.out.println("Reach local optimum"); break;
			}
			
			Move m = cand.get(R.nextInt(cand.size()));
			X[m.i].setValuePropagate(m.v);
			System.out.println("Step " + it + ", c = " + 
					c.violations() + ", f1 = " + f1.getValue()
					+ ", f2 = " + f2.getValue());
			it++;
		}
		printSol();
	}

}
