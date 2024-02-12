#!/bin/bash
#int run_dETdeta(int nproc = 0, string tag = "", int datormc = 0 (1 for MC), int debug = 0, int nevt = 0, int correct = 1, int zs = 0, int upweightb = 0, int doupweight = 0)

root -b -q 'run_dETdeta.C(0,"debug",1,1,10,1,0,1,1)'
