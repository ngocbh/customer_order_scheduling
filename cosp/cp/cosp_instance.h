#ifndef __COSP_INSTANCE_H__
#define __COSP_INSTANCE_H__

#include "utils.h"

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
			for (int j = 0; j < p; j++)
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
			for (int j = 0; j < p; j++)
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

#endif