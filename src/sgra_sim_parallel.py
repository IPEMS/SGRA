import multiprocessing
import subprocess
import argparse
import numpy as np
import os
import shlex
import atexit
import sys
from threading import Thread

ON_POSIX = 'posix' in sys.builtin_module_names
 
        

######################################################################
# UTILITY FUNCTIONS
######################################################################
def enqueue_output(out,s):
    for line in iter(out.readline, b''):
        print '[' + s + ']: ' + line
	sys.stdout.flush()
    out.close()

def _kill(instance):
	instance.kill()

#TODO: -n
def sim(cmd):
	print cmd
	#_instance = subprocess.Popen(cmd[0],shell=True)
	_instance = subprocess.Popen(cmd[0],shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,bufsize=1,close_fds=ON_POSIX)

        t_out = Thread(target=enqueue_output, args=(_instance.stdout, cmd[1] + ' out'))
        t_out.daemon = True
        t_out.start()

        t_err = Thread(target=enqueue_output, args=(_instance.stderr, cmd[1] + ' err'))
        t_err.daemon = True
        t_err.start()

	atexit.register(_kill,_instance)
	_instance.wait()

######################################################################
# MAIN
######################################################################

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Spawn the simulator in parallel.')

	parser.add_argument('-f', '--filename', type=str, required=True, help='input filename')
	parser.add_argument('-n', '--num', type=int, required=False, default=1, help='number of times to run each file in the input file')
	parser.add_argument('-N', '--pool', type=int, required=False, default=multiprocessing.cpu_count(), help='number of workers')
	parser.add_argument('-o', '--output', type=str, required=True, help='output folder for the simulations')

	args = parser.parse_args()

	_in = np.loadtxt(args.filename,dtype=np.str)
	
	FOLDER = os.path.join(os.path.split(args.filename)[0],_in[0])
	FILES = np.delete(_in,0)

	INPUT = []

	for f in FILES:
		INPUT.append(('./sim -f ' + os.path.join(FOLDER,f) + ' -o ' + os.path.join(args.output, f.split('.')[0]) + os.path.sep, f.split(os.path.sep)[-1]))

	pool = multiprocessing.Pool(processes=args.pool)
	pool.map(sim, INPUT)

