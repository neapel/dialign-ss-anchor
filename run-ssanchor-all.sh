#!/bin/bash

find bb3_release -iname '*.tfa' \
| time xargs -I {} -P 6 -n 1 ./run-ssanchor.sh {} $1 2>&1 \
| tee run-ssanchor-all.log
