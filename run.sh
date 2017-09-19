#!/bin/bash
date
time ./counter.out dots.txt
date
time ./counter_blas.out dots.txt
date
time ./counter_ATLAS.out dots.txt
date
time ./counter_LA.out dots.txt
date
time ./counter_KAL.out dots.txt
date