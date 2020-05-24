/*
 *  Problem : cosp_instance.h
 *  Description : 
 *  Created by ngocjr7 on [2020-05-24 10:41]	
*/

#ifndef __COSP_INSTANCE_H__
#define __COSP_INSTANCE_H__

#include "utils.h"

class COSPInstance{
public:
	int n, m, p;
	int** s;
	long long* sp; // $sp[*] = sum_{i=0}^m s[i][*]$
	long long* slack_n; 
	int** d;
	long long* dp; // $dp[*] = sum_{i=0}^n d[i][*]$
	long long* dn;
	int*** c;
	int* flatten_c; 
	int*** load;
	int*** max_load; // $max_load[i][j][k] = max(s[i][j], d[k][j])$
	long long obj1, obj2;
	long long max_obj1; // $max_obj1 = max_i^n sum_j^p d[i][j]$
	long long max_obj2; // $max_obj2 = sum_{i,j,k} c[i][j][k]*max_load[i][j][k]
	int** allocated_d;

	COSPInstance() {}

	COSPInstance(COSPInstance& org) {
		n = org.n, p = org.p, m = org.m;

		allocate(m,p,n);

		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++) {
				s[i][j] = org.s[i][j];
			}

		for (int i = 0; i < n; i++) 
			for (int j = 0; j < p; j++) {
				d[i][j] = org.d[i][j], allocated_d[i][j] = org.allocated_d[i][j];
			}
		
		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) {
					c[i][j][k] = flatten_c[id(i,j,k)] = org.c[i][j][k];
					load[i][j][k] = org.load[i][j][k];
					max_load[i][j][k] = org.max_load[i][j][k];
				}

		obj1 = org.obj1, obj2 = org.obj2; 

		max_obj1 = org.max_obj1, max_obj2 = org.max_obj2;
		for (int i = 0; i < p; i++) sp[i] = org.sp[i], dp[i] = org.dp[i];
		for (int i = 0; i < n; i++) dn[i] = org.dn[i], slack_n[i] = org.slack_n[i];
	}

	void parse_from_stream(istream& inp) {
		inp >> n >> p >> m;
		allocate(m,p,n);	
		// read s and compute bound
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++) {
				inp >> s[i][j], sp[j] += s[i][j];
				assert(s[i][j] >= 0);
				assert(sp[j] >= 0);
			}

		// read d
		for (int i = 0; i < n; i++) {
			long long temp_max_obj1 = 0;
			for (int j = 0; j < p; j++) {
				inp >> d[i][j], temp_max_obj1 += d[i][j], dp[j] += d[i][j], dn[i] += d[i][j];
				assert(d[i][j] >= 0);
				assert(dp[j] >= 0);
				assert(dn[i] >= 0);
			}
			max_obj1 = max(max_obj1,temp_max_obj1);	
		}
		assert(max_obj1 >= 0);
		// read c
		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) {
					inp >> c[i][j][k], flatten_c[id(i,j,k)] = c[i][j][k];
					assert(c[i][j][k] >= 0);
					max_load[i][j][k] = max(s[i][j], d[k][j]);
					assert(max_load[i][k][k] >= 0);
					max_obj2 += c[i][j][k] * max_load[i][j][k];
				}
		assert(max_obj2 >= 0);
		for (int i = 0; i < n; i++)
			assert(slack_n[i] >= 0);
		minimize_max_slack();
	}

	void allocate(int m,int p,int n) {
		// allocation and // initialization
		s = new int*[m];
		for (int i = 0; i < m; i++) {
			s[i] = new int[p];
			for (int j = 0; j < p; j++) 
				s[i][j] = 0;
		}

		d = new int*[n];
		for (int i = 0; i < n; i++) {
			d[i] = new int[p];
			for (int j = 0; j < p; j++)
				d[i][j] = 0;
		}

		c = new int**[m], load = new int**[m], max_load = new int**[m];
		for (int i = 0; i < m; i++) {
			c[i] = new int*[p], load[i] = new int*[p], max_load[i] = new int*[p];
			for (int j = 0; j < p; j++) {
				c[i][j] = new int[n], load[i][j] = new int[n], max_load[i][j] = new int[n];
				for (int k = 0; k < n; k++)
					c[i][j][k] = load[i][j][k] = max_load[i][j][k] = 0;
			}
		}

		allocated_d = new int*[n];
		for (int i = 0; i < n; i++) {
			allocated_d[i] = new int[p];
			for (int j = 0; j < p; j++) 
				allocated_d[i][j] = 0;
		}

		flatten_c = new int[m*p*n];
		memset(flatten_c, 0,m*p*n*sizeof(int));

		sp = new long long[p];
		memset(sp, 0, p*sizeof(long long));

		dp = new long long[p];
		memset(dp, 0, p*sizeof(long long));

		dn = new long long[n];
		memset(dn, 0, n*sizeof(long long));

		slack_n = new long long[n];
		memset(slack_n, 0, n*sizeof(long long));
	}

	int id(int i, int j,int k) const {
		return i*p*n + j*n + k;
	}

	// this function checks if we can archived obj1
	bool check(int obj1) {
		// allocating temporary array
		bool ret = true;
		int* remaining_sp = new int[p];
		for (int i = 0; i < n; i++) 
			for (int j = 0; j < p; j++) 
				allocated_d[i][j] = 0;

		// initialization
		for (int i = 0; i < p; i++) remaining_sp[i] = sp[i];
		for (int i = 0; i < n; i++) slack_n[i] = 0;

		// greedy 
		for (int i = 0; i < n; i++) {
			//to maximize slack_n[i]
			slack_n[i] = dn[i];
			if (dn[i] <= obj1) {
				// if the demand of a customer less than obj1 then we do not sell anything
				continue;
			} else {
				// otherwise, we have to sell dn[i] - obj1 unit
				// tried to sell any thing we have first
				for (int j = 0; j < p; j++) {
					allocated_d[i][j] = min(min(1LL*d[i][j], 1LL*remaining_sp[j]), 1LL*slack_n[i] - obj1);
					remaining_sp[j] -= allocated_d[i][j];
					slack_n[i] -= allocated_d[i][j];
				}

				// if we still have enough goods, then no thing to worry about
				if ( slack_n[i] == obj1 ) 
					continue;
					
				// otherwise, we try to switch goods with customers [0...i-1]
				for (int j = 0; j < p; j++) 
					// if customer i still need product j
					if (allocated_d[i][j] < d[i][j]) {
						// try to exchange product i to others
						for (int jj = 0; jj < p; jj++) 
							if (remaining_sp[jj] != 0 ) // find another product that we still have 
								for (int ii = 0; ii < i; ii++) 
									if ( allocated_d[ii][j] > 0 &&  // and exchanged customer was supplied that product
									  d[ii][jj]-allocated_d[ii][jj] > 0) {// and they still need our product j
										// exchange it
										long long exchanged_unit = min(1LL*remaining_sp[jj], 1LL*d[i][j] - allocated_d[i][j]);
										exchanged_unit = min(exchanged_unit, 
											min(1LL*allocated_d[ii][j], 1LL*d[ii][jj]-allocated_d[ii][jj]));
										exchanged_unit = min(exchanged_unit, 1LL*slack_n[i] - obj1);

										
										remaining_sp[jj] -= exchanged_unit;
										allocated_d[ii][j] -= exchanged_unit;
										allocated_d[i][j] += exchanged_unit;
										allocated_d[ii][jj] += exchanged_unit;
										slack_n[i] -= exchanged_unit;
									}
					}

				// if we cannot allocate goods to customer i, we cannot archived obj1
				if (slack_n[i] != obj1) 
					ret = false;
			}
		}
		delete[] remaining_sp;
		return ret;
	}

	// complexity O(n^2*p^2*log(max(obj1)))
	void minimize_max_slack() {
		int l = -1, r = max_obj1 + 1;
		while ( l < r-1 ) {
			int mid = (l+r) >> 1;

			if (check(mid)) r = mid;
			else l = mid;
		}
		obj1 = r;
		check(obj1);
	}

};

#endif