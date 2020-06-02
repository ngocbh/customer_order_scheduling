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
RESULT_FORDER='./results/greedy'

def run_greedy(filename, input_dir, output_dir):
	input_path = join(input_dir, filename)
	output_path = join(output_dir, 'greedy/{}'.format(filename))
	create_dir(output_path)
	print(filename)
	process = Popen(['./bin/cosp_greedy', '-i', input_path, '-o', output_path], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	print(stdout)


def run():
	for filename in listdir(DATA_FOLDER):
		if filename[0] == '.':
			continue
		run_greedy(filename, DATA_FOLDER, RESULT_FORDER)
		# break

if __name__ == '__main__':
	run()
