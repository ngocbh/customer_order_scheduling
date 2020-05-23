#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
*  Filename : evaluate_cp.py
*  Description : 
*  Created by ngocjr7 on [2020-05-19 11:05]	
"""

import sys, codecs, os
from os import listdir
from os.path import isfile, join, isdir
from shutil import copyfile
from subprocess import Popen, PIPE

from utils import create_dir


DATA_FOLDER='./data'
RESULT_FORDER='./results/cp'

def run_naive(filename, input_dir, output_dir):
	input_path = join(input_dir, filename)
	output_path = join(output_dir, 'naive/{}'.format(filename))
	create_dir(output_path)
	process = Popen(['./bin/cosp_cp_naive', '-i', input_path, '-o', output_path], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	print(stdout)
	
def run_greedy(filename, input_dir, output_dir):
	input_path = join(input_dir, filename)
	output_path = join(output_dir, 'greedy/{}'.format(filename))
	create_dir(output_path)
	process = Popen(['./bin/cosp_cp_greedy', '-i', input_path, '-o', output_path], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	print(stdout)


def run():
	for filename in listdir(DATA_FOLDER):
		if filename[0] == '.':
			continue
		run_naive(filename, DATA_FOLDER, RESULT_FORDER)
		run_greedy(filename, DATA_FOLDER, RESULT_FORDER)
		# break

if __name__ == '__main__':
	run()

