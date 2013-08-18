#!/bin/bash

printf "seq\trun\tblock\tsp\ttc" > run-baliscore-all.dat
find bb3_release -iname '*.dialign-tx.msf' \
| time xargs -n 1 ./run-baliscore.sh \
| tee -a run-baliscore-all.dat
