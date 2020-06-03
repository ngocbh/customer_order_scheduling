import os
import matplotlib.pyplot as plt
import xlsxwriter
import pprint 
pp = pprint.PrettyPrinter(indent=4)
import collections

data_dir = '../data/'

def get_data(data_name):
	data_path = data_dir + data_name
	with open(data_path) as f:
		line_1 = f.read().split('\n')[0]
		[N, P, M] = [int(inp) for inp in line_1.split()]
	return N, P, M

def get_time_1(s):
	start = '('
	end = ')'
	try:
		return float(s[s.find(start)+len(start):s.find(end)-2])
	except ValueError:
		return None

def get_time_2(s):
	for w in s.split():
		if w.isdigit():
			return int(w)

def read_results(dir_path, output_xlsx_name='experimental_results.xlsx'):
	results = {}
	excel_results = {}
	# all_types = ['bf', r'cp\greedy', r'cp\naive', r'greedy\greedy', r'mip\greedy', r'mip\naive', r'mip\two_phase', r'mip\weighted_sum']
	first_algo_types = {r'cp\greedy', r'cp\naive', r'greedy\greedy', r'mip\two_phase', r'ls\greedy'}
	second_algo_types = {r'mip\greedy', r'mip\naive', r'mip\weighted_sum'}
	other_algo_types = {'bf'}



	for path, subdirs, files in os.walk(dir_path):
		for name in files:
			with open(os.path.join(path, name), 'r') as f:
				run_time = None
				obj1 = None
				obj2 = None
				[max_size, max_value] = name.split('_')[1:3]
				max_size = int(max_size)
				max_value = int(max_value)
				
				algo_type = path[path.find('results\\')+len('results\\'):]

				if 'stat' not in name:
					try:
						values = f.read().split()
						if len(values) > 1:
							obj1 = int(values[0])
							obj2 = int(values[1])
						# [obj1, obj2] = [int(x) for x in f.read().split('\n')[0].split()]
					except IndexError:
						print(values)
						print(path)
						print(name)
					except ValueError:
						print('aaaa')
				else:
					if algo_type in first_algo_types:
						# print('a')
						run_time = get_time_1(f.read())
					elif algo_type in second_algo_types:
						# print('b')
						run_time = get_time_2(f.read())
					else:
						pass

				if 'stat' in name:
					name = name[:-len('stat')-1]
				N, P, M = get_data(name)

				# if run_time == None:
				# 	results[algo_type, max_size, max_value] = [obj1, obj2]
				# 	excel_results[(name, N, P, M)] = (algo_type, obj1, obj2)
				# elif results[algo_type, max_size, max_value][0] != None:
				# 	results[algo_type, max_size, max_value].append(run_time)
				# 	a_list = list(excel_results[(name, N, P, M)])
				# 	a_list.append(run_time)
				# 	excel_results[(name, N, P, M)] = tuple(a_list)

				if run_time == None:
					results[algo_type, max_size, max_value] = [obj1, obj2]
					if (name, N, P, M) not in excel_results:
						excel_results[(name, N, P, M)] = {algo_type:(obj1, obj2)}
					# if algo_type in excel_results[(name, N, P, M)]:
					# 	print('naniiii')
					# 	print('algo_type = ', algo_type)
					# 	excel_results[(name, N, P, M)] = {algo_type:(obj1, obj2)}
					# else:
					# 	print(algo_type)
					else:
						excel_results[(name, N, P, M)][algo_type] = (obj1, obj2)
					
				elif results[algo_type, max_size, max_value][0] != None:
					results[algo_type, max_size, max_value].append(run_time)
					try:
						a_list = list(excel_results[(name, N, P, M)][algo_type])
						a_list.append(run_time)
						excel_results[(name, N, P, M)][algo_type] = tuple(a_list)
					except TypeError:
						print('name = ', name)
						print('algo_type = ', algo_type)
						print(excel_results[name][algo_type])

	# pp.pprint(excel_results)
	keys = list(excel_results.keys())
	keys.sort(key=lambda x:x[1]*x[2]*x[3])
	new_results = {}
	for key in keys:
		new_results[key] = excel_results[key]
	excel_results = new_results

	# Create xlsx file
	workbook = xlsxwriter.Workbook(output_xlsx_name)
	worksheet = workbook.add_worksheet()

	worksheet.write_string(1, 0, 'Data')
	worksheet.write_string(1, 1, 'N')
	worksheet.write_string(1, 2, 'P')
	worksheet.write_string(1, 3, 'M')

	algo_type_list = [r'cp\naive', r'mip\weighted_sum', r'mip\two_phase', r'cp\greedy', r'greedy\greedy', r'ls\greedy',  r'mip\greedy']
	t = 1
	for i, algo_type in enumerate(algo_type_list):
		worksheet.write_string  (0, 3 + t, algo_type)
		worksheet.write_string  (1, 3 + t, 'Obj1')
		worksheet.write_string  (1, 3 + t + 1, 'Obj2')
		worksheet.write_string  (1, 3 + t + 2, 'Time')
		t += 3

	row = 2
	# Should sort dict by keys
	for keys, values in excel_results.items():
		worksheet.write_string(row, 0, keys[0])
		worksheet.write_number(row, 1, keys[1])
		worksheet.write_number(row, 2, keys[2])
		worksheet.write_number(row, 3, keys[3])
		t = 4
		for algo_type in algo_type_list:
			for small_key, small_value in values.items():
				if small_key == algo_type:
					# print('small_key = ', small_key)
					# print('algo_type = ', algo_type)
					try:
						worksheet.write_number(row, t, small_value[0])
						worksheet.write_number(row, t+1, small_value[1])
						worksheet.write_number(row, t+2, small_value[2])
					except IndexError:
						pass
					except TypeError:
						pass
			t += 3
		row += 1

	non_none_results = {}
	# print(len(results))
	# print(results)
	# print(len(excel_results))
	# print(excel_results)

	workbook.close()
	# Make new results
	for key, value in results.items():
		if len(value) >= 3:
			non_none_results[key] = value

	return non_none_results

def plot_size_time(dir_path):
	results = read_results(dir_path)
	# print(results)
	

	def create_size_dict(size_time_dict, time_checking_types, index):
		for keys, values in results.items():
			max_size = keys[1]
			run_time = values[index]
			if keys[0] in time_checking_types:
				if max_size in size_time_dict:
					if keys[2] in size_time_dict[max_size]:
						if type(size_time_dict[max_size][keys[2]][0]) == tuple:
							a_list = list(size_time_dict[max_size][keys[2]])

						else:
							a_list = [tuple(size_time_dict[max_size][keys[2]])]

						a_list.append((keys[0], run_time))
						size_time_dict[max_size][keys[2]] = tuple(a_list)
					else:
						size_time_dict[max_size][keys[2]] = (keys[0], run_time)
				else:
					size_time_dict[max_size] = {keys[2] : (keys[0], run_time)}
	
	size_time_dict = {}
	time_checking_types = {r'cp\naive', r'mip\weighted_sum', r'mip\two_phase', r'cp\greedy', r'greedy\greedy', r'ls\greedy',  r'mip\greedy'}
	create_size_dict(size_time_dict, time_checking_types, 2)

	size_obj1_dict = {}
	obj1_checking_types = {r'greedy\greedy', r'mip\two_phase'}
	create_size_dict(size_obj1_dict, obj1_checking_types, 0)

	size_obj2_dict = {}
	obj2_checking_types = {r'cp\greedy', r'greedy\greedy', r'ls\greedy',  r'mip\greedy'}
	create_size_dict(size_obj2_dict, obj2_checking_types, 1)


	# pp.pprint(size_time_dict)
	# pp.pprint(size_obj1_dict)
	size_time_dict = \
	{   12: {   3: (   ('cp\\greedy', 0.073),
                   ('cp\\naive', 0.307),
                   ('greedy\\greedy', 0.112),
                   ('ls\\greedy', 1.018),
                   ('mip\\greedy', 7),
                   ('mip\\two_phase', 10.605),
                   ('mip\\weighted_sum', 6)),
            },
    20: {   20: (   ('cp\\greedy', 38.065),
                    ('cp\\naive', 19999.8),
                    ('greedy\\greedy', 0.185),
                    ('ls\\greedy', 1.995),
                    ('mip\\greedy', 8),
                    ('mip\\two_phase', 9.746),
                    ('mip\\weighted_sum', 7)),
            },
    30: {   5: (   ('cp\\greedy', 156.107),
                   ('cp\\naive', 19999.7),
                   ('greedy\\greedy', 0.236),
                   ('ls\\greedy', 3.99),
                   ('mip\\greedy', 7),
                   ('mip\\two_phase', 10.924),
                   ('mip\\weighted_sum', 10))},
    50: {   50: (   ('cp\\naive', 20000.5),
                    ('greedy\\greedy', 0.178),
                    ('ls\\greedy', 66.821),
                    ('mip\\greedy', 8),
                    ('mip\\two_phase', 11.506),
                    ('mip\\weighted_sum', 7)),
            },
    100: {   
             100: (   ('cp\\naive', 19999.7),
                      ('greedy\\greedy', 0.253),
                      ('ls\\greedy', 159.452),
                      ('mip\\greedy', 11),
                      ('mip\\two_phase', 15.392),
                      ('mip\\weighted_sum', 14)),
             },
    500: {   20: (   ('cp\\greedy', 19999.7),
                     ('cp\\naive', 19999.7),
                     ('greedy\\greedy', 0.454),
                     ('ls\\greedy', 102.744),
                     ('mip\\greedy', 25),
                     ('mip\\two_phase', 37.725),
                     ('mip\\weighted_sum', 16)),
             },
    1000: {   
              50: (   ('cp\\naive', 19999.6),
                      ('greedy\\greedy', 1.274),
                      ('ls\\greedy', 25796.9),
                      ('mip\\greedy', 37),
                      ('mip\\two_phase', 65.064),
                      ('mip\\weighted_sum', 45)),
              },
    42484: {   
               100: (   ('greedy\\greedy', 114.206),
                        ('mip\\greedy', 2946),
                        ('mip\\two_phase', 7136.8),
                        ('mip\\weighted_sum', 18378)),
               },
    146200: {   
                50: (   ('greedy\\greedy', 299.381),
                        ('mip\\greedy', 26100),
                        ('mip\\two_phase', 57750.3)),
                },
    228735: {   
                50: (   ('greedy\\greedy', 169.05),
                        ('mip\\two_phase', 87001.1),
                        ('mip\\weighted_sum', 7945)),
                },
    274344: {   
                50: ('greedy\\greedy', 491.051)},
    2171169: {   
                 50: ('greedy\\greedy', 7217.42)}}

	size_obj1_dict = \
{   12: {3: (('greedy\\greedy', 1), ('mip\\two_phase', 1))},
    20: {   1000: (('greedy\\greedy', 84), ('mip\\two_phase', 84))},
    30: {5: (('greedy\\greedy', 5), ('mip\\two_phase', 5))},
    50: {   
            1000: (('greedy\\greedy', 1544), ('mip\\two_phase', 1544))},
    100: {   
             1000: (('greedy\\greedy', 2034), ('mip\\two_phase', 2025))},
    500: {   
             1000: (('greedy\\greedy', 0), ('mip\\two_phase', 0))},
    1000: {  
              100: (('greedy\\greedy', 38), ('mip\\two_phase', 35))},
    42484: {   
               10000: (('greedy\\greedy', 260348), ('mip\\two_phase', 260348))},
    146200: {   
                500: (('greedy\\greedy', 5750), ('mip\\two_phase', 5750))},
    228735: {   
                500: (('greedy\\greedy', 0), ('mip\\two_phase', 0))},
    274344: {   
                500: ('greedy\\greedy', 25924)},
    2171169: {   
                 500: ('greedy\\greedy', 33513)}}
	
	# pp.pprint(size_obj2_dict)
	size_obj2_dict = \
	{   12: {   3: (   ('cp\\greedy', 12),
                   ('greedy\\greedy', 12),
                   ('ls\\greedy', 12),
                   ('mip\\greedy', 12)),
            },
    20: {   
            50: (   ('cp\\greedy', 6522),
                    ('greedy\\greedy', 6462),
                    ('ls\\greedy', 6462),
                    ('mip\\greedy', 6462)),
		},
    30: {   5: (   ('cp\\greedy', 188),
                   ('greedy\\greedy', 196),
                   ('ls\\greedy', 188),
                   ('mip\\greedy', 188))},
    50: {   50: (   ('greedy\\greedy', 4661),
                    ('ls\\greedy', 3643),
                    ('mip\\greedy', 3608))},
    100: {   
             100: (   ('greedy\\greedy', 44099),
                      ('ls\\greedy', 32533),
                      ('mip\\greedy', 32453))},
    500: {   
             1000: (   ('cp\\greedy', 10471430),
                       ('greedy\\greedy', 3988084),
                       ('ls\\greedy', 3972205),
                       ('mip\\greedy', 3764896))},
    1000: {   
              50: (   ('greedy\\greedy', 28028),
                      ('ls\\greedy', 23426),
                      ('mip\\greedy', 19899)),
              },
    42484: {   
               100: (('greedy\\greedy', 909526), ('mip\\greedy', 159697))},
    146200: {   
                50: (('greedy\\greedy', 414111), ('mip\\greedy', 154699))},
    228735: {   
                50: ('greedy\\greedy', 80631)},
    274344: {   
                50: ('greedy\\greedy', 519995)},
    2171169: {   
                 50: ('greedy\\greedy', 1059825)}}

	def plot_dict(size_time_dict, time_checking_types, name):
		# print('size_time_dict = ', size_time_dict)
		time_checking_types = list(time_checking_types)
		plot_list = []
		for i in range(len(time_checking_types)):
			plot_list.append([])

		for key, value in size_time_dict.items():
			# Should sort by size of small value???
			idx = -1
			for small_key, small_value in value.items():
				# if (len(small_value)) == len(time_checking_types):
				if True: # Need to change this
					try:
						for (this_type, run_time) in small_value:
							for i in range(len(time_checking_types)):
								if this_type == time_checking_types[i]:
									plot_list[i].append((key, run_time))
					except ValueError:
						this_type = small_value[0]
						run_time = small_value[1]
						for i in range(len(time_checking_types)):
							if this_type == time_checking_types[i]:
								plot_list[i].append((key, run_time))
					break

		for i, small_plot_list in enumerate(plot_list):
			# print('small_plot_list = ', small_plot_list)
			X = [point[0] for point in small_plot_list]
			Y = [point[1] for point in small_plot_list]

			plt.plot(X, Y, 'o', label=time_checking_types[i])
		plt.legend()
		plt.savefig('{}_all.png'.format(name))
		plt.close()

		for i, small_plot_list in enumerate(plot_list):
			# print(small_plot_list)
			X = [point[0] for point in small_plot_list if point[0] <= 1000]
			Y = [point[1] for point in small_plot_list if point[0] <= 1000]

			plt.plot(X, Y, 'o', label=time_checking_types[i])
		plt.legend()
		plt.savefig('{}_below_1000.png'.format(name))
		plt.close()
		# plt.show()

		for i, small_plot_list in enumerate(plot_list):
			# print(small_plot_list)
			X = [point[0] for point in small_plot_list if point[0] > 1000]
			Y = [point[1] for point in small_plot_list if point[0] > 1000]

			plt.plot(X, Y, 'o', label=time_checking_types[i])
		plt.legend()
		plt.savefig('{}_upper_1000.png'.format(name))
		plt.close()

		# plt.show()

	plot_dict(size_time_dict, time_checking_types, 'time')
	plot_dict(size_obj1_dict, obj1_checking_types, 'obj1')
	plot_dict(size_obj2_dict, obj2_checking_types, 'obj2')



def main():
	dir_path = '../results'
	plot_size_time(dir_path)

if __name__ == '__main__':
	main()