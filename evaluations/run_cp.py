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


DATA_FOLDER='./data'
RESULT_FORDER='./results/cp'

def run_naive(filename, input_dir, output_dir):
	input_path = join(input_dir, filename)
	output_path = join(output_dir, 'naive/{}'.format(filename))
	process = Popen(['g++', '-std=c++11', '-F/Library/Frameworks', 
		'-framework', 'gecode', '-o', './cosp/cp/cosp_naive.exe', './cosp/cp/cosp_naive.cpp'], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	process = Popen(['./cosp/cp/cosp_naive.exe', '-i', input_path, '-o', output_path], stdout=PIPE, universal_newlines=True)
	(stdout, stderr) = process.communicate()
	print(stdout)
	


def run():
	for filename in listdir(DATA_FOLDER):
		if filename[0] == '.':
			continue
		run_naive(filename, DATA_FOLDER, RESULT_FORDER)
		# break

if __name__ == '__main__':
	run()

