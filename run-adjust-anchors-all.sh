#!/bin/bash

scale=$1
if [ ! -n "$scale" ] ; then
	echo "Usage: $0 <scale>"
	exit
fi

find bb3_release -name '*.abs.anc' -or -name '*.q3.anc' -or -name '*.sov.anc' \
| time xargs -I {} -n 1 ./run-adjust-anchors.sh {} $scale
