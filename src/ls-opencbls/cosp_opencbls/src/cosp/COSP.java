package cosp;

import localsearch.model.ConstraintSystem;
import localsearch.model.IFunction;
import localsearch.model.LocalSearchManager;
import localsearch.model.VarIntLS;
import localsearch.constraints.basic.LessOrEqual;
import localsearch.functions.basic.FuncMinus;
import localsearch.functions.basic.FuncMult;
import localsearch.functions.max_min.FMax;
import localsearch.functions.max_min.Max;
import localsearch.functions.sum.*;
import cosp.HillClimbingSearch;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random;
import java.util.Scanner;

public class COSP {
	int M, N, P;
	int[][] S;
	int[][] D;
	int[][][] c;
	LocalSearchManager mgr;
	ConstraintSystem CS;
	VarIntLS[][][] load;
	
	IFunction obj1;
	IFunction obj2;

	Random random = new Random();;
	
	public int readData(String filePath) {
		int sum = 0;
		try {
			File file = new File(filePath);
			Scanner in = new Scanner(file);
			N = in.nextInt();
			P = in.nextInt();
			M = in.nextInt();
			System.out.println("N = " + N + " M = " + M + " P = " + P);
			S = new int[M][P];
			
			for (int m = 0; m < M; m++) {
				for (int p = 0; p < P; p++) {
					S[m][p] = in.nextInt();
					sum += S[m][p];
				}
			}

			D = new int[N][P];
			for (int n = 0; n < N; n++) {
				for (int p = 0; p < P; p++) {
					D[n][p] = in.nextInt();
				}
			}

			c = new int[M][P][N];
			for (int m = 0; m < M; m++) {
				for (int p = 0; p < P; p++) {
					for (int n = 0; n < N; n++) {
						c[m][p][n] = in.nextInt();
					}
				}
			}
			in.close();
		} catch (FileNotFoundException e) {
			System.out.println("FileNotFoundException");
			e.printStackTrace();
		}
		return sum;
	}
	
	public void initSolution() {
		for (int m = 0; m < M; m++) {
			for (int p = 0; p < P; p++) {
				for (int n = 0; n < N; n++) {
					load[m][p][n].setValue(random.nextInt(load[m][p][n].getMaxValue()+1));
				}
			}
		}
	}

	public void stateModel(int max_obj1) {
		mgr = new LocalSearchManager();
		CS = new ConstraintSystem(mgr);
		
		load = new VarIntLS[M][P][N];
		for (int m = 0; m < M; m++) {
			for (int p = 0; p < P; p++) {
				for (int n = 0; n < N; n++) {
					load[m][p][n] = new VarIntLS(mgr, 0, Math.min(S[m][p], D[n][p]));
				}
			}
		}
		initSolution();
		
		// Constraint #1
		for (int m = 0; m < M; m++) {
			for (int p = 0; p < P; p++) {
				CS.post(new LessOrEqual(new SumVar(load[m][p]), S[m][p]));
			}
		}
		
		// Constraint #2
		// Objective #1
		IFunction[] all_slack = new IFunction[N];
		for (int n = 0; n < N; n++) {
			IFunction[] slacks = new IFunction[P];
			for (int p = 0; p < P; p++) {
				VarIntLS[] load_m = new VarIntLS[M];
				for (int m = 0; m < M; m++) {
					load_m[m] = load[m][p][n];
				}
				slacks[p] = new FuncMinus(D[n][p], new SumVar(load_m));
				CS.post(new LessOrEqual(0, slacks[p]));
			}
//			CS.post(new LessOrEqual(new SumFun(slacks), obj1));
			all_slack[n] = new SumFun(slacks);
		}
		obj1 = new FMax(all_slack);
		
		// Objective #2
		IFunction[] load_mult_c = new IFunction[M*P*N];
		int i = 0;
		for (int m = 0; m < M; m++) {
			for (int p = 0; p < P; p++) {
				for (int n = 0; n < N; n++) {
					load_mult_c[i] = new FuncMult(load[m][p][n], c[m][p][n]);
					i++;
				}
			}
		}
		obj2 = new SumFun(load_mult_c);
		
		mgr.close();
	}

	public static void main(String[] args) {
		COSP cosp = new COSP();
		String filePath = "src/cosp/data/cos_1000_1000";
		int max_obj1 = cosp.readData(filePath);
		System.out.println("Max_obj1 = " + max_obj1);
		cosp.stateModel(max_obj1);
//		for (int i = 0; i < cosp.CS.getVariables().length; i++) {
//			System.out.println("x[" + i + "] = " + cosp.CS.getVariables()[i].getValue());
//		}
		HillClimbingSearch searcher = new HillClimbingSearch(cosp.CS, cosp.obj1, cosp.obj2, cosp.CS.getVariables());
		searcher.search(1000);
		
//		System.out.println(cosp.obj1.getValue());
	}
}
