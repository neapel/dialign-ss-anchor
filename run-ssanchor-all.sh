#!/bin/bash

find bb3_release -iname '*.tfa' \
| time xargs -P 4 -n 1 ./run-ssanchor.sh 2>&1 \
| tee run-ssanchor-all.log
