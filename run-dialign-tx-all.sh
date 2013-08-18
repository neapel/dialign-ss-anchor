#!/bin/bash

find bb3_release -iname '*.tfa' \
| time xargs -P 6 -n 1 ./run-dialign-tx.sh 2>&1 \
| tee run-ssanchor-all.log
