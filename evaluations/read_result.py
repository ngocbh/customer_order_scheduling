import os
import matplotlib.pyplot as plt

def get_time_1(s):
	start = '('
	end = ')'
	try:
		return float(s[s.find(start)+len(start):s.find(end)-2])
	except ValueError:
		return -1

def get_time_2(s):
	for w in s.split():
		if w.isdigit():
			return int(w)

def read_results(dir_path):
	results = {}
	for path, subdirs, files in os.walk(dir_path):
		for name in files:
			with open(os.path.join(path, name), 'r') as f:
				try:
					run_time = -1
					obj1 = -1
					obj2 = -1
					[max_size, max_value] = name.split('_')[1:3]

					if 'mip' in path:
						type = 'mip_' + path.split('\\')[-1]
						if 'stat' in name:
							run_time = get_time_2(f.read())
						else:
							[obj1, obj2] = [int(x) for x in f.readline().split()]
					elif 'cp' in path:
						type = 'cp_' + path.split('\\')[-1]
						if 'stat' in name:
							run_time = get_time_1(f.read())
						else:
							[obj1, obj2] = [int(x) for x in f.readline().split()]
					elif 'ls' in path:
						type = 'ls_' + path.split('\\')[-1]
					elif 'bf' in path:
						type = 'bf'
						[obj1, obj2] = [int(x) for x in f.readline().split()]
					else: # greedy
						type = 'greedy_' + path.split('\\')[-1]
						if 'stat' in name:
							run_time = get_time_1(f.read())
						else:
							[obj1, obj2] = [int(x) for x in f.readline().split()]
					if run_time == -1 or type == 'bf':
						results[type, max_size, max_value] = [obj1, obj2]
					elif results[type, max_size, max_value][0] != None:
						results[type, max_size, max_value].append(run_time)
				except:
					if (type, max_size, max_value) not in results:
						results[type, max_size, max_value] = [None, None, None]
	for keys, values in results.items():
		print(keys)
		print(values)
	return results


def main():
	dir_path = '../results'
	results = read_results(dir_path)
	# print(results)
	mip_greedy_size_time = []
	mip_naive_size_time = []
	cp_greedy_size_time = []
	cp_naive_size_time = []
	
	for keys, values in results.items():
		max_size = int(keys[1])
		# print('keys = ', keys)
		# print('values = ', values)
		time_ = values[-1]
		if keys[0] == 'mip_greedy':
			mip_greedy_size_time.append((max_size, time_))
		elif keys[0] == 'mip_naive':
			mip_naive_size_time.append((max_size, time_))
		elif keys[0] == 'cp_greedy':
			cp_greedy_size_time.append((max_size, time_))
		elif keys[0] == 'cp_naive':
			cp_naive_size_time.append((max_size, time_))

	# print(results['cp_greedy', '50', '50'])

	mip_greedy_size_time = [(x,y) for (x,y) in mip_greedy_size_time if x != None and y != None]
	mip_naive_size_time = [(x,y) for (x,y) in mip_naive_size_time if x != None and y != None]
	cp_greedy_size_time = [(x,y) for (x,y) in cp_greedy_size_time if x != None and y != None]
	cp_naive_size_time = [(x,y) for (x,y) in cp_naive_size_time if x != None and y != None]

	mip_greedy_size_time.sort(key=lambda x:x[0])
	mip_naive_size_time.sort(key=lambda x:x[0])
	cp_greedy_size_time.sort(key=lambda x:x[0])
	cp_naive_size_time.sort(key=lambda x:x[0])
	plt.plot([x[0] for x in mip_greedy_size_time], [x[1] for x in mip_greedy_size_time], label="mip_greedy")
	plt.plot([x[0] for x in mip_naive_size_time], [x[1] for x in mip_naive_size_time], label="mip_naive")
	plt.plot([x[0] for x in cp_greedy_size_time], [x[1] for x in cp_greedy_size_time], label="cp_greedy")
	# plt.plot([x[1] for x in cp_naive_size_time], [x[0] for x in cp_naive_size_time], label="cp_naive")
	print([x[0] for x in cp_greedy_size_time])
	print([x[1] for x in cp_greedy_size_time])
	
	plt.legend()
	plt.show()

if __name__ == '__main__':
	main()