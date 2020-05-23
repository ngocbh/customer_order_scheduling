import random as rd
import sys, codecs, os
from os import listdir
from os.path import isfile, join, isdir
from shutil import copyfile
from subprocess import Popen, PIPE

# mien gia tri 5, 10, 20
# M*N*P = 20
# M*N*P = 50
# M*N*P = 100
# M*N*P = 500
# M*N*P = 1000
N = 20
P = 90
M = 25
max_variable_values = [20, 50, 100, 1000]
samples = [1]
# for max_variable_value in max_variable_values:
	# for sample in samples:
def generate_data():
	ret = ''
	max_variable_value = 1000
	f = open('./examples/mycpp/cos_{}_{}'.\
		# format(N*P*M, max_variable_value, sample), 'a')
		format(N*P*M, max_variable_value), 'w')

	f.write('{} {} {}\n'.format(N, P, M))
	ret += '{} {} {}\n'.format(N, P, M)

	for m in range(M):
		for p in range(P):
			f.write(str(rd.randint(0, max_variable_value)))
			ret += str(rd.randint(0, max_variable_value))
			if p != P - 1:
				f.write(' ')
				ret += ' '
		f.write('\n')
		ret += '\n'

	for n in range(N):
		for p in range(P):
			f.write(str(rd.randint(0, max_variable_value)))
			ret += str(rd.randint(0, max_variable_value))
			if p != P - 1:
				f.write(' ')
				ret += ' '
		f.write('\n')
		ret += '\n'

	for m in range(M):
		for p in range(P):
			for n in range(N):
				f.write(str(rd.randint(0, max_variable_value)))
				ret += str(rd.randint(0, max_variable_value))
				if n != N - 1:
					f.write(' ')
					ret += ' ' 
			if m != M - 1 or p != P - 1:
				f.write('\n')
				ret += '\n'
	return ret

def run():
	process = Popen(['make', 'run', 'SOURCE=examples/mycpp/cosp-mip-naive.cc'], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	lines = stdout.split('\n')
	for i, line in enumerate(lines):
		if 'bin/cosp-mip-naive' in line:
			return lines[i+1], lines[i+2]

def check():
	while True:
		input_data = generate_data()
		break
		a, b = run()
		print(input_data)
		print(a,b)
		if a != b:
			return

if __name__ == '__main__':
	check()