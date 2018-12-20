##

# run all biig crush tests

nproc = 8
njobs = 10
option = "-rabbit"
rng_name = "mixmax8"
outPrefix = "/home/moneta/cernbox/root/tests/mixmax/"

import os
import sys
import multiprocessing as mp

def func(pos,n) :
    print "Running for process",pos
    for i in range(0,n):
#        option = " -rabbit"
#        gname = " mixmax17"
        cmd = "./testu01_bigcrush " + option + " " + rng_name
        rid = pos * njobs + i + 1
        outLogFile = outPrefix + "/rabbit/test_2E30/test_" + rng_name + ".run" + str(rid) + ".log"
        cmd += " > " + outLogFile
        print "running",cmd
        ret = os.system(cmd)
        if (ret != 0) :
            sys.exit()


processes = [mp.Process(target=func, args=(x,njobs) )  for x in range(nproc)]

# Run processes
for p in processes:
    p.start()

# Exit the completed processes
for p in processes:
    p.join()

