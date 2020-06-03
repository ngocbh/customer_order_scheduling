import numpy as np
# cos_1000_100
# load[m][p][n]
data_dir = '../data/'
result_dir = '../results/mip/two_phase/'

def check_result(data_file_name='cos_1000_1000'):
	# data
	with open(data_dir+data_file_name) as f:
		lines = [line for line in f.read().split('\n') if line != ''] 
	
	[N, P, M] = [int(inp) for inp in lines[0].split()]
	print('M, P, N = ', M, P, N)
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

	# result
	with open(result_dir+data_file_name) as f:
		lines = [line for line in f.read().split('\n') if line != ''] 
		
	parts = lines[0].split()
	obj1 = int(parts[0])
	obj2 = int(parts[1])

	matrix = []
	for i in range(1, len(lines)):
		matrix.append([int(x) for x in lines[i].split()])

	load = np.asarray(matrix).reshape(M, P, N)
	non_zero_count = 0
	for m in range(M):
		for p in range(P):
			for n in range(N):
				if load[m][p][n] != 0:
					non_zero_count += 1
	print('Non-zero values: ', non_zero_count)
	print('Non-zero percent: ', non_zero_count/146200)
	# Check if constraint 1 is satisfied
	for m in range(M):
		for p in range(P):
			sum_load = 0
			for n in range(N):
				sum_load += load[m][p][n]
			if sum_load > S[m][p]:
				print('False at Constraint #1')
				print('At m = {}, p = {}'.format(m, p))
				return False

	# check if constraint 2 is satisfied and obj1 is correct
	losses = []
	for n in range(N):
		loss = 0
		for p in range(P):
			sum_load = 0
			for m in range(M):
				sum_load += load[m][p][n]
			if sum_load > D[n][p]:
				print('False at Constraint #2')
				print('At n = {}, p = {}'.format(m, p))
				print('D[{}][{}] = {}'.format(n, p, D[n][p]))
				print('Total products type {} for customer {} = {}'.format(p, n, sum_load))
				return False
			loss += D[n][p] - sum_load
		losses.append(loss)
	if max(losses) != obj1:
		print('False at Objective #1')
		return False

	return True
	
def main():
	data_file_name='cos_1000_1000'
	print(check_result(data_file_name))
	# print(check_result())

if __name__ == '__main__':
	main()
