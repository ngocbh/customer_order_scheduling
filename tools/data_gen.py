import random as rd


# mien gia tri 5, 10, 20
# M*N*P = 20
# M*N*P = 50
# M*N*P = 100
# M*N*P = 500
# M*N*P = 1000
N = 2
P = 5
M = 2
max_variable_values = [20, 50, 100, 1000]
samples = [1]
for max_variable_value in max_variable_values:
	for sample in samples:
		f = open('./cos_data/cos_{}_{}'.\
			# format(N*P*M, max_variable_value, sample), 'a')
			format(N*P*M, max_variable_value), 'a')

		f.write('{} {} {}\n'.format(N, P, M))

		for m in range(M):
			for p in range(P):
				f.write(str(rd.randint(0, max_variable_value)))
				if p != P - 1:
					f.write(' ')
			f.write('\n')

		for n in range(N):
			for p in range(P):
				f.write(str(rd.randint(0, max_variable_value)))
				if p != P - 1:
					f.write(' ')
			f.write('\n')

		for m in range(M):
			for p in range(P):
				for n in range(N):
					f.write(str(rd.randint(0, max_variable_value)))
					if n != N - 1:
						f.write(' ')
				if m != M - 1 or p != P - 1:
					f.write('\n')
