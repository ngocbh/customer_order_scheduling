import numpy as np
# import resource, sys
# resource.setrlimit(resource.RLIMIT_STACK, (2**29,-1))
# import sys
# sys.setrecursionlimit(10**9)

########################### INPUT ###############################
with open('ao_n_2_p_2_m_3.txt') as f:
	lines = f.readlines()

[N, P, M] = [int(inp) for inp in lines[0].split()]
# print(M, P, N)
S = []
for m in range(M):
	S.append([int(inp) for inp in lines[1 + m].split()])

D = []
for n in range(N):
	D.append([int(inp) for inp in lines[1 + M + n].split()])

c = []
for line in lines[1 + M + N:]:
	c.append([int(inp) for inp in line.split()])

c = np.asarray(c)
c = c.reshape(M, P, N)
#################################################################
# X(m,p,n): So luong san pham p nha cung cap m cung cap cho n



def ao_brute_force(M, P, N, S, D, c, x):
	# Constraints
	# C1
	for m in range(M):
		for p in range(P):
			sum_n_x = 0
			for n in range(N):
				sum_n_x += x[m][p][n]
			if S[m][p] < sum_n_x:
				return None, None

	# Optimization
	# C2
	# O1
	max_con_thieu = float('-inf')
	for n in range(N):
		con_thieu = 0
		for p in range(P):
			sum_m_x = 0
			for m in range(M):
				sum_m_x += x[m][p][n]
			if D[n][p] < sum_m_x:
				return None, None
			# print('sum_m_x[{}][{}]= {}'.format(n, p, sum_m_x) )
			# print('D[{}][{}] = {}'.format(n, p, D[n][p]))
			con_thieu += D[n][p] - sum_m_x
		if con_thieu > max_con_thieu:
			max_con_thieu = con_thieu

	# O2	
	total_cost = 0
	for n in range(N):
		for p in range(P):
			for m in range(M):
				total_cost += c[m][p][n] * x[m][p][n]

	# print('returned max_con_thieu, total_cost = ', max_con_thieu, total_cost)
	return max_con_thieu, total_cost

def main():
	max_variable_value = 3
	min_o1 = float('inf')
	min_o2 = float('inf')
	opt_y = None

	x = [0] * (M*N*P)
	for j in range(4**(M*N*P)-1):	
		# print(x)
		i = len(x) - 1
		while x[i] == max_variable_value:
			i -= 1
			if i == -1:
				return 
		x[i] += 1
		if i != len(x) - 1:
			for j in range(i+1, len(x)):
				x[j] = 0

		y = np.asarray(x)
		y = y.reshape(M, P, N)
		max_con_thieu, total_cost = ao_brute_force(M, P, N, S, D, c, y)
		# print('max con thieu = ', max_con_thieu)
		if max_con_thieu != None:
			if max_con_thieu < min_o1:
				min_o1 = max_con_thieu
				min_o2 = total_cost
				opt_y = y
			if max_con_thieu == min_o1 and total_cost < min_o2:
				min_o2 = total_cost
				opt_y = y
		# print('opt_y = ', opt_y)
	print('min_o1 = ', min_o1)
	print('min_o2 = ', min_o2)
	print('opt_y = ', opt_y)

if __name__ == '__main__':
	main()